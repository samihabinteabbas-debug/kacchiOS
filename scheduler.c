#include "scheduler.h"
#include "serial.h"

pcb_t *current_proc = NULL;
void serial_puthex(uint32_t val) {
    char hex[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        serial_putc(hex[(val >> i) & 0xF]);
    }
}
static pcb_t* select_next(void) {
    pcb_t *best = NULL;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        pcb_t *p = &proc_table[i];
        if (p->state == PROC_READY) {
            if (!best ||
                (p->priority + p->age) >
                (best->priority + best->age)) {
                best = p;
            }
        }
    }
    return best;
}
void schedule(void) {
    pcb_t *next = select_next();
    if (!next)
        return;  // No ready process

    /* aging */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_table[i].state == PROC_READY)
            proc_table[i].age++;
    }

    pcb_t *prev = current_proc;

    if (prev && prev->state == PROC_RUNNING)
        prev->state = PROC_READY;

    next->state = PROC_RUNNING;
    next->age = 0;

    serial_puts("[sched] switch to PID=");
    serial_putu(next->pid);
    serial_puts("\n");

    current_proc = next;
serial_puts("[sched] next->stack_ptr = 0x");
serial_puthex((uint32_t)next->stack_ptr);
serial_puts("\n");
    serial_puts("[sched] About to context switch\n");
serial_puts("[sched] prev = 0x");
serial_puthex((uint32_t)prev);
serial_puts("\n");
serial_puts("[sched] next = 0x");
serial_puthex((uint32_t)next);
serial_puts("\n");
serial_puts("[sched] next->stack_ptr = 0x");
serial_puthex((uint32_t)next->stack_ptr);
serial_puts("\n");

// Print what's at the top of the new stack
uint32_t *sp = next->stack_ptr;
for (int i = 0; i < 11; i++) {
    serial_puts("[sched] sp[");
    serial_putu(i);
    serial_puts("] = 0x");
    serial_puthex(sp[i]);
    serial_puts("\n");
}
    if (prev == NULL) {
        ctx_switch(NULL, &next->stack_ptr);
    } else {
        ctx_switch(&prev->stack_ptr, &next->stack_ptr);
    }
}
