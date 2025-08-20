#ifndef COMPAT_ACE
#define COMPAT_ACE

#include "kindlebt_defines.h"

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

// ACE things
typedef struct {
    uint16_t callback_server_id;
    uint16_t server_id;
} aceBT_aipcHandles_t;
typedef aceBT_aipcHandles_t aipcHandles_t;

status_t getSessionInfo(sessionHandle sessionHandle, aipcHandles_t* handles);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE
