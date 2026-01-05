.text
.global ctx_switch

ctx_switch:
    pushfl                   # Save EFLAGS
    pushal                   # Save all registers
    
    movl 40(%esp), %eax      # Get &old_sp (first parameter)
    test %eax, %eax
    jz no_save
    movl %esp, (%eax)        # Save current stack pointer
no_save:
    movl 44(%esp), %eax      # Get &new_sp (second parameter)
    movl (%eax), %esp        # Load new stack pointer

    popal                    # Restore registers
    popfl                    # Restore EFLAGS
    ret                      # Return to saved address
