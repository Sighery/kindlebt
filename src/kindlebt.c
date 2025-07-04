#include "ace/bluetooth_common_api.h"

bool isBLESupported(void) {
    return aceBT_isBLESupported();
};
