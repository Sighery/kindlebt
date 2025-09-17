#ifndef COMPAT_ACE_UTILS_H
#define COMPAT_ACE_UTILS_H

#include <stddef.h>

#include "compat_ace_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

void dump_hex(const void* ptr, size_t size);
void dump_registerCbackGattcData(const registerCbackGattcData_t* data);
void dump_gattc_get_db_data_t(const gattc_get_db_data_t* data);
void dump_aceAipc_parameter_t(aceAipc_parameter_t* task);
void dump_mask_bits(uint16_t mask);
void dump_aipc_handle(aipcHandles_t aipc_handle);
void dump_uuid2(const uuid_t* uuid);
void dump_bleGattRecord_t(const bleGattRecord_t* record);
void dump_bleGattDescriptor_t(const bleGattDescriptor_t* descriptor);
void dump_bleGattCharacteristicsValue_t(const bleGattCharacteristicsValue_t chars_value);
void dump_notify_data_t(const notify_data_t* data);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE_UTILS_H
