#ifndef COMPAT_ACE_INTERNALS
#define COMPAT_ACE_INTERNALS

#include "kindlebt_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t callback_server_id;
    uint16_t server_id;
} aceBT_aipcHandles_t;
typedef aceBT_aipcHandles_t aipcHandles_t;

typedef struct {
    uint32_t size;
    uint32_t session_handle;
    status_t out_status;
} __attribute__((packed)) acebt_request_unreg_t;
typedef acebt_request_unreg_t request_unreg_t;

// Reversed. Don't ask me why the repeated aipcHandles_t
// Used for GATT Client/Server deregister requests
typedef struct {
    uint32_t size;
    aipcHandles_t h1;
    aipcHandles_t h2;
} gatt_request_unreg_t;

status_t getSessionInfo(sessionHandle sessionHandle, aipcHandles_t* handles);

typedef struct {
    uint16_t size;
    uint32_t session_handle;
    uint16_t callback_mask;
    uint8_t is_unregister;
    bleAppId_t app_id;
    status_t out_status;
} aceBtMgr_registerCbackGattcData_t;
typedef aceBtMgr_registerCbackGattcData_t registerCbackGattcData_t;

void aceBt_serializeGattcRegisterData(
    registerCbackGattcData_t* data, uint32_t session_handle, uint16_t mask, bleAppId_t app_id
);

// Used for dataIndex
#define CALLBACK_INDEX_CLASSIC_DEV_MGR 0x00
#define CALLBACK_INDEX_COMMON_SECURITY 0x01
#define CALLBACK_INDEX_BLE_BEACON_MGR 0x02
#define CALLBACK_INDEX_BLE_GATT_SERVER 0x03
#define CALLBACK_INDEX_BLE_ADAPTER 0x04
#define CALLBACK_INDEX_SOCKET_SERVER 0x05
#define CALLBACK_INDEX_SOCKET_CLIENT 0x06
#define CALLBACK_INDEX_BLE_GATT_CLIENT 0x07
#define CALLBACK_INDEX_HFP 0x08

/**
 * Registers client info for a specific session handle with the session
 * manager.
 * Framework clients can store and retrieve this info for use-cases like
 * storing callback
 * pointers to notify their respective app clients.
 *
 * @param sessionHandle session handle for current Bluetooth session
 * @param p_client_data pointer to data
 * @param : dataIndex , index of this data within senssion
 * @return ACEBT_STATUS_SUCCESS if registered successfully; else,
 * aceBT_status_t error code
 */
status_t registerBTClientData(sessionHandle sessionHandle, uint8_t dataIndex, void* p_client_data);

typedef enum {
    /** Common APIs for Classic and BLE */
    ACE_BT_API_SESSION_OPEN = 0,
    ACE_BT_API_SESSION_CLOSE,
    ACE_BT_API_REGISTER,
    ACE_BT_API_UNREGISTER,
    ACE_BT_API_ENABLE,
    ACE_BT_API_DISABLE,
    ACE_BT_API_ENABLE_HCI,
    ACE_BT_API_DISABLE_HCI,
    ACE_BT_API_GET_ADAPTER_NAME,
    ACE_BT_API_SET_ADAPTER_NAME,
    ACE_BT_API_GET_ADAPTER_PROPERTIES,
    ACE_BT_API_GET_RADIO_STATE,
    ACE_BT_API_PAIR_DEVICE,
    ACE_BT_API_UNPAIR_DEVICE,
    ACE_BT_API_CANCEL_PAIR,
    ACE_BT_API_SET_PIN_REPLY,
    ACE_BT_API_SET_SSP_REPLY,
    ACE_BT_API_GET_NAME,
    ACE_BT_API_GET_DEVICE_TYPE,
    ACE_BT_API_GET_BONDED_DEVICES_COUNT,
    ACE_BT_API_GET_BONDED_DEVICES,
    ACE_BT_API_GET_CONNECTED_DEVICES_COUNT,
    ACE_BT_API_GET_CONNECTED_DEVICES,
    ACE_BT_API_GET_BOND_STATE,
    ACE_BT_API_GET_CONNECTION_STATE,
    ACE_BT_API_READ_RSSI,
    ACE_BT_API_COMMON_MAX = 100,
    /** APIs for Classic device management */
    ACE_BT_API_START_SCAN = 101,
    ACE_BT_API_STOP_SCAN = 102,
    ACE_BT_API_GET_SCAN_MODE = 103,
    ACE_BT_API_SET_SCAN_MODE = 104,
    ACE_BT_API_GET_SCAN_TYPE = 105,
    ACE_BT_API_SET_SCAN_TYPE = 106,
    ACE_BT_API_GET_DEVICE_UUIDS = 107,
    ACE_BT_API_GET_DEVICE_UUIDS_COUNT = 108,
    ACE_BT_API_GET_EIR = 109,
    ACE_BT_API_SET_RESTRICTED_PROFILES = 110,
    ACE_BT_API_GET_REMOTE_CLASSIC_PROPERTY = 111,
    ACE_BT_API_CLASSIC_DM_MAX = 200,
    /** APIs for classic profiles */
    ACE_BT_A2DP_SRC_API_INIT,
    ACE_BT_A2DP_SRC_API_CONNECT,
    ACE_BT_A2DP_SRC_API_DISCONNECT,
    ACE_BT_A2DP_SRC_API_GET_CONN_STATE,
    ACE_BT_A2DP_SRC_API_GET_AUDIO_STATE,
    ACE_BT_A2DP_SRC_MAX = 300,
    ACE_BT_A2DP_SNK_API_INIT,
    ACE_BT_A2DP_SNK_API_CONNECT,
    ACE_BT_A2DP_SNK_API_DISCONNECT,
    ACE_BT_A2DP_SNK_API_GET_CONN_STATE,
    ACE_BT_A2DP_SNK_API_GET_AUDIO_STATE,
    ACE_BT_AVRC_CTRL_INIT,
    ACE_BT_AVRC_CTRL_REGISTER,
    ACE_BT_AVRC_CTRL_UNREGISTER,
    ACE_BT_AVRC_CTRL_SEND_KEY,
    ACE_BT_A2DP_SNK_MAX = 400,
    ACE_BT_HID_HOST_API_INIT,
    ACE_BT_HID_HOST_API_CONNECT,
    ACE_BT_HID_HOST_API_DISCONNECT,
    ACE_BT_HID_HOST_API_GET_PROFILE_STATE,
    ACE_BT_HID_HOST_API_VIRTUAL_UNPLUG,
    ACE_BT_HID_HOST_API_SEND_DATA,
    ACE_BT_HID_HOST_API_GET_BATTERY_LEVEL,
    ACE_BT_HID_HOST_API_GET_PROTOCOL,
    ACE_BT_HID_HOST_API_SET_PROTOCOL,
    ACE_BT_HID_HOST_API_GET_REPORT,
    ACE_BT_HID_HOST_API_SET_REPORT,
    ACE_BT_HID_HOST_MAX = 500,
    ACE_BT_HFP_API_INIT,
    ACE_BT_HFP_API_CONNECT,
    ACE_BT_HFP_API_DISCONNECT,
    ACE_BT_HFP_API_REGISTER,
    ACE_BT_HFP_API_GET_PROFILE_STATE,
    ACE_BT_HFP_API_GET_AUDIO_STATE,
    ACE_BT_HFP_API_CONNECT_AUDIO,
    ACE_BT_HFP_API_DISCONNECT_AUDIO,
    ACE_BT_HFP_API_CHANGE_CALL_STATE,
    ACE_BT_HFP_MAX = 600,
    /** APIs for BLE */
    ACE_BT_BLE_REGISTER_BEACON_CLI_API,
    ACE_BT_BLE_UNREGISTER_BEACON_CLI_API,
    ACE_BT_BLE_GET_MAX_ADV_DATA_LEN_API,
    ACE_BT_BLE_START_BEACON_API,
    ACE_BT_BLE_START_BEACON_WITH_SCAN_RSP_API,
    ACE_BT_BLE_SET_PERIODIC_BEACON_PARAM_API,
    ACE_BT_BLE_STOP_BEACON_API,
    ACE_BT_BLE_START_BEACON_SCAN_API,
    ACE_BT_BLE_STOP_BEACON_SCAN_API,
    ACE_BT_BLE_API_MAX = 700,
    /*GATT SERVER */
    ACE_BT_BLE_REGISTER_GATT_SERVER_API,
    ACE_BT_BLE_UNREGISTER_GATT_SERVER_API,
    ACE_BT_BLE_GATT_ADD_SERVICE_API,
    ACE_BT_BLE_GATT_SEND_RESPONSE_API,
    ACE_BT_BLE_GATT_SET_MTU_API,
    ACE_BT_BLE_GATT_NOTIFY_CHARACTERISTICS_API,
    ACE_BT_BLE_GATT_REMOVE_SERVICE_API,
    ACE_BT_BLE_GATT_GET_BDADDR_API,
    ACE_BT_BLE_GATT_SERVER_API_MAX = 800,
    /* BLE API */
    ACE_BT_BLE_REGISTER_BLE_API = 801,
    ACE_BT_BLE_UNREGISTER_BLE_API = 802,
    ACE_BT_BLE_GET_MTU_API = 803,
    ACE_BT_BLE_SET_MTU_API = 804,
    ACE_BT_BLE_CONN_PARAM = 805,
    ACE_BT_BLE_CONN_REQ = 806,
    ACE_BT_BLE_CANCEL_CONN_REQ = 807,
    ACE_BT_BLE_DISC_REQ = 808,
    ACE_BT_BLE_READ_RSSI = 809,
    ACE_BT_BLE_CONN_PRIORITY = 810,

    ACE_BT_BLE_API_ADAPTER_MAX = 900,
    /** APIs for Sockets: Putting under classic for now for the purposes of SPP.
       Might move to common if/when we do BLE sockets*/
    ACE_BT_SOCKET_REGISTER_SERVER_CALLBACKS,
    ACE_BT_SOCKET_CREATE_SERVER_SOCKET,
    ACE_BT_SOCKET_ACCEPT_CLIENT_CONNECTION,
    ACE_BT_SOCKET_REGISTER_CLIENT_CALLBACKS,
    ACE_BT_SOCKET_CONNECT,
    ACE_BT_SOCKET_WRITE,
    ACE_BT_SOCKET_READ,
    ACE_BT_SOCKET_CLOSE,
    ACE_BT_SOCKET_ACL,
    ACE_BT_SOCKET_UP,
    ACE_BT_SOCKET_GET_LIST,
    ACE_BT_SOCKET_API_MAX = 1000,
    /*GATT Client */
    ACE_BT_BLE_REGISTER_GATT_CLIENT_API,
    ACE_BT_BLE_UNREGISTER_GATT_CLIENT_API,
    ACE_BT_BLE_GATT_CLIENT_DISC_ALL_SVC_API,
    ACE_BT_BLE_GATT_CLIENT_DISC_PRIMARY_SVC_API,
    ACE_BT_BLE_GATT_CLIENT_GET_SERVICE_API,
    ACE_BT_BLE_GATT_CLIENT_GET_SERVICE_BYUUID_API,
    ACE_BT_BLE_GATT_CLIENT_READ_CHARS_API,
    ACE_BT_BLE_GATT_CLIENT_WRITE_CHARS_API,
    ACE_BT_BLE_GATT_CLIENT_READ_DESC_API,
    ACE_BT_BLE_GATT_CLIENT_WRITE_DESC_API,
    ACE_BT_BLE_GATT_CLIENT_SET_NOTIFY_API,
    ACE_BT_BLE_GATT_CLIENT_BEGIN_RELIABLE_WRITE_API,
    ACE_BT_BLE_GATT_CLIENT_EXEC_RELIABLE_WRITE_API,
    ACE_BT_BLE_GATT_CLIENT_ATT_API,
    ACE_BT_BLE_GATT_CLIENT_REFRESH_DB_API,
    ACE_BT_BLE_GATT_CLIENT_API_MAX = 1100,
    /*BTPM testing*/
    ACE_BT_PM_GET_ACL_CONN_LIST_API = 1101,
    ACE_BT_PM_GET_CONN_LST_API = 1102,
    ACE_BT_PM_GET_AVAIL_CONN_CNT_API = 1103,
    ACE_BT_PM_API_REGISTER = 1104,
    ACE_BT_PM_SET_MAX_LINK_COUNT = 1105,
    ACE_BT_PM_API_MAX = 1200,
    /* AVRCP TG */
    ACE_BT_AVRCP_TG_INIT = 1201,
    ACE_BT_AVRCP_TG_REGISTER = 1202,
    ACE_BT_AVRCP_TG_SEND_METADATA = 1203,
    ACE_BT_AVRCP_TG_SEND_PLAY_STATUS = 1204,
    ACE_BT_AVRCP_TG_SEND_NOTIF = 1205,
    ACE_BT_AVRCP_TG_MAX = 1300,
    /* AVRCP CT */
    ACE_BT_AVRCP_CT_REGISTER = 1301,
    ACE_BT_AVRCP_CT_SEND_APP_SETTING = 1302,
    ACE_BT_AVRCP_CT_MAX = 1400,

    MAX_API = 0xFFFF, /* END of all API's */
} acebt_ipc_api_enum_t;

#define ACEBT_BT_CBACK_IDX_COMMON 0x0
#define ACEBT_BT_CBACK_IDX_CLASSIC 0x1
#define ACEBT_BT_CBACK_IDX_HFP 0x2
#define ACEBT_BT_CBACK_IDX_SOCKET_CLIENT 0x3
#define ACEBT_BT_CBACK_IDX_SOCKET_SERVER 0x4
#define ACEBT_BT_CBACK_IDX_BLE 0x5
#define ACEBT_BT_CBACK_IDX_BEACON 0x6
#define ACEBT_BT_CBACK_IDX_GATT_SERVER 0x7
#define ACEBT_BT_CBACK_IDX_GATT_CLIENT 0x8
#define ACEBT_BT_CBACK_IDX_PM 0x9
#define ACEBT_BT_CBACK_IDX_AVRCP_TG 0xA
#define ACEBT_BT_CBACK_IDX_AVRCP_CT 0xB
#define ACEBT_BT_CBACK_IDX_MAX MAX_BT_EVT_HANDLERS

/* BLE Gatt Client call back mask */
#define ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_REGISTERED 0x01
#define ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_DISCOVERED 0x02
#define ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_READ_CHARS 0x04
#define ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_WRITE_CHARS 0x08
#define ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_NOTIFY_CHARS 0x10
#define ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_DESC_WRITE 0x20
#define ACEBT_IPC_CBACK_MASK_GATTC_SERVICE_DESC_READ 0x40
#define ACEBT_IPC_CBACK_MASK_GATTC_GET_DB 0x80
#define ACEBT_IPC_CBACK_MASK_GATTC_EXECUTE_WRITE 0x100

typedef enum {
    /* Common Callback Interfaces under ACEBT_BT_CBACK_IDX_COMMON */
    ACE_BT_CALLBACK_COMMON_INIT = 1,
    ACE_BT_CALLBACK_SERVER_RESTARTED = 2,
    ACE_BT_CALLBACK_ADAPTER_STATE_CHANGED = 3,
    ACE_BT_CALLBACK_BOND_STATE_CHANGED = 4,
    ACE_BT_CALLBACK_PIN_REQ_CALLBACK = 5,
    ACE_BT_CALLBACK_SSP_REQ_CALLBACK = 6,
    ACE_BT_CALLBACK_ACL_CALLBACK = 7,
    ACE_BT_CALLBACK_COMMON_MAX = 10,
    /* Classic Callback Interfaces under ACEBT_BT_CBACK_IDX_CLASSIC */
    ACE_BT_CALLBACK_CLASSIC_INIT = 11,
    ACE_BT_CALLBACK_CONN_STATE_CHANGED = 12,
    ACE_BT_CALLBACK_PROFILE_STATE_CHANGED = 13,
    ACE_BT_CALLBACK_AUDIO_STATE_CHANGED = 14,
    ACE_BT_CALLBACK_DISCOVERY_STATE_CHANGED = 15,
    ACE_BT_CALLBACK_DEVICE_FOUND = 16,
    ACE_BT_CALLBACK_SCAN_MODE_CHANGED = 17,
    ACE_BT_CALLBACK_SCAN_TYPE_CHANGED = 18,
    ACE_BT_CALLBACK_ACL_PRIORITY_CHANGED = 19,
    ACE_BT_CALLBACK_CLASSIC_MAX = 20,
    /* BT HFP Callback Interfaces under ACEBT_BT_CBACK_IDX_HFP */
    ACE_BT_CALLBACK_HFP_INIT = 21,
    ACE_BT_CALLBACK_HFP_CONN_STATE = 22,
    ACE_BT_CALLBACK_HFP_AUDIO_STATE = 23,
    ACE_BT_CALLBACK_HFP_ANSWER = 24,
    ACE_BT_CALLBACK_HFP_HANGUP = 25,
    ACE_BT_CALLBACK_HFP_DIAL = 26,
    ACE_BT_CALLBACK_HFP_CHLD = 27,
    ACE_BT_CALLBACK_HFP_CLCC = 28,
    ACE_BT_CALLBACK_HFP_BIND = 29,
    ACE_BT_CALLBACK_HFP_MAX = 30,
    /* BT Socket Client Callback Interfaces under
       ACEBT_BT_CBACK_IDX_SOCKET_SERVER */
    ACE_BT_CALLBACK_SOCKET_CLIENT_INIT,
    ACE_BT_CALLBACK_SOCKET_CLIENT_CONN,
    ACE_BT_CALLBACK_SOCKET_CLIENT_DATA_READY,
    ACEBT_BT_CALLBACK_SOCKET_CLIENT_SDP_UUID_FOUND,
    ACE_BT_CALLBACK_SOCKET_CLIENT_MAX = 40,
    /* BT Socket Server Callback Interfaces under
       ACEBT_BT_CBACK_IDX_SOCKET_CLIENT */
    ACE_BT_CALLBACK_SOCKET_SERVER_INIT,
    ACE_BT_CALLBACK_SOCKET_SERVER_CONN_REQ,
    ACE_BT_CALLBACK_SOCKET_SERVER_CONN_CFM,
    ACE_BT_CALLBACK_SOCKET_SERVER_MAX = 50,
    /* BLE Callback Interfaces under ACEBT_BT_CBACK_IDX_BLE*/
    ACE_BT_CALLBACK_BLE_INIT,
    ACE_BT_CALLBACK_BLE_REGISTERED,
    ACE_BT_CALLBACK_BLE_CONN_STATE_CHANGED,
    ACE_BT_CALLBACK_BLE_MTU_CHANGED,
    ACE_BT_CALLBACK_BLE_READ_RSSI,
    ACE_BT_CALLBACK_BLE_MAX = 60,
    /* BLE Beacon Manager Callback Interfaces under ACEBT_BT_CBACK_IDX_BEACON */
    ACE_BT_CALLBACK_BEACON_INIT,
    ACE_BT_CALLBACK_ADV_STATE_CHANGED,
    ACE_BT_CALLBACK_SCAN_STATE_CHANGED,
    ACE_BT_CALLBACK_SCAN_RESULT,
    ACE_BT_CALLBACK_BEACON_REGISTERED,
    ACE_BT_CALLBACK_BEACON_MAX = 70,
    /* BLE GATT Server Callback Interfaces under
       ACEBT_BT_CBACK_IDX_GATT_SERVER */
    ACE_BT_CALLBACK_GATTS_INIT,
    ACE_BT_CALLBACK_GATTS_SERVICE_STARTED,
    ACE_BT_CALLBACK_GATTS_CHARS_WRITE_REQ,
    ACE_BT_CALLBACK_GATTS_CHARS_READ_REQ,
    ACE_BT_CALLBACK_GATTS_DESC_WRITE_REQ,
    ACE_BT_CALLBACK_GATTS_DESC_READ_REQ,
    ACE_BT_CALLBACK_GATTS_EXEC_WRITE_REQ,
    ACE_BT_CALLBACK_GATTS_MTU_CHANGED_REQ,
    ACE_BT_CALLBACK_GATTS_NOTIFY_SENT,
    ACE_BT_CALLBACK_GATTS_CONN_INDICATION,
    ACE_BT_CALLBACK_GATTS_MAX = 80,

    /* BT Policy Manager Callback Interfaces under ACEBT_BT_CBACK_IDX_PM */
    ACE_BT_CALLBACK_PM_CONN_LIST = 81,
    ACE_BT_CALLBACK_PM_ACL_LIST = 82,
    ACE_BT_CALLBACK_PM_MAX = 90,

    /* BLE GATT Client Callback Interfaces under ACEBT_BT_CBACK_IDX_GATT_CLIENT
     */
    ACE_BT_CALLBACK_GATTC_INIT,
    ACE_BT_CALLBACK_GATTC_SERVICE_REGISTERED,
    ACE_BT_CALLBACK_GATTC_SERVICE_DISCOVERED,
    ACE_BT_CALLBACK_GATTC_CHARS_READ_RSP,
    ACE_BT_CALLBACK_GATTC_CHARS_WRITE_RSP,
    ACE_BT_CALLBACK_GATTC_EXEC_WRITE_RSP,
    ACE_BT_CALLBACK_GATTC_NOTIFY_CHARS_CHANGED,
    ACE_BT_CALLBACK_GATTC_DESC_WRITE_RSP,
    ACE_BT_CALLBACK_GATTC_DESC_READ_RSP,
    ACE_BT_CALLBACK_GATTC_GET_DB_RSP,
    ACE_BT_CALLBACK_GATTC_MAX = 100,

    /* BT AVRCP Target Callback Interfaces under ACEBT_BT_CBACK_IDX_AVRCP_TG
     */
    ACE_BT_CALLBACK_AVRCP_TG_INIT = 101,
    ACE_BT_CALLBACK_AVRCP_TG_GET_METADATA = 102,
    ACE_BT_CALLBACK_AVRCP_TG_GET_PLAY_STATUS = 103,
    ACE_BT_CALLBACK_AVRCP_TG_REG_NOTIFICATION = 104,
    ACE_BT_CALLBACK_AVRCP_TG_PASSTHROUGH_CMD = 105,
    ACE_BT_CALLBACK_AVRCP_TG_REMOTE_FEATURES = 106,
    ACE_BT_CALLBACK_AVRCP_TG_MAX = 110,

    /* BT AVRCP Controller Callback Interfaces under ACEBT_BT_CBACK_IDX_AVRCP_CT
     */
    ACE_BT_CALLBACK_AVRCP_CT_INIT = 111,
    ACE_BT_CALLBACK_AVRCP_CT_CONN_STATE = 112,
    ACE_BT_CALLBACK_AVRCP_CT_REMOTE_FEATURES = 113,
    ACE_BT_CALLBACK_AVRCP_CT_SET_APP_SETTING_RSP = 114,
    ACE_BT_CALLBACK_AVRCP_CT_APP_SETTING_CHANGED = 115,
    ACE_BT_CALLBACK_AVRCP_CT_NOTIF = 116,
    ACE_BT_CALLBACK_AVRCP_CT_MAX = 120,
} acebt_ipc_evt_enum_t;
typedef acebt_ipc_evt_enum_t ipc_evt_enum_t;

typedef struct {
    uint32_t size;
    uint32_t conn_handle;
    uuid_t uuid;
} __attribute__((packed)) acebt_gattc_get_db_data_t;
typedef acebt_gattc_get_db_data_t gattc_get_db_data_t;

void serialize_ble_get_db_req(gattc_get_db_data_t* data, uint32_t conn_handle, uuid_t* uuid);

// Reversed struct. Here be dragons
typedef struct {
    uint16_t reserved0; // 0x00: always 0x0000?

    uint16_t size;     // 0x02: 30 00  → 0x30 (48)
    uint32_t msg_type; // 0x04: 02 00 00 00  → 0x02 (2)

    uint32_t function_id; // 0x08: 3e 00 00 00  -> 0x3E (62)

    void* cb1;    // 0x0C: 70 44 50 b5
    void* cb2;    // 0x10: 74 44 50 b5
    void* buffer; // 0x14: 48 45 50 b5

    uint32_t reserved1; // 0x18: 00 00 00 00
    uint32_t reserved2; // 0x1C: 00 00 00 00
    uint32_t reserved3; // 0x20: 00 00 00 00

    uint32_t flags;     // 0x24: 01 00 00 00
    uint32_t reserved4; // 0x28: 00 00 00 00

    /* Matches aceBT_aipcHandles_t dump at 0x2C–0x2F */
    uint16_t callback_id; // 0x2C: 4c 19  → 0x194C (6476)
    uint16_t server_id;   // 0x2E: f0 b6  → 0xB6F0 (46832)
} aceAipc_parameter_t;

/** NOTE: These two functions are used in the BT event handlers.
 * getSessionForCallback was used <5.17
 * Since >=5.17 they switched to getSessionForTask
 * We cannot define them or the linker will fail to resolve
 */
// /** Returns ACE BT Session associated with a callback ID
//  *
//  * @param callback_client_id Callback ID
//  * @return session handle if valid session found else error code
//  */
// sessionHandle getSessionForCallback(uint16_t callback_client_id);
// sessionHandle getSessionForTask(aceAipc_parameter_t* task);

typedef void (*bt_event_handler)(aceAipc_parameter_t* task);

/** Registers an AIPC event handler with session manager to receive BT manager
 * events
 *
 * @param sessionHandle session handle for current Bluetooth session
 * @return SUCCESS if registration is successful, else error
 */
status_t registerBTEvtHandler(
    sessionHandle sessionHandle, bt_event_handler cb, acebt_ipc_evt_enum_t min,
    acebt_ipc_evt_enum_t max
);

status_t aipc_invoke_sync_call(uint16_t func_id, void* payload, uint32_t len);

/**
 * Retrieve the client callback pointer for a specific session handle. If the
 * session handle
 * is not found in the session manager, an appropriate error code will be
 * returned.
 *
 * @param sessionHandle session handle for current Bluetooth session
 * @param  dataIndex index of this data within senssion
 * @return ACEBT_STATUS_SUCCESS if sessionHandle is valid; else
 * ACEBT_STATUS_INVALID_PARAM
 */
void* getBTClientData(sessionHandle sessionHandle, uint8_t dataIndex);

/**
 * Used for the pre 5.17.0 bleDiscoverAllServices implementation
 */
typedef struct {
    uint32_t size;
    uint32_t session_handle;
    uint32_t conn_handle;
    status_t out_status;
} __attribute__((packed)) acebt_request_disc_all_svc_t;
typedef acebt_request_disc_all_svc_t request_disc_all_svc_t;

void serialize_gattc_disc_all_svc(request_disc_all_svc_t* data, uint32_t conn_handle);

/**
 * Used during BT event handler GATT Client discover all services operations
 */
typedef struct {
    uint32_t size;
    uint32_t conn_handle;
    status_t out_status;
} __attribute__((packed)) acebt_dis_req_t;
typedef acebt_dis_req_t dis_req_t;

/**
 * Used during BT event handler GATT Client get DB operations
 */
typedef struct {
    bleGattServiceType_t serviceType;
    uuid_t uuid;
} __attribute__((packed)) acebt_included_svc_t;
typedef acebt_included_svc_t included_svc_t;

typedef enum {
    ACEBT_BLE_GATT_CHARACTERISTICS,
    ACEBT_BLE_GATT_DESCRIPTOR,
    ACEBT_BLE_GATT_INC_SVC,
} aceBT_bleSerealizedGattDataType_t;
typedef aceBT_bleSerealizedGattDataType_t bleSerealizedGattDataType_t;

typedef struct {
    bleSerealizedGattDataType_t dataType;
    union {
        bleGattCharacteristicsValue_t charValue;
        bleGattDescriptor_t descValue;
        included_svc_t svc;
    };
} acebt_service_data_t;
typedef acebt_service_data_t service_data_t;

typedef struct {
    uint8_t is_unregister;
    uint16_t size;
    uint16_t callback_mask;
    uint32_t session_handle;
    uint32_t conn_handle;
    status_t out_status;
    bleGattServiceType_t serviceType;
    uuid_t uuid;
    uint16_t no_svc_data;
    service_data_t svc_data[];
} acebt_register_cback_gatts_t;
typedef acebt_register_cback_gatts_t register_cback_gatts_t;

typedef struct {
    uint16_t no_svc;
    uint32_t conn_handle;
    register_cback_gatts_t svc_list[];
} acebt_register_cback_gatts_list_t;
typedef acebt_register_cback_gatts_list_t register_cback_gatts_list_t;

void deserealize_all_gatts_register_data(
    register_cback_gatts_list_t* data, bleGattsService_t** gattService, uint32_t* no_svc
);

void cleanup_all_service(bleGattsService_t* service, int no_svc);

/**
 * Used for the pre 5.17.0 bleReadCharacteristics implementation
 */
typedef struct {
    uint32_t size;
    uint32_t conn_handle;
    bleGattCharacteristicsValue_t chars;
    status_t status;
} __attribute__((packed)) acebt_gattc_read_chars_req_data_t;
typedef acebt_gattc_read_chars_req_data_t gattc_read_chars_req_data_t;

void serialize_gattc_read_chars_req(
    gattc_read_chars_req_data_t* data, uint32_t conn_handle,
    bleGattCharacteristicsValue_t charsValue
);

/**
 * Used during BT event handler GATT Client read characteristic operations
 */
typedef struct {
    uint32_t size;
    uint32_t conn_handle;
    bleGattCharacteristicsValue_t value;
    status_t status;
    uint32_t data_len;
    uint8_t data[];
} __attribute__((packed)) acebt_gattc_read_chars_data_t;
typedef acebt_gattc_read_chars_data_t gattc_read_chars_data_t;

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE_INTERNALS
