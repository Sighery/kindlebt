#define _GNU_SOURCE

#include "compat_ace.h"

#include <dlfcn.h>
#include <stdbool.h>
#include <stdio.h>

#include "log.h"

// TODO: Might want to turn it into a bitmask instead
acebt_abi acebt_abi_version(void) {
    static acebt_abi cached_abi = PRE_5170;
    static bool initialized = false;

    if (!initialized) {
        void* handle = dlopen(NULL, RTLD_LAZY);
        if (handle) {
            dlerror();
            void* sym = dlsym(handle, "aceBt_bleRegisterGattClient");
            if (sym) {
                cached_abi = SINCE_5170;
            }
        } else {
            log_error("dlopen failed: %s", dlerror());
        }
        initialized = true;
    }

    return cached_abi;
}
