#include "process.h"
#include "memory.h"
#include "serial.h"
#include "scheduler.h"

pcb_t proc_table[MAX_PROCESSES];
//pcb_t *current_proc = NULL;

/* ---------------- internal helpers ---------------- */

static int find_free_slot(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_table[i].state == PROC_UNUSED)
            return i;
    }
    return -1;
}

uint32_t *init_stack(void *stack_base, void (*entry)(void)) {
    uint32_t *sp = (uint32_t *)stack_base;

    serial_puts("[init_stack] stack_base = 0x");
    serial_puthex((uint32_t)stack_base);
    serial_puts("\n");
    serial_puts("[init_stack] entry = 0x");
    serial_puthex((uint32_t)entry);
    serial_puts("\n");

    // Push in the order they'll be popped (REVERSE order)
    *--sp = 0; /* EDI */
    *--sp = 0; /* ESI */
    *--sp = 0; /* EBP */
    *--sp = 0; /* ESP */
    *--sp = 0; /* EBX */
    *--sp = 0; /* EDX */
    *--sp = 0; /* ECX */
    *--sp = 0; /* EAX */
    *--sp = 0x202; /* EFLAGS */
    *--sp = (uint32_t)entry; /* Return address */

    serial_puts("[init_stack] final sp = 0x");
    serial_puthex((uint32_t)sp);
    serial_puts("\n");
    
    serial_puts("[init_stack] sp[0] = 0x");
    serial_puthex(sp[0]);
    serial_puts("\n");
    serial_puts("[init_stack] sp[9] = 0x");
    serial_puthex(sp[9]);
    serial_puts("\n");

    return sp;
}
/* ---------------- public API ---------------- */

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        proc_table[i].pid = i;
        proc_table[i].state = PROC_UNUSED;
        proc_table[i].msg_count = 0;
        proc_table[i].stack_base = 0;
        proc_table[i].stack_ptr = 0;
    }

    current_proc = 0;

    serial_puts("[proc] initialized\n");
}

int process_create(void (*entry)(void), uint32_t priority) {
    int slot = find_free_slot();
    if (slot < 0)
        return -1;

    void *stack = alloc_stack();
    if (!stack)
        return -1;

    pcb_t *p = &proc_table[slot];

    p->pid = slot;                 /* PID reuse */
    p->state = PROC_READY;
    p->priority = priority;
    p->age = 0;
    p->time_slice = 5;   
    p->time_left  = p->time_slice;

    p->stack_base = stack;
    p->stack_ptr  = init_stack(stack, entry);
    p->msg_count  = 0;

    serial_puts("[proc] created PID=");
    serial_putu(p->pid);
    serial_puts("\n");

    return p->pid;
}

void process_exit(void) {
    if (!current_proc)
        return;

    serial_puts("[proc] exit PID=");
    serial_putu(current_proc->pid);
    serial_puts("\n");

    current_proc->state = PROC_ZOMBIE;

    if (current_proc->stack_base)
        free_stack(current_proc->stack_base);

    current_proc = 0;

    schedule();   /* immediately switch */
}

void process_set_state(pid32 pid, proc_state_t state) {
    pcb_t *p = process_get(pid);
    if (p)
        p->state = state;
}

proc_state_t process_get_state(pid32 pid) {
    pcb_t *p = process_get(pid);
    if (!p)
        return PROC_UNUSED;
    return p->state;
}

pcb_t* process_get(pid32 pid) {
    if (pid < 0 || pid >= MAX_PROCESSES)
        return 0;

    if (proc_table[pid].state == PROC_UNUSED)
        return 0;

    return &proc_table[pid];
}

pid32 process_current_pid(void) {
    if (!current_proc)
        return -1;
    return current_proc->pid;
}

/* ---------------- cleanup ---------------- */

void process_reap(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (proc_table[i].state == PROC_ZOMBIE) {
            proc_table[i].state = PROC_UNUSED;
        }
    }
}

/* ---------------- IPC  ---------------- */

int process_send(pid32 dest_pid, uint32_t value) {
    if (!current_proc)
        return -1;

    pcb_t *dest = process_get(dest_pid);
    if (!dest || dest->state == PROC_ZOMBIE)
        return -1;

    if (dest->msg_count >= MAX_MESSAGES)
        return -1;

    dest->msg_queue[dest->msg_count].sender_pid = current_proc->pid;
    dest->msg_queue[dest->msg_count].value = value;
    dest->msg_count++;

    /* Wake up blocked receiver */
    if (dest->state == PROC_BLOCKED) {
        dest->state = PROC_READY;
    }

    serial_puts("[ipc] send ");
    serial_putu(current_proc->pid);
    serial_puts(" -> ");
    serial_putu(dest_pid);
    serial_puts("\n");

    return 0;
}

int process_receive(uint32_t *out_value) {
    if (!current_proc)
        return -1;

    /* No message → block */
    if (current_proc->msg_count == 0) {
        current_proc->state = PROC_BLOCKED;
        schedule();          

       
        if (current_proc->msg_count == 0)
            return -1;
    }

    *out_value = current_proc->msg_queue[0].value;

    /* Shift queue */
    for (uint32_t i = 1; i < current_proc->msg_count; i++)
        current_proc->msg_queue[i - 1] =
            current_proc->msg_queue[i];

    current_proc->msg_count--;

    serial_puts("[ipc] recv PID=");
    serial_putu(current_proc->pid);
    serial_puts("\n");

    return 0;
}

