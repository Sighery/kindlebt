#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "kindlebt.h"
#include "kindlebt_defines.h"

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
        printf("Callback: adapter_state_callback() state: STATE_ENABLED\n");
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.bt_enabled, true
        );
    } else if (state == ACEBT_STATE_DISABLED) {
        printf("Callback: adapter_state_callback() state: STATE_DISABLED\n");
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
        printf("Callback: bond state changed() status: %d addr: %s state: NONE\n", status, addr);
        break;
    case ACEBT_BOND_STATE_BONDING:
        printf("Callback: bond state changed() status: %d addr: %s state: BONDING\n", status, addr);
        break;
    case ACEBT_BOND_STATE_BONDED:
        printf("Callback: bond state changed() status: %d addr: %s state: BONDED\n", status, addr);
        break;
    default:
        printf(
            "Callback: bond state changed() status: %d addr: %s state: UNKNOWN(%d)\n", status, addr,
            state
        );
        break;
    }
}

void bleMtuUpdatedCallback(status_t status, bleConnHandle conn_handle, int mtu) {
    printf("Callback: bleMtuUpdatedCallback() status: %d ", status);
    printf("mtu %d, conn_handle %p\n", mtu, conn_handle);

    if (status == ACE_STATUS_OK)
        setCallbackVariable(&callback_vars_lock, &callback_vars_cond, &callback_vars.mtu_set, true);
}

void bleRegCallback(status_t status) {
    printf("Callback: bleRegCallback() status: %d\n", status);

    if (status == ACE_STATUS_OK)
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.ble_registered, true
        );
}

void bleConnStateChangedCallback(
    bleConnState_t state, gattStatus_t status, const bleConnHandle conn_handle, bdAddr_t* p_addr
) {
    printf("Callback: bleConnStateChangedCallback() ");
    printf(
        "state %d status %d conn_handle %p addr %02x\n", state, status, conn_handle,
        p_addr->address[5]
    );

    ble_conn_handle = conn_handle;

    if (status == ACEBT_GATT_STATUS_SUCCESS) {
        if (state == ACEBT_BLE_STATE_CONNECTED) {
            printf("BLE device connected\n");
            setCallbackVariable(
                &callback_vars_lock, &callback_vars_cond, &callback_vars.gattc_connected, true
            );
        } else if (state == ACEBT_BLE_STATE_DISCONNECTED) {
            printf("BLE device disconnected\n");
            setCallbackVariable(
                &callback_vars_lock, &callback_vars_cond, &callback_vars.gattc_disconnected, true
            );
        }
    }
}

void bleGattcGetDbCallback(
    bleConnHandle conn_handle, bleGattsService_t* gatt_service, uint32_t no_svc
) {
    printf("CLI callback : aceBt_bleGattcGetDbCallback() ");
    printf("connHandle %p no_svc %" PRIu32 "\n", conn_handle, no_svc);

    gNo_svc = no_svc;
    status_t status = bleCloneGattService(&pGgatt_service, gatt_service, gNo_svc);

    if (status == ACE_STATUS_OK) {
        for (uint32_t i = 0; i < no_svc; i++) {
            printf("GATT Database index :%" PRIu32 " %p \n", i, &pGgatt_service[i]);
            utilsDumpServer(&pGgatt_service[i]);
        }

        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.got_gatt_db, true
        );
    } else {
        printf("Error copying GATT Database %d\n", status);
    }
}

void bleGattcNotifyCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value
) {
    printf("CLI callback: %s()\n", __func__);
    printf("conn_handle %p\n", conn_handle);
    char buff[256];
    utilsPrintUuid(buff, &chars_value.gattRecord.uuid, 256);
    printf("UUID:: %s\n", buff);
    for (int idx = 0; idx < chars_value.blobValue.size; idx++)
        printf("%x", chars_value.blobValue.data[idx]);
    printf("\n");
}

void bleGattcReadCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value, status_t status
) {
    printf("CLI callback: bleGattcReadCharsCallback() status: %d\n", status);
    printf("bleGattcReadCharsCallback - conn_handle %p\n", conn_handle);

    char buff[256];
    utilsPrintUuid(buff, &chars_value.gattRecord.uuid, 256);
    printf("bleGattcReadCharsCallback - Characteristic UUID:: %s\n", buff);

    printf("bleGattcReadCharsCallback - Characteristic value: ");
    for (int idx = 0; idx < chars_value.blobValue.size; idx++)
        printf("%02x", chars_value.blobValue.data[idx]);
    printf("\n");
}

void bleGattcWriteCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t gatt_characteristics, status_t status
) {
    printf("CLI callback: bleGattcWriteCharsCallback()\n");
    printf("conn_handle %p gatt format %u\n", conn_handle, gatt_characteristics.format);
}

// Wrappers needed for when we need to share callbacks between library and application
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
    bleGattcNotifyCharsCallback(conn_handle, chars_value);

    if (application_gatt_client_callbacks.notify_characteristics_cb != NULL) {
        application_gatt_client_callbacks.notify_characteristics_cb(conn_handle, chars_value);
    }
}

void bleGattcReadCharsCallbackWrapper(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value, status_t status
) {
    bleGattcReadCharsCallback(conn_handle, chars_value, status);

    if (application_gatt_client_callbacks.on_ble_gattc_read_characteristics_cb != NULL) {
        application_gatt_client_callbacks.on_ble_gattc_read_characteristics_cb(
            conn_handle, chars_value, status
        );
    }
}

void bleGattcWriteCharsCallbackWrapper(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t gatt_characteristics, status_t status
) {
    bleGattcWriteCharsCallback(conn_handle, gatt_characteristics, status);

    if (application_gatt_client_callbacks.on_ble_gattc_write_characteristics_cb != NULL) {
        application_gatt_client_callbacks.on_ble_gattc_write_characteristics_cb(
            conn_handle, gatt_characteristics, status
        );
    }
}
