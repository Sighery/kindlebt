#include "kindlebt_application.h"
#include "kindlebt_defines.h"

bleGattClientCallbacks_t application_gatt_client_callbacks = {
    .size = sizeof(bleGattClientCallbacks_t),
};
