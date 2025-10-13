/**
 * @file kindlebt_defines.h
 * @brief Internal definitions
 *
 * This file contains some internal structures used in kindlebt as well as some typedefs
 * of internal \c darkroot data structures. For more information on some of these types,
 * you should get the \c darkroot subproject from the Meson project, and from there you
 * can check it out locally and look through it.
 * @sa @ref darkroot
 */

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

/**
 * @defgroup DARKROOT_DS Darkroot Data Structures
 *
 * @brief darkroot-specific data structures
 *
 * For more information on some of these types, you should get the \c darkroot subproject
 * from the Meson project, and from there you can check it out locally and look through
 * it.
 * @sa @ref darkroot
 *
 * @{
 */

/**
 * @name MAC address
 * @{
 */
/**
 * @brief MAC address length
 */
#define MAC_ADDR_LEN ACEBT_MAC_ADDR_LEN
/**
 * @brief MAC address string representation length (XX:XX:XX:XX:XX:XX)
 */
#define MAC_ADDR_STR_LEN ACEBT_MAC_ADDR_STR_LEN
/** @} */

/**
 * @name BLE attribute format
 *
 * @brief Attribute format used for @ref bleGattAttributeFormat
 *
 * @{
 */
/** @brief UINT8 format */
#define BLE_FORMAT_UINT8 ACEBT_BLE_FORMAT_UINT8
/** @brief UINT16 format */
#define BLE_FORMAT_UINT16 ACEBT_BLE_FORMAT_UINT16
/** @brief UINT32 format */
#define BLE_FORMAT_UINT32 ACEBT_BLE_FORMAT_UINT32
/** @brief SINT8 format */
#define BLE_FORMAT_SINT8 ACEBT_BLE_FORMAT_SINT8
/** @brief SINT16 format */
#define BLE_FORMAT_SINT16 ACEBT_BLE_FORMAT_SINT16
/** @brief SINT32 format */
#define BLE_FORMAT_SINT32 ACEBT_BLE_FORMAT_SINT32
/** @brief SFLOAT format */
#define BLE_FORMAT_SFLOAT ACEBT_BLE_FORMAT_SFLOAT
/** @brief FLOAT format */
#define BLE_FORMAT_FLOAT ACEBT_BLE_FORMAT_FLOAT
/** @brief BLOB format */
#define BLE_FORMAT_BLOB ACEBT_BLE_FORMAT_BLOB
/** @} */

/**
 * @brief Bluetooth radio state
 */
typedef aceBT_state_t state_t;
/**
 * @brief Bluetooth API status codes
 */
typedef ace_status_t status_t;
/**
 * @brief Bluetooth address
 */
typedef aceBT_bdAddr_t bdAddr_t;
/**
 * @brief Bluetooth bond state
 */
typedef aceBT_bondState_t bondState_t;
/**
 * @brief Bluetooth UUID types
 */
typedef aceBT_UUIDType_t UUIDType_t;
/**
 * @brief Bluetooth UUID struct
 *
 * The length will depend on the chosen @ref UUIDType_t
 */
typedef aceBT_uuid_t uuid_t;
/**
 * @brief Type of write operation for a BLE characteristic
 */
typedef aceBT_responseType_t responseType_t;

/**
 * @brief Session handle for the lifetime of the Bluetooth application
 */
typedef aceBT_sessionHandle sessionHandle;
/**
 * @brief Bluetooth session type (Classic, Low Energy, Dual)
 */
typedef aceBT_sessionType_t sessionType_t;

/**
 * @brief Connection handle for the lifetime of a Bluetooth connection
 */
typedef aceBT_bleConnHandle bleConnHandle;
/**
 * @brief BLE connection state
 */
typedef aceBT_bleConnState_t bleConnState_t;
/**
 * @brief BLE connection parameters
 */
typedef aceBt_bleConnParam_t bleConnParam_t;
/**
 * @brief BLE connection role
 */
typedef aceBT_bleConnRole_t bleConnRole_t;
/**
 * @brief BLE connection priority
 */
typedef aceBt_bleConnPriority_t bleConnPriority_t;

/**
 * @brief BLE GATT status
 */
typedef aceBT_gattStatus_t gattStatus_t;
/**
 * @brief BLE application type
 */
typedef aceBt_bleAppId_t bleAppId_t;
/**
 * @brief Structure for a GATT Server service
 */
typedef aceBT_bleGattsService_t bleGattsService_t;
/**
 * @brief BLE attribute format
 */
typedef aceBT_bleGattAttributeFormat bleGattAttributeFormat;
/**
 * @brief BLE blob value
 *
 * When reading/writing a characteristic value/descriptor, data will be given through
 * this struct.
 */
typedef aceBT_bleGattBlobValue_t bleGattBlobValue_t;
/**
 * @brief BLE GATT Characteristic
 */
typedef aceBT_bleGattCharacteristicsValue_t bleGattCharacteristicsValue_t;
/**
 * @brief BLE GATT Descriptor
 */
typedef aceBT_bleGattDescriptor_t bleGattDescriptor_t;
/**
 * @brief BLE GATT Record (ATT attributes)
 */
typedef aceBT_bleGattRecord_t bleGattRecord_t;
/**
 * @brief BLE GATT service type
 */
typedef aceBT_bleGattServiceType_t bleGattServiceType_t;

/**
 * @brief Callback struct of common Classic/LE Bluetooth operations
 *
 * @remark For a list of available callbacks, search for the original type in
 * @ref darkroot
 *
 * @note Generally no function expects this struct directly, but rather this struct is
 * embedded into the BLE/Classic callbacks structs (such as @ref bleCallbacks_t)
 */
typedef aceBT_commonCallbacks_t commonCallbacks_t;
/**
 * @brief Callback struct of LE Bluetooth operations
 *
 * @remark For a list of available callbacks, search for the original type in
 * @ref darkroot
 */
typedef aceBT_bleCallbacks_t bleCallbacks_t;
/**
 * @brief Callback struct of GATT Client Bluetooth operations
 *
 * @remark For a list of available callbacks, search for the original type in
 * @ref darkroot
 */
typedef aceBT_bleGattClientCallbacks_t bleGattClientCallbacks_t;

/** @} */ // DARKROOT_DS

/**
 * @internal
 * @defgroup KINDLEBT_DS Kindlebt Data Structures
 * @brief kindlebt-specific data structures
 *
 * @warning As an application, you shouldn't need to use or interact with any of these
 * structs/variables. These are all for use within the internal kindlebt code to provide
 * the API. However, they are documented here since it can help contributors.
 *
 * @{
 */

/**
 * @brief Internal struct for certain Bluetooth conditions
 *
 * Most of the Bluetooth operations are asynchronous, even if the API call returns
 * immediately. This is because the API call return just indicates whether the request
 * was made successfully or not. To know whether the Bluetooth hardware completed your
 * request (or failed to), you'll need to sign up for different callbacks.
 *
 * For some of these operations, kindlebt will set up its own callbacks and wait for the
 * condition, this way we can provide a synchronous behaviour to a few of the API calls.
 */
typedef struct {
    bool bt_enabled;
    bool ble_registered;
    bool mtu_set;
    bool gattc_connected;
    bool gattc_disconnected;
    bool gattc_discovered;
    bool got_gatt_db;
} bleCallbackVars_t;

/**
 * @brief Internal @ref bleCallbackVars_t
 */
extern bleCallbackVars_t callback_vars;
/**
 * @brief Internal number of GATT services, often paired with @ref pGgatt_service
 */
extern uint32_t gNo_svc;
/**
 * @brief Internal reference to a GATT service
 */
extern bleGattsService_t* pGgatt_service;
/**
 * @brief Internal BLE connection handle
 */
extern bleConnHandle ble_conn_handle;

/** @} */ // KINDLEBT_DS

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_DEFINES_H
