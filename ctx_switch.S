.text
.global ctx_switch

ctx_switch:
    pushl %ebp
    movl %esp, %ebp
    
    pushfl
    pushal
    
    movl 8(%ebp), %eax       # old_sp (first param)
    test %eax, %eax
    jz no_save
    movl %esp, (%eax)
no_save:
    movl 12(%ebp), %eax      # new_sp (second param)
    movl (%eax), %esp
    
    popal
    popfl
    
    popl %ebp
    ret
