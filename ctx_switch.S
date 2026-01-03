/* ctx_switch.S - x86 context switch */
.global ctx_switch
/* void ctx_switch(pcb_t *old, pcb_t *new); */
ctx_switch:
    pusha                      /* save registers (32 bytes) */
    
    movl 36(%esp), %eax        /* eax = old (4 + 32) */
    testl %eax, %eax           /* check if old is NULL */
    jz load_new                /* skip save if NULL */
    movl %esp, 20(%eax)        /* old->stack_ptr = esp (offset 20) */

load_new:
    movl 40(%esp), %eax        /* eax = new (8 + 32) */
    movl 20(%eax), %esp        /* esp = new->stack_ptr (offset 20) */
    
    popa                       /* restore registers */
    ret
