#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "log.h"

#include "kindlebt.h"
#include "kindlebt_defines.h"
#include "kindlebt_log.h"

#include "kindlebt_application.c"
#include "kindlebt_utils.c"

pthread_mutex_t callback_vars_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t callback_vars_cond = PTHREAD_COND_INITIALIZER;
bleCallbackVars_t callback_vars;
bleConnHandle ble_conn_handle;
// GATT get DB global variables
uint32_t gNo_svc;
bleGattsService_t* pGgatt_service = NULL;

void adapterStateCallback(state_t state) {
    if (state == ACEBT_STATE_ENABLED) {
        log_debug("Callback %s(): state: STATE_ENABLED", __func__);
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.bt_enabled, true
        );
    } else if (state == ACEBT_STATE_DISABLED) {
        log_debug("Callback %s(): state: STATE_DISABLED", __func__);
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.bt_enabled, false
        );
    }
}

void bondStateCallback(status_t status, bdAddr_t* p_remote_addr, aceBT_bondState_t state) {
    char addr[MAC_ADDR_STR_LEN];
    memset(addr, 0, MAC_ADDR_STR_LEN);
    utilsConvertBdAddrToStr(p_remote_addr, addr);

    switch (state) {
    case ACEBT_BOND_STATE_NONE:
        log_debug("Callback %s(): status: %d addr: %s state: NONE", __func__, status, addr);
        break;
    case ACEBT_BOND_STATE_BONDING:
        log_debug("Callback %s(): status: %d addr: %s state: BONDING", __func__, status, addr);
        break;
    case ACEBT_BOND_STATE_BONDED:
        log_debug("Callback %s(): status: %d addr: %s state: BONDED", __func__, status, addr);
        break;
    default:
        log_debug(
            "Callback %s(): status: %d addr: %s state: UNKNOWN(%d)", __func__, status, addr, state
        );
        break;
    }
}

void bleMtuUpdatedCallback(status_t status, bleConnHandle conn_handle, int mtu) {
    log_debug(
        "Callback %s(): status %d, mtu %d, conn_handle %p", __func__, status, mtu, conn_handle
    );

    if (status == ACE_STATUS_OK)
        setCallbackVariable(&callback_vars_lock, &callback_vars_cond, &callback_vars.mtu_set, true);
}

void bleRegCallback(status_t status) {
    log_debug("Callback %s(): status: %d\n", __func__, status);

    if (status == ACE_STATUS_OK)
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.ble_registered, true
        );
}

void bleConnStateChangedCallback(
    bleConnState_t state, gattStatus_t status, const bleConnHandle conn_handle, bdAddr_t* p_addr
) {
    char addr[MAC_ADDR_STR_LEN];
    memset(addr, 0, MAC_ADDR_STR_LEN);
    utilsConvertBdAddrToStr(p_addr, addr);
    log_debug(
        "Callback %s(): state %d, status %d, conn_handle %p addr %s", __func__, state, status,
        conn_handle, addr
    );

    ble_conn_handle = conn_handle;

    if (status == ACEBT_GATT_STATUS_SUCCESS) {
        if (state == ACEBT_BLE_STATE_CONNECTED) {
            log_info("BLE device %s connected", addr);
            setCallbackVariable(
                &callback_vars_lock, &callback_vars_cond, &callback_vars.gattc_connected, true
            );
            setCallbackVariable(
                &callback_vars_lock, &callback_vars_cond, &callback_vars.gattc_disconnected, false
            );
        } else if (state == ACEBT_BLE_STATE_DISCONNECTED) {
            log_info("BLE device %s disconnected", addr);
            setCallbackVariable(
                &callback_vars_lock, &callback_vars_cond, &callback_vars.gattc_connected, false
            );
            setCallbackVariable(
                &callback_vars_lock, &callback_vars_cond, &callback_vars.gattc_disconnected, true
            );
        }
    }
}

void bleGattcServiceDiscoveredCallback(bleConnHandle conn_handle, status_t status) {
    log_debug("Callback %s(): conn_handle %p status %d", __func__, conn_handle, status);

    if (status == ACE_STATUS_OK) {
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.gattc_discovered, true
        );
    }
}

void bleGattcGetDbCallback(
    bleConnHandle conn_handle, bleGattsService_t* gatt_service, uint32_t no_svc
) {
    log_debug("Callback %s(): conn_handle %p no_svc %" PRIu32 "", __func__, conn_handle, no_svc);

    gNo_svc = no_svc;
    status_t status = bleCloneGattService(&pGgatt_service, gatt_service, gNo_svc);

    if (status != ACE_STATUS_OK) {
        log_error("Error copying GATT Database %d\n", status);
        return;
    }

    size_t size = 1024, offset = 0;
    char* log_buff = malloc(size);
    if (!log_buff) return;

    for (uint32_t i = 0; i < no_svc; i++) {
        log_buff = append_to_buffer(
            log_buff, &size, &offset, "GATT Database index :%" PRIu32 " %p\n", i, &pGgatt_service[i]
        );
        log_buff = utilsDumpServer(&pGgatt_service[i], log_buff, &size, &offset);
    }

    log_debug("%s", log_buff);
    free(log_buff);

    setCallbackVariable(&callback_vars_lock, &callback_vars_cond, &callback_vars.got_gatt_db, true);
}

void bleGattcNotifyCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value
) {
    log_debug("Callback %s(): conn_handle %p", __func__, conn_handle);

    char buff[256];
    utilsPrintUuid(buff, &chars_value.gattRecord.uuid, 256);
    log_debug("%s() UUID:: %s", __func__, buff);

    size_t size = 1024, offset = 0;
    char* log_buff = malloc(size);
    if (!log_buff) return;

    log_buff = append_to_buffer(log_buff, &size, &offset, "%s() DATA:: ", __func__);
    for (int idx = 0; idx < chars_value.blobValue.size; idx++)
        log_buff =
            append_to_buffer(log_buff, &size, &offset, "%x", chars_value.blobValue.data[idx]);

    log_debug("%s", log_buff);
    free(log_buff);
}

void bleGattcReadCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value, status_t status
) {
    log_debug("Callback %s(): status %d conn_handle %p", __func__, status, conn_handle);

    char buff[256];
    utilsPrintUuid(buff, &chars_value.gattRecord.uuid, 256);
    log_debug("%s() UUID:: %s", __func__, buff);

    size_t size = 1024, offset = 0;
    char* log_buff = malloc(size);
    if (!log_buff) return;

    log_buff = append_to_buffer(log_buff, &size, &offset, "%s() DATA:: ", __func__);
    for (int idx = 0; idx < chars_value.blobValue.size; idx++)
        log_buff =
            append_to_buffer(log_buff, &size, &offset, "%x", chars_value.blobValue.data[idx]);

    log_debug("%s", log_buff);
    free(log_buff);
}

void bleGattcWriteCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t gatt_characteristics, status_t status
) {
    log_debug(
        "Callback %s(): conn_handle %p GATT format %u", __func__, conn_handle,
        gatt_characteristics.format
    );
}

// Wrappers needed for when we need to share callbacks between library and application
void bleGattcServiceDiscoveredCallbackWrapper(bleConnHandle conn_handle, status_t status) {
    bleGattcServiceDiscoveredCallback(conn_handle, status);

    if (application_gatt_client_callbacks.on_ble_gattc_service_discovered_cb != NULL) {
        application_gatt_client_callbacks.on_ble_gattc_service_discovered_cb(conn_handle, status);
    }
}

void bleGattcGetDbCallbackWrapper(
    bleConnHandle conn_handle, bleGattsService_t* gatt_service, uint32_t no_svc
) {
    bleGattcGetDbCallback(conn_handle, gatt_service, no_svc);

    if (application_gatt_client_callbacks.on_ble_gattc_get_gatt_db_cb != NULL) {
        application_gatt_client_callbacks.on_ble_gattc_get_gatt_db_cb(
            conn_handle, gatt_service, no_svc
        );
    }
}

void bleGattcNotifyCharsCallbackWrapper(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value
) {
    // bleGattcNotifyCharsCallback(conn_handle, chars_value);

    if (application_gatt_client_callbacks.notify_characteristics_cb != NULL) {
        application_gatt_client_callbacks.notify_characteristics_cb(conn_handle, chars_value);
    }
}

void bleGattcReadCharsCallbackWrapper(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value, status_t status
) {
    // bleGattcReadCharsCallback(conn_handle, chars_value, status);

    if (application_gatt_client_callbacks.on_ble_gattc_read_characteristics_cb != NULL) {
        application_gatt_client_callbacks.on_ble_gattc_read_characteristics_cb(
            conn_handle, chars_value, status
        );
    }
}

void bleGattcWriteCharsCallbackWrapper(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t gatt_characteristics, status_t status
) {
    // bleGattcWriteCharsCallback(conn_handle, gatt_characteristics, status);

    if (application_gatt_client_callbacks.on_ble_gattc_write_characteristics_cb != NULL) {
        application_gatt_client_callbacks.on_ble_gattc_write_characteristics_cb(
            conn_handle, gatt_characteristics, status
        );
    }
}
