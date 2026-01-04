#include "scheduler.h"
#include "serial.h"

static pcb_t* select_next(void) {
    pcb_t *best = 0;

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
        return;

    /* Aging for READY processes */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_table[i].state == PROC_READY)
            proc_table[i].age++;
    }

    /* If current process still has time, keep running */
    if (current_proc &&
        current_proc->state == PROC_RUNNING &&
        current_proc->time_left > 0) {

        current_proc->time_left--;
        return;
    }

    pcb_t *prev = current_proc;

    if (prev && prev->state == PROC_RUNNING) {
        prev->state = PROC_READY;
        prev->time_left = prev->time_slice;
    }

    next->state = PROC_RUNNING;
    next->age = 0;
    next->time_left = next->time_slice;

    serial_puts("[sched] switch to PID=");
    serial_putu(next->pid);
    serial_puts("\n");

    current_proc = next;

    if (prev) {
        ctx_switch(&prev->stack_ptr, &next->stack_ptr);
    } else {
        uint32_t *dummy = 0;
        ctx_switch(&dummy, &next->stack_ptr);
    }
}

