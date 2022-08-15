/* DEVICE_WRAPPER(name, handler)
*  DESCRIPTION: calls correct interrupt (keyboard or rtc)
 *  INPUTS: handler name,  handler function
 *  OUTPUT: NONE
 *  RETURN: NONE
 *  SIDE AFFECTS: Allows for handler to be called (saves all registers)
 */

#define DEVICE_WRAPPER(name, handler) \
    .globl name                   ;\
    name:                         ;\
        CLI                       ;\
        pushal                    ;\
        pushfl                    ;\
        call handler                ;\
        popfl                     ;\
        popal                     ;\
        STI                        ;\
        iret                        

// device wrappers     
DEVICE_WRAPPER(RTC_WRAPPER, interrupt_RTC);
DEVICE_WRAPPER(KEYBOARD_WRAPPER, interrupt_keyboard);
DEVICE_WRAPPER(PIT_WRAPPER, interrupt_PIT)
.globl syscall_wrapper

syscall_wrapper:
    # sti?
    cmpl $0, %eax
    jle syscall_invalid
    cmpl $11, %eax
    jge syscall_invalid 
    pushfl
    # pushal 
    pushl %esi
    pushl %edi
    pushl %ebx
    # cmpl $0, %eax
    # jle syscall_invalid
    # cmpl $11, %eax
    # jge syscall_invalid 
    pushl %edx # third arg
    pushl %ecx # second arg
    pushl %ebx # first arg

    call *syscall_table(, %eax, 4)
    jmp syscall_done

syscall_invalid:
    movl  $-1, %eax
    sti
    iret
syscall_done:
    popl %ebx
    popl %ecx
    popl %edx 
    # popal 
    popl %ebx
    popl %edi
    popl %esi
    popfl
    # sti
    iret
syscall_table:
    .long 0x0, halt, execute, general_read, general_write, general_open, general_close, getargs, vidmap, set_handler, sigreturn 


    



