/* ctx_switch.S - x86 context switch */
.global ctx_switch

/* void ctx_switch(uint32_t **old_sp, uint32_t *new_sp); */
ctx_switch:
    pusha                   /* save registers */

    movl 8(%esp), %eax      /* eax = old_sp */
    movl %esp, (%eax)       /* *old_sp = esp */

    movl 12(%esp), %esp     /* esp = new_sp */

    popa                    /* restore registers */
    ret
