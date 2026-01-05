.text
.global ctx_switch

ctx_switch:
    pushfl                   # Save EFLAGS (4 bytes)
    pushal                   # Save all GPRs (32 bytes)
    
    movl 44(%esp), %eax      # &old_sp (36 bytes of saved state + 8 bytes of args)
    test %eax, %eax
    jz no_save
    movl %esp, (%eax)        # Save current ESP
no_save:
    movl 48(%esp), %eax      # &new_sp
    movl (%eax), %esp        # Load new ESP

    popal                    # Restore registers
    popfl                    # Restore EFLAGS
    ret                      # Jump to return address on stack
