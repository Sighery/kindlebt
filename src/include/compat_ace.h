#ifndef COMPAT_ACE
#define COMPAT_ACE

#include "compat_ace_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PRE_5170 = 1,   // < 5.17.0
    SINCE_5170 = 2, // >= 5.17.0
} acebt_abi;

acebt_abi acebt_abi_version(void);

status_t pre5170_bleRegisterGattClient(
    sessionHandle session_handle, bleGattClientCallbacks_t* callbacks, bleAppId_t app_id
);
status_t pre5170_bleDeregisterGattClient(sessionHandle session_handle);
status_t pre5170_bleGetService(bleConnHandle conn_handle);
status_t pre5170_bleReadCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value
);

// Shim functions
// Shim for the getSessionForCallback/getSessionForTask runtime resolution
sessionHandle getSessionFromHandler(aceAipc_parameter_t* task);
status_t shim_bleRegisterGattClient(
    sessionHandle session_handle, bleGattClientCallbacks_t* callbacks, bleAppId_t app_id
);
status_t shim_bleDeregisterGattClient(sessionHandle session_handle);
status_t shim_bleGetDatabase(bleConnHandle conn_handle);
status_t shim_bleReadCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value
);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE
