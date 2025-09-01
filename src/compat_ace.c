#include "compat_ace.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>

#include "log.h"

// TODO: Might want to turn it into a bitmask instead
acebt_abi acebt_abi_version(void) {
    static acebt_abi cached_abi = PRE_5170;
    static bool initialized = false;

    if (!initialized) {
        void* handle = dlopen(NULL, RTLD_LAZY);
        if (handle) {
            dlerror();
            void* sym = dlsym(handle, "aceBt_bleRegisterGattClient");
            if (sym) {
                cached_abi = SINCE_5170;
            }
        } else {
            log_error("dlopen failed: %s", dlerror());
        }
        initialized = true;
    }

    return cached_abi;
}

void dump_hex(const void* ptr, size_t size) {
    const unsigned char* data = (const unsigned char*)ptr;

    // Each byte is "XX ", 3 chars. One '\n' every 16 bytes, and a final '\0'
    size_t lines = (size + 15) / 16;
    size_t buf_len = (size * 3) + lines + 1;
    char* buf = malloc(buf_len);
    if (!buf) return;

    char* p = buf;
    for (size_t i = 0; i < size; ++i) {
        p += snprintf(p, buf + buf_len - p, "%02x ", data[i]);

        // Newline every 16 bytes
        if ((i + 1) % 16 == 0) *p++ = '\n';
    }
    *p = '\0';
    log_debug("Dumping hex:\n%s", buf);
    free(buf);
}

void dump_registerCbackGattcData(const registerCbackGattcData_t* data) {
    if (data == NULL) {
        log_warn("Dumping registerCbackGattcData_t: NULL\n");
        return;
    }

    log_warn(
        "Dumping registerCbackGattcData_t {\n"
        "  size: %u\n"
        "  session_handle: %p\n"
        "  callback_mask: %u\n"
        "  is_unregister: %u\n"
        "  app_id: %d\n"
        "  out_status: %d\n"
        "}",
        data->size, data->session_handle, data->callback_mask, data->is_unregister, data->app_id,
        data->out_status
    );
}

void dump_mask_bits(uint16_t mask) {
    const int len = 64;
    char* buf = malloc(len);
    if (!buf) return;

    char* p = buf;
    for (int i = 15; i >= 0; --i) {
        *p++ = (mask & (1u << i)) ? '1' : '0';
        // Add spacing every 4 bits
        if (i % 4 == 0 && i != 0) *p++ = ' ';
    }
    *p = '\0';

    log_debug("Dumping mask: 0x%04x (binary: %s)", mask, buf);
    free(buf);
}

void dump_aipc_handle(aipcHandles_t aipc_handle) {
    log_debug(
        "aipc_handle {\n  callback_server_id: %u (0x%04x),\n  server_id: %u (0x%04x)\n}",
        aipc_handle.callback_server_id, aipc_handle.callback_server_id, aipc_handle.server_id,
        aipc_handle.server_id
    );
}

void pre5170_gattc_cb_handler(aceAipc_parameter_t* task) {
    log_debug("Called into pre 5.17.0 %s", __func__);

    if (task == NULL) {
        log_info("[%s()]: Server handler callback: data is null", __func__);
        return;
    }

    printf("Dumping aceAipc_parameter_t memory:\n");
    dump_hex(task, 128);

    /* In AIPC callback this runs in server side callback context. Hence use the
       callback id to retrive the session info*/
    sessionHandle session_handle = getSessionForTask(task);
    bleGattClientCallbacks_t* p_client_callbacks =
        getBTClientData(session_handle, CALLBACK_INDEX_BLE_GATT_CLIENT);
    if (session_handle == NULL || p_client_callbacks == NULL) {
        log_error(
            "[%s()]: Error invalid handle, session %p callback %p", __func__, session_handle,
            p_client_callbacks
        );
        return;
    }

    // TODO: Actually implement these callbacks
    switch ((ipc_evt_enum_t)task->function_id) {
    case ACE_BT_CALLBACK_GATTC_SERVICE_REGISTERED: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_SERVICE_REGISTERED");
    } break;
    case ACE_BT_CALLBACK_GATTC_SERVICE_DISCOVERED: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_SERVICE_DISCOVERED");
    } break;
    case ACE_BT_CALLBACK_GATTC_CHARS_READ_RSP: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_CHARS_READ_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_CHARS_WRITE_RSP: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_CHARS_WRITE_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_EXEC_WRITE_RSP: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_EXEC_WRITE_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_NOTIFY_CHARS_CHANGED: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_NOTIFY_CHARS_CHANGED");
    } break;
    case ACE_BT_CALLBACK_GATTC_DESC_WRITE_RSP: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_DESC_WRITE_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_DESC_READ_RSP: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_DESC_READ_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_GET_DB_RSP: {
        log_warn("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_GET_DB_RSP");
    } break;
    default:
        log_warn("[%s()]: Unknown GATT Client callback type %d", __func__, task->function_id);
        break;
    }

    return;
}

/**
 * Static local helper function to create a callback mask based on the
 * p_callbacks passed by app client.
 *
 * @param p_callbacks pointer to callbacks struct bleGattClientCallbacks_t
 * @return uint16_t callback supports up to 16 mask
 */
static uint16_t create_client_callback_mask(bleGattClientCallbacks_t* p_callbacks) {
    uint16_t cback_mask = 0;
    if (p_callbacks == NULL || p_callbacks->size == 0) return cback_mask;
    cback_mask |= p_callbacks->on_ble_gattc_service_registered_cb
                      ? ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_REGISTERED
                      : 0;
    cback_mask |= p_callbacks->on_ble_gattc_service_discovered_cb
                      ? ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_DISCOVERED
                      : 0;
    cback_mask |= p_callbacks->on_ble_gattc_read_characteristics_cb
                      ? ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_READ_CHARS
                      : 0;
    cback_mask |= p_callbacks->on_ble_gattc_write_characteristics_cb
                      ? ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_WRITE_CHARS
                      : 0;
    cback_mask |= p_callbacks->notify_characteristics_cb
                      ? ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_NOTIFY_CHARS
                      : 0;
    cback_mask |= p_callbacks->on_ble_gattc_write_descriptor_cb
                      ? ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_DESC_WRITE
                      : 0;
    cback_mask |= p_callbacks->on_ble_gattc_read_descriptor_cb
                      ? ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_DESC_READ
                      : 0;
    cback_mask |= p_callbacks->on_ble_gattc_get_gatt_db_cb ? ACEBT_IPC_CBACK_MASK_GATTC_GET_DB : 0;
    cback_mask |=
        p_callbacks->on_ble_gattc_execute_write_cb ? ACEBT_IPC_CBACK_MASK_GATTC_EXECUTE_WRITE : 0;
    return cback_mask;
}

status_t pre5170_bleRegisterGattClient(
    sessionHandle session_handle, bleGattClientCallbacks_t* callbacks, bleAppId_t app_id
) {
    status_t status;
    uint16_t mask = 0;
    aipcHandles_t aipc_handle;
    registerCbackGattcData_t manager_callbacks;

    status = getSessionInfo(session_handle, &aipc_handle);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Couldn't get session info. Result %d", __func__, status);
        return ACE_STATUS_BAD_PARAM;
    }

    dump_aipc_handle(aipc_handle);

    mask = create_client_callback_mask(callbacks);
    dump_mask_bits(mask);

    // In theory this should match session_handle exactly
    uint32_t temp_aipc = (aipc_handle.server_id << 16) + aipc_handle.callback_server_id;
    log_debug("[%s()]: temp_aipc %u (0x%04x)", __func__, temp_aipc, temp_aipc);
    aceBt_serializeGattcRegisterData(&manager_callbacks, temp_aipc, mask, app_id);
    log_debug("[%s()]: Register GATT Client session handle %p", __func__, session_handle);

    dump_registerCbackGattcData(&manager_callbacks);

    status = registerBTClientData(session_handle, CALLBACK_INDEX_BLE_GATT_CLIENT, (void*)callbacks);
    log_debug("[%s()]: registerBTClientData step. Result: %d", __func__, status);
    if (status != ACEBT_STATUS_SUCCESS) return status;

    status = registerBTEvtHandler(
        session_handle, &pre5170_gattc_cb_handler, ACE_BT_CALLBACK_GATTC_INIT,
        ACE_BT_CALLBACK_GATTC_MAX
    );
    log_debug("[%s()]: registerBTEvtHandler step. Result: %d", __func__, status);
    if (status != ACEBT_STATUS_SUCCESS) return status;

    status = aipc_invoke_sync_call(
        ACE_BT_BLE_REGISTER_GATT_CLIENT_API, (void*)&manager_callbacks, manager_callbacks.size
    );
    if (status != ACE_STATUS_OK) {
        log_error(
            "[%s()]: failed to register gatt client callbacks with server! result: %d", __func__,
            status
        );
        registerBTClientData(session_handle, CALLBACK_INDEX_BLE_GATT_CLIENT, NULL);
        registerBTEvtHandler(
            session_handle, NULL, ACE_BT_CALLBACK_GATTC_INIT, ACE_BT_CALLBACK_GATTC_MAX
        );
    }

    return status;
}

status_t pre5170_bleDeregisterGattClient(sessionHandle session_handle) {
    status_t status;
    aipcHandles_t aipc_handle;
    request_unreg_t unregister;
    unregister = (request_unreg_t){.session_handle = (uint32_t)session_handle,
                                   .size = sizeof(request_unreg_t)};

    status = getSessionInfo(session_handle, &aipc_handle);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Couldn't get session info. Result %d", __func__, status);
        return ACE_STATUS_BAD_PARAM;
    }

    dump_aipc_handle(aipc_handle);

    status = aipc_invoke_sync_call(
        ACE_BT_BLE_UNREGISTER_GATT_CLIENT_API, (void*)&unregister, unregister.size
    );
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Failed to send AIPC call %d", __func__, status);
    }

    return status;
}
