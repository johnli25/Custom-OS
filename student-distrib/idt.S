/* INTERRUPT_WRAPPER(name, handler, num)
*  DESCRIPTION: calls correct interrupt (keyboard or rtc)
 *  INPUTS: interrupt name,  handler function, interrupt number 
 *  OUTPUT: NONE
 *  RETURN: NONE
 *  SIDE AFFECTS: Allows for handler to be called (saves all registers)
 */
#define INTERRUPT_WRAPPER(name, handler, num) \
    .globl name                   ;\
    name:                         ;\
        pushal                    ;\
        pushfl                    ;\
        pushl $num                    ;\
        call handler                 ;\
        popl $4, %esp
        popfl                     ;\
        popal                     ;\
        iret

// interrupt wrappers     
INTERRUPT_WRAPPER(DEFAULT_EXCEPTION, interrupt_handler, 0xFF);
INTERRUPT_WRAPPER(EXCEPTION_DIVIDE_BY_ZERO, interrupt_handler, 0x00);
//etc.. fill in all the ones below in the same manner 
//write interrupt_handler itself (printf statement while loop) in lib.c or wherever

extern void SINGLE_STEP_INTERRUPT(void);    //0x01
extern void EXCEPTION_NMI(void);            //0x02
extern void EXCEPTION_BREAKPOINT(void);     //0x03
extern void EXCEPTION_OVERFLOW(void);       //0x04
extern void EXCEPTION_BOUND_RANGE_EXCEEDED(void);   //0x05
extern void EXCEPTION_INVALID_OPCODE(void);         //0x06
extern void EXCEPTION_COPROCESSOR_NOT_AVAILABLE(void);   //0x07
extern void EXCEPTION_DOUBLE_FAULT(void);           //0x08
extern void EXCEPTION_COPROCESSOR_SEG_OVERRUN(void);//0x09
extern void EXCEPTION_INVALID_TSS(void);            //0x0A
extern void EXCEPTION_SEGMENT_NOT_PRESENT(void);    //0x0B
extern void EXCEPTION_STACK_SEGMENT_FAULT(void);    //0x0C
extern void EXCEPTION_GENERAL_PROTECTION(void);     //0x0D
extern void EXCEPTION_PAGE_FAULT(void);             //0x0E
extern void EXCEPTION_RESERVED(void);               //0x0F
extern void EXCEPTION_FLOATING_POINT(void);         //0x10
extern void EXCEPTION_ALIGNMENT_CHECK(void);        //0x11
extern void EXCEPTION_MACHINE_CHECK(void);          //0x12
extern void EXCEPTION_SIMD_FLOATING_POINT(void);   //0x13
extern void EXCEPTION_VIRTUALIZATION(void);   //0x14
extern void EXCEPTION_CONTROL_PROTECTION(void);   //0x15


