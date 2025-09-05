#define _GNU_SOURCE

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

/* Internal typedefs matching the two getSessionFor* functions
 * getSessionForCallback < 5.17
 * getSessionForTask >= 5.17
 * Our shim is getSessionFromHandler
 */
typedef sessionHandle (*getSessionForCallback_fn_t)(uint16_t);
typedef sessionHandle (*getSessionForTask_fn_t)(aceAipc_parameter_t*);

sessionHandle getSessionFromHandler(aceAipc_parameter_t* task) {
    static getSessionForCallback_fn_t old_api = NULL;
    static getSessionForTask_fn_t new_api = NULL;
    static bool initialized = false;

    if (!initialized) {
        new_api = (getSessionForTask_fn_t)dlsym(RTLD_DEFAULT, "getSessionForTask");
        if (!new_api) {
            old_api = (getSessionForCallback_fn_t)dlsym(RTLD_DEFAULT, "getSessionForCallback");
        }
        initialized = true;
    }

    if (new_api) {
        return new_api(task);
    } else if (old_api) {
        return old_api(task->callback_id);
    } else {
        // Nothing matched. We shouldn't reach this
        log_error("[%s()]: couldn't match any of the expected getSessionFor* symbols", __func__);
        return (sessionHandle)-1;
    }
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
        log_warn("Dumping registerCbackGattcData_t: NULL");
        return;
    }

    log_debug(
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

void dump_gattc_get_db_data_t(const gattc_get_db_data_t* data) {
    if (data == NULL) {
        log_warn("Dumping gattc_get_db_data_t: NULL");
        return;
    }

    log_debug(
        "Dumping gattc_get_db_data_t {\n"
        "  size: %d\n"
        "  conn_handle: %p\n"
        "  uuid: %p\n"
        "}",
        data->size, data->conn_handle, data->uuid
    );
}

void dump_aceAipc_parameter_t(aceAipc_parameter_t* task) {
    if (task == NULL) {
        log_warn("Dumping aceAipc_parameter_t: NULL");
        return;
    }

    log_debug(
        "Dumping aceAipc_parameter_t {\n"
        "  reserved0 %u (%p)\n"
        "  size %u (%p)\n"
        "  msg_type %u (%p)\n"
        "  function_id %u (%p)\n"
        "  cb1 %p\n"
        "  cb2 %p\n"
        "  buffer %p\n"
        "  reserved1 %u (%p)\n"
        "  reserved2 %u (%p)\n"
        "  reserved3 %u (%p)\n"
        "  flags %u (%p)\n"
        "  reserved4 %u (%p)\n"
        "  callback_id %u (%p)\n"
        "  server_id %u (%p)\n"
        "}",
        task->reserved0, task->reserved0, task->size, task->size, task->msg_type, task->msg_type,
        task->function_id, task->function_id, task->cb1, task->cb2, task->buffer, task->reserved1,
        task->reserved1, task->reserved2, task->reserved2, task->reserved3, task->reserved3,
        task->flags, task->flags, task->reserved4, task->reserved4, task->callback_id,
        task->callback_id, task->server_id, task->server_id
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
    dump_aceAipc_parameter_t(task);

    /* In AIPC callback this runs in server side callback context. Hence use the
       callback id to retrive the session info*/
    sessionHandle session_handle = getSessionFromHandler(task);
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
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_SERVICE_REGISTERED");
        log_debug("Not actually implemented at all in acebt, so ignore this log");
    } break;
    case ACE_BT_CALLBACK_GATTC_SERVICE_DISCOVERED: {
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_SERVICE_DISCOVERED");
    } break;
    case ACE_BT_CALLBACK_GATTC_CHARS_READ_RSP: {
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_CHARS_READ_RSP");
        if (p_client_callbacks->on_ble_gattc_read_characteristics_cb == NULL) {
            log_error("[%s()]: on_ble_gattc_read_characteristics_cb not implemented", __func__);
            break;
        }

        gattc_read_chars_data_t* data = (gattc_read_chars_data_t*)task->buffer;

        // TODO: Decompiled code does way more assignments. So there is a chance I'm not filling
        // the whole struct
        data->value.blobValue.data = data->data;
        uint32_t len32 = data->data_len;
        if (len32 > UINT16_MAX) {
            log_error("[%s()]: data_len too big: %d", __func__, data->data_len);
            len32 = UINT16_MAX;
        }
        data->value.blobValue.size = (uint16_t)len32;
        data->value.blobValue.offset = 0;
        data->value.format = BLE_FORMAT_BLOB;

        p_client_callbacks->on_ble_gattc_read_characteristics_cb(
            (bleConnHandle)data->conn_handle, data->value, data->status
        );
    } break;
    case ACE_BT_CALLBACK_GATTC_CHARS_WRITE_RSP: {
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_CHARS_WRITE_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_EXEC_WRITE_RSP: {
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_EXEC_WRITE_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_NOTIFY_CHARS_CHANGED: {
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_NOTIFY_CHARS_CHANGED");
    } break;
    case ACE_BT_CALLBACK_GATTC_DESC_WRITE_RSP: {
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_DESC_WRITE_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_DESC_READ_RSP: {
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_DESC_READ_RSP");
    } break;
    case ACE_BT_CALLBACK_GATTC_GET_DB_RSP: {
        log_debug("BLE GATTC callback handler, case ACE_BT_CALLBACK_GATTC_GET_DB_RSP");
        if (p_client_callbacks->on_ble_gattc_get_gatt_db_cb == NULL) {
            log_error("[%s()]: on_ble_gattc_get_gatt_db_callback not implemented", __func__);
            break;
        }

        bleGattsService_t* gatt_service;
        uint32_t no_svc;
        register_cback_gatts_list_t* data = (register_cback_gatts_list_t*)task->buffer;

        deserealize_all_gatts_register_data(data, &gatt_service, &no_svc);
        p_client_callbacks->on_ble_gattc_get_gatt_db_cb(
            (bleConnHandle)data->conn_handle, gatt_service, no_svc
        );
        cleanup_all_service(gatt_service, no_svc);
    } break;
    default:
        log_error("[%s()]: Unknown GATT Client callback type %d", __func__, task->function_id);
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
        session_handle, pre5170_gattc_cb_handler, ACE_BT_CALLBACK_GATTC_INIT,
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
    gatt_request_unreg_t unregister;

    status = getSessionInfo(session_handle, &unregister.h1);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Couldn't get session info. Result %d", __func__, status);
        return ACE_STATUS_BAD_PARAM;
    }

    dump_aipc_handle(unregister.h1);

    unregister.h2 = unregister.h1;
    unregister.size = sizeof(unregister);

    status =
        aipc_invoke_sync_call(ACE_BT_BLE_UNREGISTER_GATT_CLIENT_API, &unregister, unregister.size);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Failed to send AIPC call %d", __func__, status);
    }

    return status;
}

status_t pre5170_bleGetService(bleConnHandle conn_handle) {
    log_debug("Called into pre 5.17.0 %s", __func__);

    status_t status;
    gattc_get_db_data_t data;

    serialize_ble_get_db_req(&data, (uint32_t)conn_handle, NULL);

    dump_gattc_get_db_data_t(&data);

    status = aipc_invoke_sync_call(ACE_BT_BLE_GATT_CLIENT_GET_SERVICE_API, &data, data.size);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Failed to send AIPC call. Status: %d", __func__, status);
    }
    return status;
}

status_t pre5170_bleReadCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value
) {
    log_debug("Called into pre 5.17.0 %s", __func__);

    status_t status;
    acebt_gattc_read_chars_req_data_t data;

    serialize_gattc_read_chars_req(&data, (uint32_t)conn_handle, chars_value);

    log_debug("[%s()]: Serialize request, status: %d", __func__, data.status);

    status = aipc_invoke_sync_call(ACE_BT_BLE_GATT_CLIENT_READ_CHARS_API, &data, data.size);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Failed to send AIPC call. Status: %d", __func__, status);
    }
    return status;
}
