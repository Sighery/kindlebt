#include "kindlebt_log.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

void kindlebt_set_log_level(int level) { log_set_level(level); }

char* append_to_buffer(char* buf, size_t* size, size_t* offset, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    size_t needed;
    char tmp[512];
    needed = vsnprintf(tmp, sizeof(tmp), fmt, args);

    if (*offset + needed >= *size) {
        *size = (*offset + needed + 1) * 2;
        buf = realloc(buf, *size);
        if (!buf) {
            va_end(args);
            return NULL;
        }
    }

    snprintf(buf + *offset, *size - *offset, "%s", tmp);
    *offset += needed;

    va_end(args);
    return buf;
}
