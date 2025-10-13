#ifndef KINDLE_BT_UTILS_H
#define KINDLE_BT_UTILS_H

#include <string.h>

#include "kindlebt_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

status_t utilsConvertStrToBdAddr(char* str, bdAddr_t* pAddr);
uint16_t utilsConvertHexStrToByteArray(char* input, uint8_t* output);

/**
 * @addtogroup KINDLEBT_PUBLIC_API
 * @{
 */

/**
 * @brief Find a GATT Characteristic Record by UUID
 *
 * Even if you know the UUID of a Characteristic, you need to find it in
 * @ref pGgatt_service. This is because the library uses these local structs to provide
 * and interface for read/writes.
 */
struct aceBT_gattCharRec_t* utilsFindCharRec(uuid_t uuid, uint8_t uuid_len);

/** @} */ // KINDLEBT_PUBLIC_API

void setGattBlobFromBytes(
    bleGattCharacteristicsValue_t* chars_value, const uint8_t* data, uint16_t size
);
void freeGattBlob(bleGattCharacteristicsValue_t* chars_value);

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_UTILS_H
