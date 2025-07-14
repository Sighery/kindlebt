#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "kindlebt.h"
#include "kindlebt_defines.h"

#include "kindlebt_utils.c"

pthread_mutex_t callback_vars_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t callback_vars_cond = PTHREAD_COND_INITIALIZER;
bleCallbackVars_t callback_vars;
bleConnHandle conn_handle;
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

void bleMtuUpdatedCallback(status_t status, bleConnHandle connHandle, int mtu) {
    printf("Callback: bleMtuUpdatedCallback() status: %d ", status);
    printf("mtu %d, connHandle %p\n", mtu, connHandle);

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
    bleConnState_t state, gattStatus_t status, const bleConnHandle connHandle, bdAddr_t* p_addr
) {
    printf("Callback: bleConnStateChangedCallback() ");
    printf(
        "state %d status %d connHandle %p addr %02x\n", state, status, connHandle,
        p_addr->address[5]
    );

    conn_handle = connHandle;

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
