#include "compat_ace_handler.h"

#include "log.h"

#include "compat_ace_internals.h"
#include "compat_ace_shims.h"
#include "compat_ace_utils.h"

void pre5170_gattc_cb_handler(aceAipc_parameter_t* task) {
    log_debug("Called into pre 5.17 %s", __func__);

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
        if (p_client_callbacks->on_ble_gattc_service_discovered_cb == NULL) {
            log_error("[%s()]: on_ble_gattc_service_discovered_cb not implemented", __func__);
            break;
        }

        dis_req_t* data = (dis_req_t*)task->buffer;
        p_client_callbacks->on_ble_gattc_service_discovered_cb(
            (bleConnHandle)data->conn_handle, data->out_status
        );
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
        if (p_client_callbacks->on_ble_gattc_write_characteristics_cb == NULL) {
            log_error("[%s()]: on_ble_gattc_write_characteristics_cb not implemented", __func__);
            break;
        }

        gattc_write_chars_data_t* data = (gattc_write_chars_data_t*)task->buffer;

        // TODO: Decompiled code does way more assignments. This might not be feature complete
        data->value.blobValue.data = data->data;
        uint32_t len32 = data->data_len;
        if (len32 > UINT16_MAX) {
            log_error("[%s()]: data_len too big: %d", __func__, data->data_len);
            len32 = UINT16_MAX;
        }
        data->value.blobValue.size = (uint16_t)len32;
        data->value.blobValue.offset = 0;
        data->value.format = BLE_FORMAT_BLOB;

        p_client_callbacks->on_ble_gattc_write_characteristics_cb(
            (bleConnHandle)data->conn_handle, data->value, data->status
        );
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
