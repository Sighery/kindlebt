#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "kindlebt_defines.h"

#define ADDR_WITH_COLON_LEN 17
#define ADDR_WITHOUT_COLON_LEN 12
#define PRINT_UUID_STR_LEN 49

static void remove_all_chars(char* str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

void utilsConvertBdAddrToStr(bdAddr_t* paddr, char* outStr) {
    sprintf(
        outStr, "%02X:%02X:%02X:%02X:%02X:%02X", paddr->address[0], paddr->address[1],
        paddr->address[2], paddr->address[3], paddr->address[4], paddr->address[5]
    );
}

uint8_t utilsConvertCharToHex(char input) {
    if (input >= '0' && input <= '9') {
        return (uint8_t)(input - '0');
    } else if (input >= 'A' && input <= 'F') {
        return (uint8_t)(input - 'A') + 10;
    } else if (input >= 'a' && input <= 'f') {
        return (uint8_t)(input - 'a') + 10;
    } else {
        return 0;
    }
}

uint16_t utilsConvertHexStrToByteArray(char* input, uint8_t* output) {
    uint8_t length = 0;
    char* hex_string = input;
    uint8_t hex_length = strlen(input);

    for (int i = 0; i < hex_length; i += 2) {
        uint8_t value = utilsConvertCharToHex(hex_string[i]) << 4;

        if (i + 1 < hex_length) {
            value |= utilsConvertCharToHex(hex_string[i + 1]);
        }
        output[length] = value;
        length++;
    }

    return length;
}

status_t utilsConvertStrToBdAddr(char* str, bdAddr_t* pAddr) {
    if (str == NULL || pAddr == NULL) {
        return ACE_STATUS_BAD_PARAM;
    }

    int length = strlen(str);
    if (length != ADDR_WITH_COLON_LEN && length != ADDR_WITHOUT_COLON_LEN) {
        printf("Invalid string format. Must be xx:xx:xx:xx:xx:xx or xxxxxxxxxxxx\n");
        return ACE_STATUS_BAD_PARAM;
    }
    // Check if string is in : format
    if (length == ADDR_WITH_COLON_LEN && str[2] == ':' && str[5] == ':' && str[8] == ':' &&
        str[11] == ':' && str[14] == ':') {
        remove_all_chars(str, ':');
    }

    if (strlen(str) != ADDR_WITHOUT_COLON_LEN) {
        printf("Invalid string format. Must be xx:xx:xx:xx:xx:xx or xxxxxxxxxxxx\n");
        return ACE_STATUS_BAD_PARAM;
    }

    for (int i = 0; i < ADDR_WITHOUT_COLON_LEN; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'A' && str[i] <= 'F') ||
              (str[i] >= 'a' && str[i] <= 'f'))) {
            printf("Contains non-hex character at index %d\n", i);
            return ACE_STATUS_BAD_PARAM;
        }
    }

    printf("str: %s\n", str);

    length = utilsConvertHexStrToByteArray(str, pAddr->address);
    if (length != MAC_ADDR_LEN) {
        return ACE_STATUS_BAD_PARAM;
    }

    return ACE_STATUS_OK;
}

void utilsPrintUuid(char* uuid_str, uuid_t* uuid, int max) {
    snprintf(
        uuid_str, max,
        "%02x %02x %02x %02x %02x %02x %02x %02x %02x"
        " %02x %02x %02x %02x %02x %02x %02x",
        uuid->uu[0], uuid->uu[1], uuid->uu[2], uuid->uu[3], uuid->uu[4], uuid->uu[5], uuid->uu[6],
        uuid->uu[7], uuid->uu[8], uuid->uu[9], uuid->uu[10], uuid->uu[11], uuid->uu[12],
        uuid->uu[13], uuid->uu[14], uuid->uu[15]
    );
}

void utilsDumpServer(bleGattsService_t* server) {
    if (!server) return;

    // struct list_head* svc_list;
    // struct list_head* char_list;
    int inc_svc_count = 0;
    char buff[PRINT_UUID_STR_LEN];
    memset(buff, 0, sizeof(char) * PRINT_UUID_STR_LEN);
    utilsPrintUuid(buff, &server->uuid, PRINT_UUID_STR_LEN);
    printf("Service 0 uuid %s serviceType %d\n", buff, server->serviceType);

    struct aceBT_gattIncSvcRec_t* svc_rec;
    STAILQ_FOREACH(svc_rec, &server->incSvcList, link) {
        memset(buff, 0, sizeof(char) * PRINT_UUID_STR_LEN);
        utilsPrintUuid(buff, &svc_rec->value.uuid, PRINT_UUID_STR_LEN);
        printf(
            "Included Services %d service Type %d uuid %s\n", inc_svc_count++,
            svc_rec->value.serviceType, buff
        );
    }
    uint8_t char_count = 0;
    struct aceBT_gattCharRec_t* char_rec = NULL;
    STAILQ_FOREACH(char_rec, &server->charsList, link) {
        memset(buff, 0, sizeof(char) * PRINT_UUID_STR_LEN);
        utilsPrintUuid(buff, &char_rec->value.gattRecord.uuid, PRINT_UUID_STR_LEN);
        if (char_rec->value.gattDescriptor.is_notify && char_rec->value.gattDescriptor.is_set) {
            printf("\tGatt Characteristics with Notifications %d uuid %s\n", char_count++, buff);
        } else {
            printf("\tGatt Characteristics %d uuid %s\n", char_count++, buff);
        }

        if (char_rec->value.gattDescriptor.is_set) {
            utilsPrintUuid(
                buff, &char_rec->value.gattDescriptor.gattRecord.uuid, PRINT_UUID_STR_LEN
            );
            printf("\t\tDescriptor UUID %s\n", buff);

        } else if (char_rec->value.multiDescCount) {
            uint8_t desc_num = 1;
            struct aceBT_gattDescRec_t* desc_rec = NULL;
            /* Traverse descriptor linked list */
            STAILQ_FOREACH(desc_rec, &char_rec->value.descList, link) {
                utilsPrintUuid(buff, &desc_rec->value.gattRecord.uuid, PRINT_UUID_STR_LEN);
                printf("\t\tDescriptor %d UUID %s\n", desc_num++, buff);
            }
        }
    }
}

struct aceBT_gattCharRec_t* utilsFindCharRec(uuid_t uuid, uint8_t uuid_len) {
    struct aceBT_gattCharRec_t* char_rec = NULL;

    if (!pGgatt_service) {
        printf("GATT DB has not been populated yet!\n");
        return (NULL);
    }

    // Iterate through all services
    for (uint32_t i = 0; i < gNo_svc; i++) {
        bleGattsService_t* services = &pGgatt_service[i];

        // Iterate through all characteristics and look for char uuid
        STAILQ_FOREACH(char_rec, &services->charsList, link) {
            // If char uuid matches, read characteristic
            if (!memcmp(char_rec->value.gattRecord.uuid.uu, &uuid.uu, uuid_len)) {
                return (char_rec);
            }
        }
    }
    printf("GATT Characteristic UUID could not be found!\n");
    return (NULL);
}

void setGattBlobFromBytes(
    bleGattCharacteristicsValue_t* chars_value, const uint8_t* data, uint16_t size
) {
    if (chars_value == NULL || data == NULL || size == 0) return;

    free(chars_value->blobValue.data);

    printf("createGattBlobFromBytes received length: %u\n", size);
    printf("input bytes: ");
    for (int i = 0; i < size; ++i) {
        printf("0x%02X ", data[i]);
    }
    printf("\n");

    uint8_t* blob = malloc(size);
    if (blob == NULL) return;

    printf("Did malloc\n");

    memcpy(blob, data, size);

    chars_value->blobValue.data = blob;
    chars_value->blobValue.size = size;
    chars_value->blobValue.offset = 0;
    chars_value->format = BLE_FORMAT_BLOB;
}

void freeGattBlob(bleGattCharacteristicsValue_t* chars_value) {
    if (chars_value == NULL || chars_value->blobValue.data == NULL) return;

    free(chars_value->blobValue.data);
    chars_value->blobValue.data = NULL;
    chars_value->blobValue.size = 0;
    chars_value->blobValue.offset = 0;
}

status_t waitForCondition(pthread_mutex_t* lock, pthread_cond_t* cond, bool* flag) {
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 10;

    pthread_mutex_lock(lock);
    while (!(*flag)) {
        int res = pthread_cond_timedwait(cond, lock, &ts);
        if (res == ETIMEDOUT) {
            pthread_mutex_unlock(lock);
            printf("Timeout occurred\n");
            return ACE_STATUS_TIMEOUT;
        }
    }
    pthread_mutex_unlock(lock);

    return ACE_STATUS_OK;
}

void setCallbackVariable(pthread_mutex_t* lock, pthread_cond_t* cond, bool* flag, bool value) {
    pthread_mutex_lock(lock);
    *flag = value;
    pthread_cond_signal(cond);
    pthread_mutex_unlock(lock);
}
