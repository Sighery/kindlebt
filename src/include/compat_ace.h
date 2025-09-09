#ifndef COMPAT_ACE_H
#define COMPAT_ACE_H

#include "compat_ace_internals.h"
#include "compat_ace_shims.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PRE_5170 = 1,   // < 5.17.0
    SINCE_5170 = 2, // >= 5.17.0
} acebt_abi;

acebt_abi acebt_abi_version(void);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE_H
