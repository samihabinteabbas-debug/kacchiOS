#include "process.h"
#include "memory.h"
#include "serial.h"

pcb_t proc_table[MAX_PROCESSES];
pcb_t *current_proc = 0;

static uint32_t next_pid = 1;

/* ---------------- internal helpers ---------------- */

static int find_free_slot(void) {
    for (int i = 0; i < MAX_PROCESSES; i++)
        if (proc_table[i].state == PROC_UNUSED)
            return i;
    return -1;
}


static uint32_t* init_stack(void *stack_top, void (*entry)(void)) {
    uint32_t *sp = (uint32_t*)stack_top;
    
    serial_puts("[init_stack] stack_top=");
    serial_putu((uint32_t)stack_top);
    serial_puts(" entry=");
    serial_putu((uint32_t)entry);
    serial_puts("\n");
    
    /* Stack layout for ctx_switch to restore:
     * - Return address (where to jump)
     * - Saved ebp value (from "addl $4, %esp")
     * - Flags (for popfl)
     * - 8 general registers (for popal: edi, esi, ebp, esp, ebx, edx, ecx, eax)
     */
    
    *(--sp) = (uint32_t)entry;  /* Return address */
    *(--sp) = 0;                /* Saved ebp */
    *(--sp) = 0x200;            /* EFLAGS (interrupts enabled) */
    
    /* Push dummy values for popal (8 registers) */
    *(--sp) = 0;  /* EDI */
    *(--sp) = 0;  /* ESI */
    *(--sp) = 0;  /* EBP */
    *(--sp) = 0;  /* ESP (ignored by popal) */
    *(--sp) = 0;  /* EBX */
    *(--sp) = 0;  /* EDX */
    *(--sp) = 0;  /* ECX */
    *(--sp) = 0;  /* EAX */
    
    serial_puts("[init_stack] final sp=");
    serial_putu((uint32_t)sp);
    serial_puts("\n");
    
    return sp;
}
/* ---------------- public API ---------------- */

void process_init(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        proc_table[i].state = PROC_UNUSED;
        proc_table[i].pid = 0;
        proc_table[i].msg_count = 0;
    }

    current_proc = 0;
    next_pid = 1;

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

    p->pid = next_pid++;
    p->state = PROC_READY;
    p->priority = priority;
    p->age = 0;

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

    current_proc->state = PROC_TERMINATED;

    if (current_proc->stack_base)
        free_stack(current_proc->stack_base);

    current_proc = 0;
}

void process_set_state(int pid, proc_state_t state) {
    pcb_t *p = process_get(pid);
    if (p)
        p->state = state;
}

proc_state_t process_get_state(int pid) {
    pcb_t *p = process_get(pid);
    if (!p)
        return PROC_UNUSED;
    return p->state;
}

pcb_t* process_get(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++)
        if (proc_table[i].state != PROC_UNUSED &&
            proc_table[i].pid == pid)
            return &proc_table[i];
    return 0;
}

int process_current_pid(void) {
    if (!current_proc)
        return -1;
    return current_proc->pid;
}

/* ---------------- IPC ---------------- */

int process_send(int dest_pid, uint32_t value) {
    if (!current_proc)
        return -1;

    pcb_t *dest = process_get(dest_pid);
    if (!dest || dest->state == PROC_TERMINATED)
        return -1;

    if (dest->msg_count >= MAX_MESSAGES)
        return -1;

    dest->msg_queue[dest->msg_count].sender_pid = current_proc->pid;
    dest->msg_queue[dest->msg_count].value = value;
    dest->msg_count++;

    serial_puts("[ipc] send ");
    serial_putu(current_proc->pid);
    serial_puts(" -> ");
    serial_putu(dest_pid);
    serial_puts("\n");

    return 0;
}

int process_receive(uint32_t *out_value) {
    if (!current_proc || current_proc->msg_count == 0)
        return -1;

    *out_value = current_proc->msg_queue[0].value;

    for (uint32_t i = 1; i < current_proc->msg_count; i++)
        current_proc->msg_queue[i - 1] =
            current_proc->msg_queue[i];

    current_proc->msg_count--;

    serial_puts("[ipc] recv PID=");
    serial_putu(current_proc->pid);
    serial_puts("\n");

    return 0;
}
