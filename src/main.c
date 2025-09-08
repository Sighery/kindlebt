#include <stdio.h>
#include <unistd.h>

#include "kindlebt.h"

static sessionHandle bt_session = NULL;
static bleConnHandle conn_handle = NULL;
static bleGattsService_t gatt_db;

extern void bleGattcReadCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value, status_t status
);

static bleGattClientCallbacks_t gatt_app_callbacks = {
    .size = sizeof(bleGattClientCallbacks_t),
    .on_ble_gattc_read_characteristics_cb = bleGattcReadCharsCallback,
};

int main() {
    // The ace_bt stuff won't run under root user
    if (setgid((gid_t)1003) || setuid((uid_t)1003)) {
        fprintf(stderr, "Can't drop privileges to bluetooth user/group\n");
        return -1;
    }

    printf("Hello World from Kindle!\n");

    bool isBLE = isBLESupported();
    printf("Is BLE enabled: %d\n", isBLE);

    if (!isBLE) return -2;

    status_t status;

    status = openSession(ACEBT_SESSION_TYPE_DUAL_MODE, &bt_session);
    printf(
        "Opened session status %d, session %p (u32 %u)\n", status, bt_session, (uint32_t)bt_session
    );

    if (status != ACE_STATUS_OK) return -3;

    status = bleRegister(bt_session);
    printf("Registered BLE: %d\n", status);

    if (status != ACE_STATUS_OK) return -4;

    status = bleRegisterGattClient(bt_session, &gatt_app_callbacks);
    printf("Registered GATT Client status: %d\n", status);

    if (status != ACE_STATUS_OK) return -5;

    sleep(1);

    bdAddr_t ble_addr = {.address = {0x2C, 0xCF, 0x67, 0xB8, 0xDC, 0x3F}};
    status = bleConnect(
        bt_session, &conn_handle, &ble_addr, ACE_BT_BLE_CONN_PARAM_BALANCED,
        ACEBT_BLE_GATT_CLIENT_ROLE, ACE_BT_BLE_CONN_PRIO_MEDIUM
    );
    printf("Connected to BLE status:%d\n", status);

    if (status != ACE_STATUS_OK) return -6;

    status = bleDiscoverAllServices(bt_session, conn_handle);
    printf("Discovered all services: %d\n", status);
    if (status != ACE_STATUS_OK) return -7;

    status = bleGetDatabase(conn_handle, &gatt_db);
    printf("Requested GATT DB status: %d\n", status);
    if (status != ACE_STATUS_OK) return -8;

    sleep(2);

    uuid_t characUuid = {
        .uu =
            {0xFF, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00},
        .type = ACEBT_UUID_TYPE_16,
    };
    struct aceBT_gattCharRec_t* charac = utilsFindCharRec(characUuid, 16);

    if (charac == NULL) {
        printf("Couldn't find the characteristic?");
        return -9;
    }

    status = bleReadCharacteristic(bt_session, conn_handle, charac->value);
    printf("BLE read to characteristic ff120000000000000000000000000000, status: %d\n", status);

    sleep(2);

    status = bleDisconnect(conn_handle);
    printf("Disconnected from BLE status: %d\n", status);

    status = bleDeregisterGattClient(bt_session);
    printf("Deregistered GATT Client status: %d\n", status);

    status = bleDeregister(bt_session);
    printf("Deregistered BLE status: %d\n", status);

    status = closeSession(bt_session);
    printf("Closed session status: %d\n", status);

    return 0;
}
