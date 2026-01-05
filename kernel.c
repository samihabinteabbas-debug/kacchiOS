/* kernel.c - Main kernel entry point */

#include "types.h"
#include "serial.h"
#include "string.h"
#include "process.h"
#include "scheduler.h"
#include "memory.h"
#include "processes.h"
void serial_puthex(uint32_t val) {
    char hex[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        serial_putc(hex[(val >> i) & 0xF]);
    }
}
/* -------------------------------------------------- */
/* Null / Idle process                                */
/* -------------------------------------------------- */
void null_process(void) {
    while (1) {
        /* Idle loop — give CPU to others */
        schedule();
    }
}

/* -------------------------------------------------- */
/* Kernel entry point                                 */
/* -------------------------------------------------- */
void kmain(void) {

    /* Initialize low-level subsystems */
    serial_init();
    memory_init();
    process_init();

    /* Print banner ONCE */
    serial_puts("\n");
    serial_puts("========================================\n");
    serial_puts("    kacchiOS - Minimal Baremetal OS\n");
    serial_puts("========================================\n");
    serial_puts("Kernel initialized.\n\n");

    /*
     * Create processes
     * IMPORTANT: null process must be created FIRST
     */
    serial_puts("Function addresses:\n");
serial_puts("  procA = 0x");
serial_puthex((uint32_t)procA);
serial_puts("\n");
    process_create(null_process, 0);   /* PID 0 : idle */
    process_create(procA, 3);           /* user process */
    // process_create(procB, 5);

    serial_puts("Starting scheduler...\n");

    /*
     * Start scheduling.
     * This call NEVER returns.
     */
    schedule();

    /* Safety net (should never reach here) */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
