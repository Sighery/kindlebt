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
 * @ref bleGattsService_t. This is because the library uses these local structs to provide
 * and interface for read/writes.
 */
struct aceBT_gattCharRec_t*
utilsFindCharRec(bleGattsService_t* services, uint32_t noSvcs, uuid_t uuid, uint8_t uuid_len);

/** @} */ // KINDLEBT_PUBLIC_API

void setGattBlobFromBytes(
    bleGattCharacteristicsValue_t* chars_value, const uint8_t* data, uint16_t size
);
void freeGattBlob(bleGattCharacteristicsValue_t* chars_value);
char* utilsDumpServer(bleGattsService_t* server, char* log_buff, size_t* size, size_t* offset);

device_context_t* dca_add_new(device_context_array_t* arr);
device_context_t* dca_find_by_handle(device_context_array_t* arr, const bleConnHandle conn_handle);
device_context_t* dca_find_by_addr(device_context_array_t* arr, const bdAddr_t* addr);
bool dca_remove(device_context_array_t* arr, const bleConnHandle conn_handle);
void dca_free(device_context_array_t* arr);

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_UTILS_H
