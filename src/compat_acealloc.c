#define _GNU_SOURCE

#include <kindlebt/compat_ace_shims.h>

#include <dlfcn.h>

#include "log.h"

// Used in the bleWriteCharacteristics and bleWriteDescriptor calls.
// I don't want to declare it because that would be a whole new linking dependency

// Defined in ace/ace_modules.h
#define ACE_MODULE_BT 33
// Defined in ace/osal_alloc.h
#define ACE_ALLOC_BUFFER_GENERIC 0

typedef void (*aceAlloc_free_fn_t)(int, int, void*);
void shadow_aceAlloc_free(void* p) {
    static aceAlloc_free_fn_t api = NULL;
    static bool initialized = false;

    if (!initialized) {
        api = (aceAlloc_free_fn_t)dlsym(RTLD_DEFAULT, "aceAlloc_free");
        initialized = true;
    }

    if (api == NULL) {
        log_error(
            "[%s()]: couldn't match aceAlloc_free symbol. This is not supposed to happen", __func__
        );
        return;
    }

    return api(ACE_MODULE_BT, ACE_ALLOC_BUFFER_GENERIC, p);
}
