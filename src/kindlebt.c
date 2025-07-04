#include "ace/bluetooth_common_api.h"
#include "ace/bluetooth_session_api.h"

#include "kindlebt_defines.h"

bool isBLESupported(void) {
    return aceBT_isBLESupported();
};

sessionType_t getSupportedSession(void) {
    return aceBT_getSupportedSession();
}

status_t openSession(sessionType_t session_type, sessionHandle* session_handle) {
    return aceBT_openSession(session_type, NULL, session_handle);
}

status_t closeSession(sessionHandle session_handle) {
    return aceBT_closeSession(session_handle);
}
