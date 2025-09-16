#include "compat_ace_implementations.h"

#include <endian.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "compat_ace_handler.h"
#include "compat_ace_internals.h"
#include "compat_ace_utils.h"
#include "compat_acealloc.h"

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

status_t pre5170_bleDiscoverAllServices(sessionHandle session_handle, bleConnHandle conn_handle) {
    log_debug("Called into pre 5.17 %s", __func__);

    status_t status;
    request_disc_all_svc_t data;

    serialize_gattc_disc_all_svc(&data, (uint32_t)conn_handle);
    status = aipc_invoke_sync_call(ACE_BT_BLE_GATT_CLIENT_DISC_ALL_SVC_API, &data, data.size);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Failed to send AIPC call. Status: %d", __func__, status);
    }
    return data.out_status;
}

status_t pre5170_bleGetService(bleConnHandle conn_handle) {
    log_debug("Called into pre 5.17 %s", __func__);

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
    log_debug("Called into pre 5.17 %s", __func__);

    status_t status;
    aipcHandles_t handle;

    status = getSessionInfo(session_handle, &handle);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Couldn't get session info. Result: %d", __func__, status);
        return ACE_STATUS_BAD_PARAM;
    }

    acebt_gattc_read_chars_req_data_t data;
    serialize_gattc_read_chars_req(&data, (uint32_t)conn_handle, chars_value);
    log_debug("[%s()]: Serialize request, status: %d", __func__, data.status);

    status = aipc_invoke_sync_call(ACE_BT_BLE_GATT_CLIENT_READ_CHARS_API, &data, data.size);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Failed to send AIPC call. Status: %d", __func__, status);
    }
    return status;
}

status_t pre5170_bleWriteCharacteristics(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
) {
    log_debug("Called into pre 5.17 %s", __func__);

    status_t status;
    aipcHandles_t handle;

    status = getSessionInfo(session_handle, &handle);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Couldn't get session info. Result: %d", __func__, status);
        return ACE_STATUS_BAD_PARAM;
    }

    uint8_t* out_data;
    uint32_t out_len;

    serialize_gattc_write_char_req(
        (uint32_t)conn_handle, chars_value, &out_data, &out_len, request_type
    );

    if (out_data == NULL) {
        log_error("[%s()]: Failed serialize_gattc_write_char_req", __func__);
        return ACE_STATUS_GENERAL_ERROR;
    }

    status = aipc_invoke_sync_call(ACE_BT_BLE_GATT_CLIENT_WRITE_CHARS_API, out_data, out_len);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Failed to send AIPC call. Status: %d", __func__, status);
        return status;
    }

    shadow_aceAlloc_free(out_data);

    return status;
}

status_t pre5170_bleWriteDescriptor(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
) {
    log_debug("Called into pre 5.17 %s", __func__);

    status_t status;
    aipcHandles_t handle;

    status = getSessionInfo(session_handle, &handle);
    if (status != ACE_STATUS_OK) {
        log_error("[%s()]: Couldn't get session info. Result: %d", __func__, status);
        return ACE_STATUS_BAD_PARAM;
    }

    uint8_t* out_data;
    uint32_t out_len;

    serialize_gattc_write_desc_req(
        (uint32_t)conn_handle, &chars_value->gattDescriptor, &out_data, &out_len, request_type
    );
    if (out_data == NULL) {
        log_error("[%s()]: serialize_gattc_write_desc_req() failed", __func__);
        return ACE_STATUS_GENERAL_ERROR;
    }

    status = aipc_invoke_sync_call(ACE_BT_BLE_GATT_CLIENT_WRITE_DESC_API, out_data, out_len);
    if (status == ACE_STATUS_OK) {
        status = ((const gattc_write_desc_data_t*)out_data)->status;
    } else {
        log_error("[%s()]: Failed to send AIPC call. Status: %d", __func__, status);
        status = ACE_STATUS_BAD_PARAM;
    }

    shadow_aceAlloc_free(out_data);

    return status;
}
