#ifndef KINDLE_BT_UTILS_H
#define KINDLE_BT_UTILS_H

#include <string.h>

#include "kindlebt_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

status_t utilsConvertStrToBdAddr(char* str, bdAddr_t* pAddr);

#ifdef __cplusplus
}
#endif

#endif // KINDLE_BT_UTILS_H
