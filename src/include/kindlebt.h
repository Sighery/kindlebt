#ifndef KINDLE_BT_H
#define KINDLE_BT_H

#include <stdbool.h>

#include "kindlebt_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

bool isBLESupported(void);

sessionType_t getSupportedSession(void);
status_t openSession(sessionType_t session_type, sessionHandle* session_handle);
status_t closeSession(sessionHandle session_handle);

status_t bleRegister(sessionHandle session_handle);
status_t bleDeregister(sessionHandle session_handle);

status_t bleRegisterGattClient(sessionHandle session_handle);
status_t bleDeregisterGattClient(sessionHandle session_handle);

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_H
