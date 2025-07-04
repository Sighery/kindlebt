#include <stdio.h>

#include "kindlebt.h"

int main() {
    printf("Hello World from Kindle!\n");

    bool isBLE = isBLESupported();
    printf("Is BLE enabled: %d\n", isBLE);

    return 0;
}
