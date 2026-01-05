.text
.global ctx_switch

ctx_switch:
    pushfl
    pushal
    movl 12(%esp), %eax      # &old_sp
    test %eax, %eax
    jz no_save
    movl %esp, (%eax)
no_save:
    movl 16(%esp), %eax      # &new_sp
    movl (%eax), %esp

    popal
    popfl
    ret
