/* ctxsw.S - ctxsw (for x86) 

.text
.globl ctx_switch
ctx_switch:

    pushl %ebp
    
    movl %esp, %ebp
    
    
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
*/
.text
.global ctx_switch

ctx_switch:
    pushl %ebp              # Save old frame pointer
    movl %esp, %ebp         # Set up new frame pointer
    pushfl                  # Save flags
    pushal                  # Save all general registers
    
    movl 8(%ebp), %eax      # Get &old_sp (first argument)
    movl %esp, (%eax)       # Save current ESP to *old_sp
    
    movl 12(%ebp), %eax     # Get &new_sp (second argument)
    movl (%eax), %esp       # Load new ESP from *new_sp
    
    popal                   # Restore general registers
    movl 4(%esp), %ebp      # Restore ebp from stack
    popfl                   # Restore flags
    addl $4, %esp           # Skip saved ebp value
    ret                     # Return to new process
