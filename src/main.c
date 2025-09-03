#include <stdio.h>
#include <unistd.h>

#include "kindlebt.h"

static sessionHandle bt_session = NULL;
static bleConnHandle conn_handle = NULL;
static bleGattsService_t gatt_db;

int main() {
    // The ace_bt stuff won't run under root user
    if (setgid((gid_t)1003) || setuid((uid_t)1003)) {
        fprintf(stderr, "Can't drop privileges to bluetooth user/group\n");
        return -1;
    }

    printf("Hello World from Kindle!\n");

    bool isBLE = isBLESupported();
    printf("Is BLE enabled: %d\n", isBLE);

    status_t status;

    status = openSession(ACEBT_SESSION_TYPE_DUAL_MODE, &bt_session);
    printf(
        "Opened session status %d, session %p (u32 %u)\n", status, bt_session, (uint32_t)bt_session
    );

    status = bleRegister(bt_session);
    printf("Registered BLE: %d\n", status);

    status = bleRegisterGattClient(bt_session, NULL);
    printf("Registered GATT Client status: %d\n", status);

    sleep(1);

    bdAddr_t ble_addr = {.address = {0x2C, 0xCF, 0x67, 0xB8, 0xDC, 0x3F}};
    status = bleConnect(
        bt_session, &conn_handle, &ble_addr, ACE_BT_BLE_CONN_PARAM_BALANCED,
        ACEBT_BLE_GATT_CLIENT_ROLE, ACE_BT_BLE_CONN_PRIO_MEDIUM
    );
    printf("Connected to BLE status:%d\n", status);

    sleep(1);

    status = bleGetDatabase(conn_handle, &gatt_db);
    printf("Requested GATT DB status: %d\n", status);

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
