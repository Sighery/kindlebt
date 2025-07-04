#include <stdio.h>
#include <unistd.h>

#include "kindlebt.h"

static sessionHandle bt_session = NULL;

int main() {
    // The ace_bt stuff won't run under root user
    if (setgid((gid_t)1003) || setuid((uid_t)1003)) {
        fprintf(stderr, "Can't drop privileges to bluetooth user/group\n");
        return -1;
    }

    printf("Hello World from Kindle!\n");

    bool isBLE = isBLESupported();
    printf("Is BLE enabled: %d\n", isBLE);

    status_t session_status = openSession(ACEBT_SESSION_TYPE_DUAL_MODE, &bt_session);
    printf("Opened session status: %d\n", session_status);

    session_status = closeSession(bt_session);
    printf("Closed session status: %d\n", session_status);

    return 0;
}
