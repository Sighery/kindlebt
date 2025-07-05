#ifndef KINDLE_BT_DEFINES_H
#define KINDLE_BT_DEFINES_H

#include <pthread.h>
#include <stdbool.h>

#include "ace/bluetooth_ble_api.h"
#include "ace/bluetooth_ble_defines.h"
#include "ace/bluetooth_ble_gatt_client_api.h"
#include "ace/bluetooth_defines.h"
#include "ace/bluetooth_session_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef aceBT_state_t state_t;
typedef ace_status_t status_t;
typedef aceBT_bdAddr_t bdAddr_t;

typedef aceBT_sessionHandle sessionHandle;
typedef aceBT_sessionType_t sessionType_t;

typedef aceBT_bleConnHandle bleConnHandle;

typedef aceBT_commonCallbacks_t commonCallbacks_t;
typedef aceBT_bleCallbacks_t bleCallbacks_t;

typedef struct {
    bool bt_enabled;
    bool ble_registered;
} bleCallbackVars_t;

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_DEFINES_H
