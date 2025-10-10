#ifndef COMPAT_ACE_HANDLER_H
#define COMPAT_ACE_HANDLER_H

#include "compat_ace_internals.h"

#ifdef __cplusplus
extern "C" {
#endif

void pre5170_gattc_cb_handler(aceAipc_parameter_t* task);

#ifdef __cplusplus
}
#endif

#endif // COMPAT_ACE_HANDLER_H
