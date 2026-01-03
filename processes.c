#include "serial.h"

void procA(void) {
    while (1) {
        serial_puts("A\n");
        for (volatile int i = 0; i < 1000000; i++);
    }
}

void procB(void) {
    while (1) {
        serial_puts("B\n");
        for (volatile int i = 0; i < 1000000; i++);
    }
}
