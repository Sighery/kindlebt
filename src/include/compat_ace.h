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

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE
