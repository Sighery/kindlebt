#define _GNU_SOURCE

#include "compat_ace_shims.h"

#include <dlfcn.h>
#include <stdbool.h>

#include "log.h"

#include "compat_ace_implementations.h"

/* Internal typedefs matching the two getSessionFor* functions
 * getSessionForCallback < 5.17
 * getSessionForTask >= 5.17
 * Our shim is getSessionFromHandler
 */
typedef sessionHandle (*getSessionForCallback_fn_t)(uint16_t);
typedef sessionHandle (*getSessionForTask_fn_t)(aceAipc_parameter_t*);

sessionHandle getSessionFromHandler(aceAipc_parameter_t* task) {
    static getSessionForCallback_fn_t old_api = NULL;
    static getSessionForTask_fn_t new_api = NULL;
    static bool initialized = false;

    if (!initialized) {
        new_api = (getSessionForTask_fn_t)dlsym(RTLD_DEFAULT, "getSessionForTask");
        if (!new_api) {
            old_api = (getSessionForCallback_fn_t)dlsym(RTLD_DEFAULT, "getSessionForCallback");
        }
        initialized = true;
    }

    if (new_api) {
        return new_api(task);
    } else if (old_api) {
        return old_api(task->callback_id);
    } else {
        // Nothing matched. We shouldn't reach this
        log_error("[%s()]: couldn't match any of the expected getSessionFor* symbols", __func__);
        return (sessionHandle)-1;
    }
}

typedef status_t (*aceBt_bleRegisterGattClient_tn_t)(
    sessionHandle, bleGattClientCallbacks_t*, bleAppId_t
);

status_t shim_bleRegisterGattClient(
    sessionHandle session_handle, bleGattClientCallbacks_t* callbacks, bleAppId_t app_id
) {
    static aceBt_bleRegisterGattClient_tn_t new_api = NULL;

#ifndef FORCE_OLD_API
    static bool initialized = false;

    if (!initialized) {
        new_api =
            (aceBt_bleRegisterGattClient_tn_t)dlsym(RTLD_DEFAULT, "aceBt_bleRegisterGattClient");
        initialized = true;
    }
#endif

    if (new_api) {
        return new_api(session_handle, callbacks, app_id);
    } else {
        return pre5170_bleRegisterGattClient(session_handle, callbacks, app_id);
    }
}

typedef status_t (*aceBT_bleDeRegisterGattClient_fn_t)(sessionHandle);

status_t shim_bleDeregisterGattClient(sessionHandle session_handle) {
    static aceBT_bleDeRegisterGattClient_fn_t new_api = NULL;

#ifndef FORCE_OLD_API
    static bool initialized = false;

    if (!initialized) {
        new_api = (aceBT_bleDeRegisterGattClient_fn_t
        )dlsym(RTLD_DEFAULT, "aceBT_bleDeRegisterGattClient");
        initialized = true;
    }
#endif

    if (new_api) {
        return new_api(session_handle);
    } else {
        return pre5170_bleDeregisterGattClient(session_handle);
    }
}

typedef status_t (*aceBT_bleDiscoverAllServices_fn_t)(sessionHandle, bleConnHandle);

status_t shim_bleDiscoverAllServices(sessionHandle session_handle, bleConnHandle conn_handle) {
    static aceBT_bleDiscoverAllServices_fn_t new_api = NULL;

#ifndef FORCE_OLD_API
    static bool initialized = false;

    if (!initialized) {
        new_api =
            (aceBT_bleDiscoverAllServices_fn_t)dlsym(RTLD_DEFAULT, "aceBT_bleDiscoverAllServices");
        initialized = true;
    }
#endif

    if (new_api) {
        return new_api(session_handle, conn_handle);
    } else {
        return pre5170_bleDiscoverAllServices(session_handle, conn_handle);
    }
}

typedef status_t (*aceBT_bleGetService_fn_t)(bleConnHandle);

status_t shim_bleGetDatabase(bleConnHandle conn_handle) {
    static aceBT_bleGetService_fn_t new_api = NULL;

#ifndef FORCE_OLD_API
    static bool initialized = false;

    if (!initialized) {
        new_api = (aceBT_bleGetService_fn_t)dlsym(RTLD_DEFAULT, "aceBT_bleGetService");
        initialized = true;
    }
#endif

    if (new_api) {
        return new_api(conn_handle);
    } else {
        return pre5170_bleGetService(conn_handle);
    }
}

typedef status_t (*aceBT_bleReadCharacteristics_fn_t)(
    sessionHandle, bleConnHandle, bleGattCharacteristicsValue_t
);

status_t shim_bleReadCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t chars_value
) {
    static aceBT_bleReadCharacteristics_fn_t new_api = NULL;

#ifndef FORCE_OLD_API
    static bool initialized = false;

    if (!initialized) {
        new_api =
            (aceBT_bleReadCharacteristics_fn_t)dlsym(RTLD_DEFAULT, "aceBT_bleReadCharacteristics");
        initialized = true;
    }
#endif

    if (new_api) {
        return new_api(session_handle, conn_handle, chars_value);
    } else {
        return pre5170_bleReadCharacteristic(session_handle, conn_handle, chars_value);
    }
}

typedef status_t (*aceBT_bleWriteCharacteristics_fn_t)(
    sessionHandle, bleConnHandle, bleGattCharacteristicsValue_t*, responseType_t
);

status_t shim_bleWriteCharacteristic(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
) {
    static aceBT_bleWriteCharacteristics_fn_t new_api = NULL;

#ifndef FORCE_OLD_API
    static bool initialized = false;

    if (!initialized) {
        new_api = (aceBT_bleWriteCharacteristics_fn_t
        )dlsym(RTLD_DEFAULT, "aceBT_bleWriteCharacteristics");
        initialized = true;
    }
#endif

    if (new_api) {
        return new_api(session_handle, conn_handle, chars_value, request_type);
    } else {
        return pre5170_bleWriteCharacteristics(
            session_handle, conn_handle, chars_value, request_type
        );
    }
}

typedef status_t (*aceBT_bleWriteDescriptor_fn_t)(
    sessionHandle, bleConnHandle, bleGattCharacteristicsValue_t*, responseType_t
);

status_t shim_bleWriteDescriptor(
    sessionHandle session_handle, bleConnHandle conn_handle,
    bleGattCharacteristicsValue_t* chars_value, responseType_t request_type
) {
    static aceBT_bleWriteDescriptor_fn_t new_api = NULL;

#ifndef FORCE_OLD_API
    static bool initialized = false;

    if (!initialized) {
        new_api =
            (aceBT_bleWriteCharacteristics_fn_t)dlsym(RTLD_DEFAULT, "aceBT_bleWriteDescriptor");
    }
#endif

    if (new_api) {
        return new_api(session_handle, conn_handle, chars_value, request_type);
    } else {
        return pre5170_bleWriteDescriptor(session_handle, conn_handle, chars_value, request_type);
    }
}
