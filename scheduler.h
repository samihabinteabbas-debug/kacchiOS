#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

/* Scheduler */
void schedule(void);


void ctx_switch(uint32_t **old_sp, uint32_t **new_sp);
#endif
