/* ctxsw.S - ctxsw (for x86) */

.text
.globl ctx_switch
ctx_switch:

    pushl %ebp
    
    mov1 %esp, %ebp
    
    
    pushfl
    
    pushal
    
    movl 8(%ebp), %eax
    
    
    movl %esp, (%eax)
    
    movl 12(%ebp), %eax
    
    
    movl (%eax), %esp
    
    movl 4(%esp), %ebp
    
    popfl
    
    add $4, %esp
    
    ret
