#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAX_PROCESSES 16
#define MAX_MESSAGES  8
#define STACK_SIZE    4096

typedef int32_t pid32;

/* Process states */
typedef enum {
    PROC_UNUSED = 0,
    PROC_READY,
    PROC_RUNNING,
    PROC_TERMINATED
} proc_state_t;

/* IPC message */
typedef struct {
    pid32 sender_pid;
    uint32_t value;
} message_t;

/* Process Control Block */
typedef struct pcb {
    pid32 pid;
    proc_state_t state;
    uint32_t priority;
    uint32_t age;

    void *stack_base;
    void *stack_ptr;

    message_t msg_queue[MAX_MESSAGES];
    uint32_t msg_count;
} pcb_t;

/* Globals */
extern pcb_t proc_table[MAX_PROCESSES];
extern pcb_t *current_proc;

/* Process management */
void process_init(void);
int  process_create(void (*entry)(void), uint32_t priority);
void process_exit(void);

void process_set_state(int pid, proc_state_t state);
proc_state_t process_get_state(int pid);

pcb_t* process_get(int pid);
int process_current_pid(void);

/* IPC */
int process_send(int dest_pid, uint32_t value);
int process_receive(uint32_t *out_value);

#endif
