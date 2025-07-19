#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "kindlebt_log.h"

void kindlebt_set_log_level(int level) { log_set_level(level); }
