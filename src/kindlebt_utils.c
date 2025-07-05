#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
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
