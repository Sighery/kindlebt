#include <pthread.h>
#include <stdbool.h>

#include "kindlebt_defines.h"

#include "kindlebt_utils.c"

pthread_mutex_t callback_vars_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t callback_vars_cond = PTHREAD_COND_INITIALIZER;
bleCallbackVars_t callback_vars;

void adapterStateCallback(state_t state) {
    if (state == ACEBT_STATE_ENABLED) {
        printf("Callback : adapter_state_callback() state: STATE_ENABLED");
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.bt_enabled, true
        );
    } else if (state == ACEBT_STATE_DISABLED) {
        printf("Callback : adapter_state_callback() state: STATE_DISABLED");
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.bt_enabled, false
        );
    }
}

void bleRegCallback(status_t status) {
    printf("Callback : bleRegCallback() status: %d\n", status);

    if (status == ACE_STATUS_OK) {
        setCallbackVariable(
            &callback_vars_lock, &callback_vars_cond, &callback_vars.ble_registered, true
        );
    }
}
