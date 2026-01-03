.global ctx_switch
.type ctx_switch, %function

/* void ctx_switch(pcb_t *prev, pcb_t *next) */
/* r0 = prev, r1 = next */

ctx_switch:
    /* Save registers of current process */
    push {r4-r11, lr}

    /* Save SP into prev->stack_ptr */
    str sp, [r0, #16]   /* offset of stack_ptr in pcb_t */

    /* Load SP from next->stack_ptr */
    ldr sp, [r1, #16]

    /* Restore registers of next process */
    pop {r4-r11, lr}

    bx lr
