#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

/* Scheduler */
void schedule(void);

/* Low-level context switch (ASM) */
void ctx_switch(pcb_t *prev, pcb_t *next);

#endif
