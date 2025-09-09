#ifndef COMPAT_ACE_IMPLEMENTATIONS_H
#define COMPAT_ACE_IMPLEMENTATIONS_H

#include <stdbool.h>

#include "kindlebt_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

status_t pre5170_bleRegisterGattClient(
    sessionHandle session_handle, bleGattClientCallbacks_t* callbacks, bleAppId_t app_id
);
status_t pre5170_bleDeregisterGattClient(sessionHandle session_handle);
status_t pre5170_bleDiscoverAllServices(sessionHandle session_handle, bleConnHandle conn_handle);
status_t pre5170_bleGetService(bleConnHandle conn_handle);
status_t pre5170_bleReadCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value
);
status_t pre5170_bleWriteCharacteristics(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE_IMPLEMENTATIONS_H
