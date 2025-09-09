#define _GNU_SOURCE

#include "compat_ace_shims.h"

#include <dlfcn.h>

#include "log.h"

// Used in the bleWriteCharacteristics and bleWriteDescriptor calls.
// I don't want to declare it because that would be a whole new linking dependency
typedef void (*aceAlloc_free_fn_t)(int, int, void*);

void shim_aceAlloc_free(int module_id, int buf_type, void* p) {
    static aceAlloc_free_fn_t api = NULL;
    static bool initialized = false;

    if (!initialized) {
        api = (aceAlloc_free_fn_t)dlsym(RTLD_DEFAULT, "aceAlloc_free");
        initialized = true;
    }

    if (api == NULL) {
        log_error("[%s()]: couldn't match aceAlloc_free symbol. This is not supposed to happen");
        return;
    }

    return api(module_id, buf_type, p);
}
