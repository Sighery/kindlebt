// #ifndef ACE_BT_H
// #define ACE_BT_H

extern "C" {

/**
 * @brief Function for clients to identify if BLE is supported by the current
 * Bluetooth adapter.
 *
 * @return TRUE if BLE is supported by current adapter; FALSE otherwise
 */
bool aceBT_isBLESupported(void);

// #endif

}
