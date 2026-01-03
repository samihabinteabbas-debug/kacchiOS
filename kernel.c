/* kernel.c - Main kernel with null process */

#include "types.h"
#include "serial.h"
#include "string.h"
#include "process.h"
#include "scheduler.h"
#include "memory.h"

#define MAX_INPUT 128

void test_proc(void)
{
    while (1) {
        serial_puts("[test] running\n");
        for (volatile int i = 0; i < 10000000; i++);
        schedule();   /* explicitly yield */
    }
}

void kmain(void)
{
    char input[MAX_INPUT];
    int pos;

    /* Initialize system */
    serial_init();
    memory_init();
    process_init();

    /* Create one extra process to show rescheduling */
    process_create(test_proc, 1);

    /* Print banner ONCE */
    serial_puts("\n");
    serial_puts("========================================\n");
    serial_puts("    kacchiOS - Minimal Baremetal OS\n");
    serial_puts("========================================\n");
    serial_puts("Hello from kacchiOS!\n");
    serial_puts("Running null process...\n\n");

    /* Null process loop */
    while (1) {
        serial_puts("kacchiOS> ");
        pos = 0;

        /* Read input */
        while (1) {
            char c = serial_getc();

            if (c == '\r' || c == '\n') {
                input[pos] = '\0';
                serial_puts("\n");
                break;
            }
            else if ((c == '\b' || c == 0x7F) && pos > 0) {
                pos--;
                serial_puts("\b \b");
            }
            else if (c >= 32 && c < 127 && pos < MAX_INPUT - 1) {
                input[pos++] = c;
                serial_putc(c);
            }
        }

        if (pos > 0) {
            serial_puts("You typed: ");
            serial_puts(input);
            serial_puts("\n");
        }

        /* Yield CPU AFTER completing shell iteration */
        schedule();
    }
}
