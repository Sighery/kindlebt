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

#define MAC_ADDR_LEN ACEBT_MAC_ADDR_LEN
#define MAC_ADDR_STR_LEN ACEBT_MAC_ADDR_STR_LEN

typedef aceBT_state_t state_t;
typedef ace_status_t status_t;
typedef aceBT_bdAddr_t bdAddr_t;
typedef aceBT_bondState_t bondState_t;
typedef aceBT_uuid_t uuid_t;

typedef aceBT_sessionHandle sessionHandle;
typedef aceBT_sessionType_t sessionType_t;

typedef aceBT_bleConnHandle bleConnHandle;
typedef aceBT_bleConnState_t bleConnState_t;
typedef aceBt_bleConnParam_t bleConnParam_t;
typedef aceBT_bleConnRole_t bleConnRole_t;
typedef aceBt_bleConnPriority_t bleConnPriority_t;

typedef aceBT_gattStatus_t gattStatus_t;
typedef aceBt_bleAppId_t bleAppId_t;

typedef aceBT_commonCallbacks_t commonCallbacks_t;
typedef aceBT_bleCallbacks_t bleCallbacks_t;
typedef aceBT_bleGattClientCallbacks_t bleGattClientCallbacks_t;

typedef struct {
    bool bt_enabled;
    bool ble_registered;
    bool mtu_set;
    bool gattc_connected;
    bool gattc_disconnected;
} bleCallbackVars_t;

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_DEFINES_H
