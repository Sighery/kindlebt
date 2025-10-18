#include <stdio.h>
#include <unistd.h>

#include <kindlebt/kindlebt.h>
#include <kindlebt/kindlebt_log.h>

static sessionHandle bt_session = NULL;
static bleConnHandle conn_handle = NULL;
static bleGattsService_t gatt_db;

// For our dumb callback waits implementation
static bool read_characteristic = false;
static bool wrote_characteristic = false;

// ===== UTILS =====
void print_uuid(uuid_t* uuid) {
    printf(
        "UUID: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
        uuid->uu[0], uuid->uu[1], uuid->uu[2], uuid->uu[3], uuid->uu[4], uuid->uu[5], uuid->uu[5],
        uuid->uu[7], uuid->uu[8], uuid->uu[9], uuid->uu[10], uuid->uu[11], uuid->uu[12],
        uuid->uu[13], uuid->uu[14], uuid->uu[15]
    );
}

// This is a very simple implementation. In some cases, the response might not even be
// a @ref bleGattBlobValue_t like we're assuming here
// Another thing to note is that the \c data field is just a \c uint8_t* field. If you
// expect text, you'll need to cast it with an ASCII table
void print_data(bleGattBlobValue_t* value) {
    printf("Data: ");
    for (int idx = 0; idx < value->size; idx++)
        printf("%02x ", value->data[idx]);
    printf("\n");
}

void freeGattBlob(bleGattCharacteristicsValue_t* chars_value) {
    if (chars_value == NULL || chars_value->blobValue.data == NULL) return;

    free(chars_value->blobValue.data);
    chars_value->blobValue.data = NULL;
    chars_value->blobValue.size = 0;
    chars_value->blobValue.offset = 0;
}

void setGattBlobFromBytes(
    bleGattCharacteristicsValue_t* chars_value, const uint8_t* data, uint16_t size
) {
    if (chars_value == NULL || data == NULL || size == 0) return;

    free(chars_value->blobValue.data);

    uint8_t* blob = malloc(size);
    if (blob == NULL) return;

    memcpy(blob, data, size);

    chars_value->blobValue.data = blob;
    chars_value->blobValue.size = size;
    chars_value->blobValue.offset = 0;
    chars_value->format = BLE_FORMAT_BLOB;
}

// ===== CALLBACKS =====
void bleGattcNotifyCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value
) {
    printf("Callback %s(): conn_handle %p", __func__, conn_handle);
    print_uuid(&chars_value.gattRecord.uuid);
    print_data(&chars_value.blobValue);
}

void bleGattcReadCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t chars_value, status_t status
) {
    printf("Callback %s(): status %d conn_handle %p\n", __func__, status, conn_handle);
    print_uuid(&chars_value.gattRecord.uuid);
    print_data(&chars_value.blobValue);
    read_characteristic = true;
}

void bleGattcWriteCharsCallback(
    bleConnHandle conn_handle, bleGattCharacteristicsValue_t gatt_characteristics, status_t status
) {
    printf("Callback %s(): conn_handle %p status %d", __func__, conn_handle, status);
    wrote_characteristic = true;
}

static bleGattClientCallbacks_t gatt_app_callbacks = {
    .size = sizeof(bleGattClientCallbacks_t),
    .notify_characteristics_cb = bleGattcNotifyCharsCallback,
    .on_ble_gattc_read_characteristics_cb = bleGattcReadCharsCallback,
    .on_ble_gattc_write_characteristics_cb = bleGattcWriteCharsCallback,
};

int main() {
    // (OPTIONAL) SET LOG LEVEL
    kindlebt_set_log_level(LOG_LEVEL_DEBUG);

    // USE BLUETOOTH PRIVILEGES
    // The ace_bt stuff won't run under root user
    if (setgid((gid_t)1003) || setuid((uid_t)1003)) {
        fprintf(stderr, "Can't drop privileges to bluetooth user/group\n");
        return -1;
    }

    printf("Hello World from Kindle!\n");

    // CHECK FOR BLE SUPPORT
    // Might not be necessary
    bool isBLE = isBLESupported();
    printf("Is BLE enabled: %d\n", isBLE);
    if (!isBLE) {
        fprintf(stderr, "BLE is not enabled\n");
        return -2;
    }

    status_t status;

    // OPEN SESSION
    // Necessary, basically all API operations require a BT session
    status = openSession(ACEBT_SESSION_TYPE_DUAL_MODE, &bt_session);
    printf(
        "Opened session status %d, session %p (u32 %u)\n", status, bt_session, (uint32_t)bt_session
    );
    if (status != ACE_STATUS_OK) {
        fprintf(stderr, "Cannot open Bluetooth session, status: %d\n", status);
        return -3;
    }

    // REGISTER FOR BLE USE
    // This registers for BLE and related callbacks functionality in btmanagerd
    status = bleRegister(bt_session);
    printf("Registered BLE: %d\n", status);
    if (status != ACE_STATUS_OK) {
        fprintf(stderr, "Cannot register BLE, status: %d\n", status);
        return -4;
    }

    // REGISTER FOR GATT CLIENT USE
    // Likewise, same as before but specifically for GATTC events in btmanagerd
    status = bleRegisterGattClient(bt_session, &gatt_app_callbacks);
    printf("Registered GATT Client status: %d\n", status);
    if (status != ACE_STATUS_OK) {
        fprintf(stderr, "Cannot register GATT Client, status: %d\n", status);
        return -5;
    }

    // CONNECT TO BLE DEVICE
    // Untested whether connecting to a Classic device with this API works
    bdAddr_t ble_addr = {.address = {0x2C, 0xCF, 0x67, 0xB8, 0xDC, 0x3F}};
    status = bleConnect(
        bt_session, &conn_handle, &ble_addr, ACE_BT_BLE_CONN_PARAM_BALANCED,
        ACEBT_BLE_GATT_CLIENT_ROLE, ACE_BT_BLE_CONN_PRIO_MEDIUM
    );
    printf("Connected to BLE status:%d\n", status);
    if (status != ACE_STATUS_OK) {
        fprintf(stderr, "Cannot connect to BLE device, status: %d\n", status);
        return -6;
    }

    // DISCOVER GATT SERVICES
    // Necessary step to discover all the GATT services of the remote device
    status = bleDiscoverAllServices(bt_session, conn_handle);
    printf("Discovered all services: %d\n", status);
    if (status != ACE_STATUS_OK) {
        fprintf(stderr, "Cannot discover services of BLE device, status: %d\n", status);
        return -7;
    }

    // RETRIEVE GATT SERVICES (DB)
    // Retrieval of services is a different and also necessary step from discovery
    // Even if you know the Characteristic UUIDs ahead of time, you still need to
    // discover and retrieve the services
    status = bleGetDatabase(conn_handle, &gatt_db);
    printf("Requested GATT DB status: %d\n", status);
    if (status != ACE_STATUS_OK) {
        fprintf(stderr, "Cannot retrieve services of BLE device, status: %d\n", status);
        return -8;
    }

    // FINDING CHARACTERISTIC
    // Again, even if you know beforehand the UUID of the characteristic you want, you
    // still need to retrieve the DB, and then find your UUID in the local DB copy to
    // interact with it
    uuid_t characUuid = {
        .uu =
            {0xFF, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00},
        .type = ACEBT_UUID_TYPE_16,
    };
    struct aceBT_gattCharRec_t* charac = utilsFindCharRec(characUuid, 16);
    if (charac == NULL) {
        printf("Couldn't find the characteristic");
        return -9;
    }

    // ENABLING NOTIFICATIONS ON CHARACTERISTIC
    status = bleSetNotification(bt_session, conn_handle, charac->value, true);
    if (status != ACE_STATUS_OK) {
        fprintf(
            stderr, "Cannot enable notifications on BLE device/characteristic, status: %d\n", status
        );
        return -10;
    }

    sleep(5);

    // DISABLING NOTIFICATIONS ON CHARACTERISTIC
    status = bleSetNotification(bt_session, conn_handle, charac->value, false);
    if (status != ACE_STATUS_OK) {
        fprintf(
            stderr, "Cannot disable notifications on BLE device/characteristic, status: %d\n",
            status
        );
        return -10;
    }

    sleep(2);

    // Block reading and writing in a loop
    for (int i = 0; i < 20; i++) {
        // READING CHARACTERISTIC
        status = bleReadCharacteristic(bt_session, conn_handle, charac->value);
        if (status != ACE_STATUS_OK) {
            fprintf(stderr, "Cannot read characteristic, status: %d\n", status);
            return -11;
        }
        for (int j = 0; j < 10; j++) {
            if (read_characteristic) break;
            sleep(1);
        }
        if (!read_characteristic) {
            fprintf(stderr, "Timed out waiting to read characteristic, status: %d\n", status);
            return -11;
        }
        read_characteristic = false;
        // Not sure if necessary but I always reset the blob after read/write
        freeGattBlob(&charac->value);

        // WRITING CHARACTERISTIC
        // Values used in my characteristic as an example of how to write
        uint8_t off[] = {'O', 'F', 'F'};
        uint8_t on[] = {'O', 'N'};
        uint8_t* states[] = {off, on};
        size_t lengths[] = {sizeof(off), sizeof(on)};

        // Now write the value to the characteristic struct
        setGattBlobFromBytes(&charac->value, states[i % 2], lengths[i % 2]);

        status = bleWriteCharacteristic(
            bt_session, conn_handle, &charac->value, ACEBT_BLE_WRITE_TYPE_RESP_REQUIRED
        );
        if (status != ACE_STATUS_OK) {
            fprintf(stderr, "Cannot write characteristic, status: %d\n", status);
            return -12;
        }
        for (int j = 0; j < 10; j++) {
            if (wrote_characteristic) break;
            sleep(1);
        }
        if (!wrote_characteristic) {
            fprintf(stderr, "Timed out waiting to write characteristic, status: %d\n", status);
            return -12;
        }
        wrote_characteristic = false;
        freeGattBlob(&charac->value);
    }

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
