#include "scheduler.h"
#include "serial.h"

pcb_t *current_proc = NULL;

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
        return;
    
    /* aging */
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_table[i].state == PROC_READY)
            proc_table[i].age++;
    }
    
    pcb_t *prev = current_proc;
    
    if (prev == next) {
        return; 
    }
    
    if (prev && prev->state == PROC_RUNNING)
        prev->state = PROC_READY;
    
    next->state = PROC_RUNNING;
    next->age = 0;
    
    serial_puts("[sched] switch to PID=");
    serial_putu(next->pid);
    serial_puts("\n");
    
    current_proc = next;
    
    if (prev == NULL) {
        ctx_switch(NULL, &next->stack_ptr);
    } else {
        ctx_switch(&prev->stack_ptr, &next->stack_ptr);
    }
}
