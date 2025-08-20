#ifndef KINDLE_BT_DEFINES_H
#define KINDLE_BT_DEFINES_H

#include <pthread.h>
#include <stdbool.h>

#include <ace/bluetooth_ble_api.h>
#include <ace/bluetooth_ble_defines.h>
#include <ace/bluetooth_ble_gatt_client_api.h>
#include <ace/bluetooth_defines.h>
#include <ace/bluetooth_session_api.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAC_ADDR_LEN ACEBT_MAC_ADDR_LEN
#define MAC_ADDR_STR_LEN ACEBT_MAC_ADDR_STR_LEN

#define BLE_FORMAT_UINT8 ACEBT_BLE_FORMAT_UINT8
#define BLE_FORMAT_UINT16 ACEBT_BLE_FORMAT_UINT16
#define BLE_FORMAT_UINT32 ACEBT_BLE_FORMAT_UINT32
#define BLE_FORMAT_SINT8 ACEBT_BLE_FORMAT_SINT8
#define BLE_FORMAT_SINT16 ACEBT_BLE_FORMAT_SINT16
#define BLE_FORMAT_SINT32 ACEBT_BLE_FORMAT_SINT32
#define BLE_FORMAT_SFLOAT ACEBT_BLE_FORMAT_SFLOAT
#define BLE_FORMAT_FLOAT ACEBT_BLE_FORMAT_FLOAT
#define BLE_FORMAT_BLOB ACEBT_BLE_FORMAT_BLOB

typedef aceBT_state_t state_t;
typedef ace_status_t status_t;
typedef aceBT_bdAddr_t bdAddr_t;
typedef aceBT_bondState_t bondState_t;
typedef aceBT_UUIDType_t UUIDType_t;
typedef aceBT_uuid_t uuid_t;
typedef aceBT_responseType_t responseType_t;

typedef aceBT_sessionHandle sessionHandle;
typedef aceBT_sessionType_t sessionType_t;

typedef aceBT_bleConnHandle bleConnHandle;
typedef aceBT_bleConnState_t bleConnState_t;
typedef aceBt_bleConnParam_t bleConnParam_t;
typedef aceBT_bleConnRole_t bleConnRole_t;
typedef aceBt_bleConnPriority_t bleConnPriority_t;

typedef aceBT_gattStatus_t gattStatus_t;
typedef aceBt_bleAppId_t bleAppId_t;
typedef aceBT_bleGattsService_t bleGattsService_t;
typedef aceBT_bleGattAttributeFormat bleGattAttributeFormat;
typedef aceBT_bleGattBlobValue_t bleGattBlobValue_t;
typedef aceBT_bleGattCharacteristicsValue_t bleGattCharacteristicsValue_t;

typedef aceBT_commonCallbacks_t commonCallbacks_t;
typedef aceBT_bleCallbacks_t bleCallbacks_t;
typedef aceBT_bleGattClientCallbacks_t bleGattClientCallbacks_t;

typedef struct {
    bool bt_enabled;
    bool ble_registered;
    bool mtu_set;
    bool gattc_connected;
    bool gattc_disconnected;
    bool got_gatt_db;
} bleCallbackVars_t;

extern bleCallbackVars_t callback_vars;
extern uint32_t gNo_svc;
extern bleGattsService_t* pGgatt_service;
extern bleConnHandle ble_conn_handle;

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_DEFINES_H
