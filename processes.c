void procA(void) {
    while (1) {
        serial_puts("A running...\n");

        for (volatile int i = 0; i < 1000000; i++);

        serial_puts("A done...\n");
        schedule();
    }
}

void procB(void) {
    while (1) {
        serial_puts("B running...\n");

        for (volatile int i = 0; i < 1000000; i++);

        serial_puts("B done...\n");
        schedule();
    }
}
