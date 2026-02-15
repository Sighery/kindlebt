#include <kindlebt/kindlebt.h>

#include <pthread.h>
#include <stdbool.h>

#include <ace/bluetooth_ble_api.h>
#include <ace/bluetooth_ble_gatt_client_api.h>
#include <ace/bluetooth_common_api.h>
#include <ace/bluetooth_session_api.h>

#include "log.h"

#include <kindlebt/compat_ace.h>
#include <kindlebt/kindlebt_defines.h>

#include "kindlebt_callbacks.c"

static bleCallbacks_t ble_callbacks = {
    .size = sizeof(bleCallbacks_t),
    .common_cbs =
        {
            .size = sizeof(commonCallbacks_t),
            .adapter_state_cb = adapterStateCallback,
            .bond_state_cb = bondStateCallback,
        },
    .ble_registered_cb = bleRegCallback,
    .connection_state_change_cb = bleConnStateChangedCallback,
    .on_ble_mtu_updated_cb = bleMtuUpdatedCallback,
};

static bleGattClientCallbacks_t gatt_client_callbacks = {
    .size = sizeof(bleGattClientCallbacks_t),
    .on_ble_gattc_service_discovered_cb = bleGattcServiceDiscoveredCallbackWrapper,
    .on_ble_gattc_get_gatt_db_cb = bleGattcGetDbCallbackWrapper,
    .notify_characteristics_cb = bleGattcNotifyCharsCallbackWrapper,
    .on_ble_gattc_read_characteristics_cb = bleGattcReadCharsCallbackWrapper,
    .on_ble_gattc_write_characteristics_cb = bleGattcWriteCharsCallbackWrapper,
};

bool isBLESupported(void) { return aceBT_isBLESupported(); };

status_t enableRadio(sessionHandle session_handle) { return aceBT_enableRadio(session_handle); }
status_t disableRadio(sessionHandle session_handle) { return aceBT_disableRadio(session_handle); }
status_t getRadioState(state_t* p_out_state) { return aceBT_getRadioState(p_out_state); }

sessionType_t getSupportedSession(void) { return aceBT_getSupportedSession(); }

status_t openSession(sessionType_t session_type, sessionHandle* session_handle) {
    return aceBT_openSession(session_type, NULL, session_handle);
}

status_t closeSession(sessionHandle session_handle) { return aceBT_closeSession(session_handle); }

status_t bleRegister(sessionHandle session_handle) {
    if (callback_vars.ble_registered) {
        log_warn("BLE is already registered\n");
        return ACE_STATUS_OK;
    }

    status_t ble_status = aceBT_bleRegister(session_handle, &ble_callbacks);
    if (ble_status != ACE_STATUS_OK) return ble_status;

    return waitForCondition(
        &callback_vars_lock, &callback_vars_cond, &callback_vars.ble_registered
    );
}

status_t bleDeregister(sessionHandle session_handle) {
    status_t ble_status = aceBT_bleDeRegister(session_handle);
    if (ble_status != ACE_STATUS_OK) return ble_status;

    setCallbackVariable(
        &callback_vars_lock, &callback_vars_cond, &callback_vars.ble_registered, false
    );

    return ble_status;
}

status_t bleRegisterGattClient(sessionHandle session_handle, bleGattClientCallbacks_t* callbacks) {
    // If provided a callback struct, replace the local copy
    // Applications can also just set the application callback directly
    if (callbacks != NULL) {
        application_gatt_client_callbacks = *callbacks;
    }

    return shim_bleRegisterGattClient(
        session_handle, &gatt_client_callbacks, ACE_BT_BLE_APPID_GADGETS
    );
}

status_t bleDeregisterGattClient(sessionHandle session_handle) {
    return shim_bleDeregisterGattClient(session_handle);
}

status_t bleDiscoverAllServices(sessionHandle session_handle, bleConnHandle conn_handle) {
    device_context_t* ctx = dca_find_by_handle(&devices_context, conn_handle);
    if (!ctx) {
        log_error("Couldn't find device context for conn_handle %p", conn_handle);
        return ACE_STATUS_UNINITIALIZED;
    }

    status_t services_status = shim_bleDiscoverAllServices(session_handle, conn_handle);
    if (services_status != ACE_STATUS_OK) return services_status;

    return waitForCondition(&ctx->lock, &ctx->cond, &ctx->gattc_discovered);
}

status_t bleGetDatabase(
    bleConnHandle conn_handle, bleGattsService_t** services_out, uint32_t* services_count
) {
    device_context_t* ctx = dca_find_by_handle(&devices_context, conn_handle);
    if (!ctx) {
        log_error("Couldn't find device context for conn_handle %p", conn_handle);
        return ACE_STATUS_UNINITIALIZED;
    }

    pthread_mutex_lock(&ctx->lock);
    ctx->gattdb_out = services_out;
    ctx->gattdb_count_out = services_count;
    pthread_mutex_unlock(&ctx->lock);

    status_t db_status = shim_bleGetDatabase(conn_handle);
    if (db_status != ACE_STATUS_OK) return db_status;

    return waitForCondition(&ctx->lock, &ctx->cond, &ctx->gattdb_retrieved);
}

status_t bleCloneGattService(
    bleGattsService_t** dst_gatt_service, const bleGattsService_t* src_gatt_service, int no_svc
) {
    return aceBT_bleCloneGattService(dst_gatt_service, src_gatt_service, no_svc);
}

status_t bleCleanupGattService(bleGattsService_t* service, int no_svc) {
    return shim_bleCleanupGattService(service, no_svc);
}

status_t bleConnect(
    sessionHandle session_handle, bleConnHandle* conn_handle, bdAddr_t* p_device,
    bleConnParam_t conn_param, bleConnRole_t conn_role, bleConnPriority_t conn_priority
) {
    device_context_t* ctx = dca_add_new(&devices_context);
    memcpy(ctx->address, p_device->address, MAC_ADDR_LEN);

    status_t conn_status =
        aceBt_bleConnect(session_handle, p_device, conn_param, conn_role, false, conn_priority);
    if (conn_status != ACE_STATUS_OK) return conn_status;

    status_t cond_status = waitForCondition(&ctx->lock, &ctx->cond, &ctx->gattc_connected);
    if (cond_status != ACE_STATUS_OK) return cond_status;

    pthread_mutex_lock(&ctx->lock);
    *conn_handle = ctx->handle;
    pthread_mutex_unlock(&ctx->lock);

    return ACE_STATUS_OK;
}

status_t bleDisconnect(bleConnHandle conn_handle) {
    device_context_t* ctx = dca_find_by_handle(&devices_context, conn_handle);
    if (!ctx) {
        log_error("Couldn't find device context for conn_handle %p", conn_handle);
        return ACE_STATUS_UNINITIALIZED;
    }

    status_t disconnect_status = aceBT_bleDisconnect(conn_handle);
    if (disconnect_status != ACE_STATUS_OK) return disconnect_status;

    status_t cond_status = waitForCondition(&ctx->lock, &ctx->cond, &ctx->gattc_disconnected);
    if (cond_status != ACE_STATUS_OK) return cond_status;

    bool dca_status = dca_remove(&devices_context, conn_handle);
    if (dca_status == false) {
        log_warn("Couldn't delete %p from the devices context array", conn_handle);
    }

    return ACE_STATUS_OK;
}

status_t bleReadCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value
) {
    return shim_bleReadCharacteristic(session_handle, conn_handle, chars_value);
}

status_t bleWriteCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
) {
    return shim_bleWriteCharacteristic(session_handle, conn_handle, chars_value, request_type);
}

status_t bleWriteDescriptor(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
) {
    return shim_bleWriteDescriptor(session_handle, conn_handle, chars_value, request_type);
}

status_t bleSetNotification(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value, bool enable
) {
    return shim_bleSetNotification(session_handle, conn_handle, chars_value, enable);
}
