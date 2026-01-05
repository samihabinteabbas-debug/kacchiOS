/* kernel.c - Main kernel entry point */

#include "types.h"
#include "serial.h"
#include "string.h"
#include "process.h"
#include "scheduler.h"
#include "memory.h"
#include "processes.h"

/* External process declarations */
extern void procA(void);
extern void procB(void);
extern void procC(void);
extern void procD(void);

/* -------------------------------------------------- */
/* Null / Idle process                                */
/* -------------------------------------------------- */
void null_process(void) {
    int idle_count = 0;
    while (1) {
        /* Check if all other processes are done */
        int active_processes = 0;
        for (int i = 1; i < MAX_PROCESSES; i++) {
            pcb_t *p = process_get(i);
            if (p && (p->state == PROC_READY || p->state == PROC_RUNNING || p->state == PROC_BLOCKED)) {
                active_processes++;
            }
        }
        
        /* If no active processes, we're done */
        if (active_processes == 0) {
            serial_puts("\n========================================\n");
            serial_puts("  All processes completed!\n");
            serial_puts("========================================\n");
            serial_puts("\nkacchiOS demonstration finished.\n");
            serial_puts("System halting.\n\n");
               __asm__ volatile ("cli; hlt");
    while (1) { __asm__ volatile ("hlt"); }
        }
        
        /* Idle loop - print status occasionally */
        idle_count++;
        if (idle_count % 10 == 0) {
            // Occasional status update
        }
        
        /* Small delay and yield */
        for (volatile int i = 0; i < 100000; i++);
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

    /* Print banner */
    serial_puts("\n");
    serial_puts("========================================\n");
    serial_puts("    kacchiOS - Educational OS Demo\n");
    serial_puts("========================================\n");
    serial_puts("\nDemonstrating:\n");
    serial_puts("  - Process Management\n");
    serial_puts("  - Context Switching\n");
    serial_puts("  - Priority Scheduling\n");
    serial_puts("  - Inter-Process Communication (IPC)\n");
    serial_puts("  - Memory Management\n");
    serial_puts("\n========================================\n");
    serial_puts("\nStarting demonstration...\n");

    /*
     * Create processes with different priorities
     * Lower priority number = lower priority
     * Higher priority number = higher priority
     */
    process_create(null_process, 0);   /* PID 0: Idle process */
    process_create(procA, 3);           /* PID 1: Producer (medium priority) */
    process_create(procB, 5);           /* PID 2: Consumer (high priority) */
    process_create(procC, 2);           /* PID 3: Memory tester (low priority) */
    process_create(procD, 4);           /* PID 4: Status reporter (medium-high priority) */

    serial_puts("\nProcesses created:\n");
    serial_puts("  PID 0: Null process (Priority 0)\n");
    serial_puts("  PID 1: Producer     (Priority 3)\n");
    serial_puts("  PID 2: Consumer     (Priority 5)\n");
    serial_puts("  PID 3: Memory Test  (Priority 2)\n");
    serial_puts("  PID 4: Status       (Priority 4)\n");
    serial_puts("\n========================================\n");
    
    serial_puts("\nStarting scheduler...\n\n");

    /*
     * Start scheduling.
     * This call NEVER returns.
     */
    schedule();

    /* Safety net (should never reach here) */
    serial_puts("ERROR: schedule() returned!\n");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
