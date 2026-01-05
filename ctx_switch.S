.text
.global ctx_switch

# void ctx_switch(uint32_t **old_sp, uint32_t **new_sp)
ctx_switch:
    # Save all registers and flags
    pushfl
    pushal
    
    # Get the parameters from stack
    # After pushfl (4 bytes) and pushal (32 bytes), we have 36 bytes pushed
    # Parameters are at: ESP+40 and ESP+44
    
    movl 40(%esp), %eax      # Get old_sp
    test %eax, %eax          # Check if NULL
    jz load_new              # Skip save if NULL
    movl %esp, (%eax)        # Save ESP to *old_sp
    
load_new:
    movl 44(%esp), %eax      # Get new_sp
    movl (%eax), %esp        # Load ESP from *new_sp
    
    # Restore registers and return
    popal
    popfl
    ret
