#ifndef KINDLE_BT_UTILS_H
#define KINDLE_BT_UTILS_H

#include <string.h>

#include "kindlebt_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

status_t utilsConvertStrToBdAddr(char* str, bdAddr_t* pAddr);
uint16_t utilsConvertHexStrToByteArray(char* input, uint8_t* output);
struct aceBT_gattCharRec_t* utilsFindCharRec(uuid_t uuid, uint8_t uuid_len);
void setGattBlobFromBytes(
    bleGattCharacteristicsValue_t* chars_value, const uint8_t* data, uint16_t size
);
void freeGattBlob(bleGattCharacteristicsValue_t* chars_value);

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_UTILS_H
