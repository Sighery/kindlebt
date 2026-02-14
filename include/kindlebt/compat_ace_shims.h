#ifndef COMPAT_ACE_SHIMS_H
#define COMPAT_ACE_SHIMS_H

#include "compat_ace_internals.h"
#include "kindlebt_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

// Shim for the getSessionForCallback/getSessionForTask runtime resolution
sessionHandle getSessionFromHandler(aceAipc_parameter_t* task);

status_t shim_bleRegisterGattClient(
    sessionHandle session_handle, bleGattClientCallbacks_t* callbacks, bleAppId_t app_id
);
status_t shim_bleDeregisterGattClient(sessionHandle session_handle);
status_t shim_bleDiscoverAllServices(sessionHandle session_handle, bleConnHandle conn_handle);
status_t shim_bleGetDatabase(bleConnHandle conn_handle);
status_t shim_bleCleanupGattService(bleGattsService_t* service, int no_svc);
status_t shim_bleReadCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value
);
status_t shim_bleWriteCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
);
status_t shim_bleWriteDescriptor(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
);
status_t shim_bleSetNotification(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value, bool enable
);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE_SHIMS_H
