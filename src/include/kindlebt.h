#ifndef KINDLE_BT_H
#define KINDLE_BT_H

#include <stdbool.h>

#include "kindlebt_defines.h"
#include "kindlebt_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

bool isBLESupported(void);

status_t enableRadio(sessionHandle session_handle);
status_t disableRadio(sessionHandle session_handle);
status_t getRadioState(state_t* p_out_state);

sessionType_t getSupportedSession(void);
status_t openSession(sessionType_t session_type, sessionHandle* session_handle);
status_t closeSession(sessionHandle session_handle);

status_t bleRegister(sessionHandle session_handle);
status_t bleDeregister(sessionHandle session_handle);

status_t bleRegisterGattClient(sessionHandle session_handle, bleGattClientCallbacks_t* callbacks);
status_t bleDeregisterGattClient(sessionHandle session_handle);
status_t bleGetDatabase(bleConnHandle conn_handle, bleGattsService_t* p_gatt_service);
status_t bleCloneGattService(
    bleGattsService_t** dst_gatt_service, const bleGattsService_t* src_gatt_service, int no_svc
);

status_t bleConnect(
    sessionHandle session_handle, bleConnHandle* conn_handle, bdAddr_t* p_device,
    bleConnParam_t conn_param, bleConnRole_t conn_role, bleConnPriority_t conn_priority
);
status_t bleDisconnect(bleConnHandle conn_handle);

status_t bleSetNotification(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value, bool enable
);
status_t bleReadCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value
);
status_t bleWriteCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
);

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_H
