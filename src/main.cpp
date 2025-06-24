#include <stdio.h>
#include "ace_bt.h"

int main() {
    printf("Hello World from Kindle!\n");

    bool isBLE = aceBT_isBLESupported();
    printf("Is BLE enabled: %d\n", isBLE);

    return 0;
}
