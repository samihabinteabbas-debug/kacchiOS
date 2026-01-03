/* ctx_switch.S - x86 context switch */
.global ctx_switch
/* void ctx_switch(pcb_t *old, pcb_t *new); */
ctx_switch:
    pusha                      /* save registers (32 bytes) */
    
    movl 36(%esp), %eax        /* eax = old (4 + 32) */
    testl %eax, %eax           /* check if old is NULL */
    jz load_new                /* skip save if NULL */
    movl %esp, (%eax)          /* old->esp = esp */

load_new:
    movl 40(%esp), %eax        /* eax = new (8 + 32) */
    movl (%eax), %esp          /* esp = new->esp */
    
    popa                       /* restore registers */
    ret
