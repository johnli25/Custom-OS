/* INTERRUPT_WRAPPER(name, handler, num)
*  DESCRIPTION: calls correct interrupt (keyboard or rtc)
 *  INPUTS: interrupt name,  handler function, interrupt number 
 *  OUTPUT: NONE
 *  RETURN: NONE
 *  SIDE AFFECTS: Allows for handler to be called (saves all registers)
 */
#define EXCEPTION_WRAPPER(name, handler, num) \
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

//etc.. fill in all the ones below in the same manner 
//write interrupt_handler itself (printf statement while loop) in lib.c or wherever
EXCEPTION_WRAPPER(DEFAULT_EXCEPTION, interrupt_handler, 0xFF);
EXCEPTION_WRAPPER(EXCEPTION_DIVIDE_BY_ZERO, interrupt_handler, 0x00);
EXCEPTION_WRAPPER(SINGLE_STEP_INTERRUPT, interrupt_handler, 0x01);
EXCEPTION_WRAPPER(EXCEPTION_NMI, interrupt_handler, 0x02);
EXCEPTION_WRAPPER(EXCEPTION_BREAKPOINT, interrupt_handler, 0x03);
EXCEPTION_WRAPPER(EXCEPTION_OVERFLOW, interrupt_handler, 0x04);
EXCEPTION_WRAPPER(EXCEPTION_BOUND_RANGE_EXCEEDED, interrupt_handler, 0x05);
EXCEPTION_WRAPPER(EXCEPTION_INVALID_OPCODE, interrupt_handler, 0x06);
EXCEPTION_WRAPPER(EXCEPTION_COPROCESSOR_NOT_AVAILABLE, interrupt_handler, 0x07);
EXCEPTION_WRAPPER(EXCEPTION_DOUBLE_FAULT, interrupt_handler, 0x08);
EXCEPTION_WRAPPER(EXCEPTION_COPROCESSOR_SEG_OVERRUN, interrupt_handler, 0x09);
EXCEPTION_WRAPPER(EXCEPTION_INVALID_TSS, interrupt_handler, 0x0A);
EXCEPTION_WRAPPER(EXCEPTION_SEGMENT_NOT_PRESENT, interrupt_handler, 0x0B);
EXCEPTION_WRAPPER(EXCEPTION_STACK_SEGMENT_FAULT, interrupt_handler, 0x0C);
EXCEPTION_WRAPPER(EXCEPTION_GENERAL_PROTECTION, interrupt_handler, 0x0D);
EXCEPTION_WRAPPER(EXCEPTION_PAGE_FAULT, interrupt_handler, 0x0E);
EXCEPTION_WRAPPER(EXCEPTION_RESERVED, interrupt_handler, 0x0F);
EXCEPTION_WRAPPER(EXCEPTION_FLOATING_POINT, interrupt_handler, 0x10);
EXCEPTION_WRAPPER(EXCEPTION_ALIGNMENT_CHECK, interrupt_handler, 0x11);
EXCEPTION_WRAPPER(EXCEPTION_MACHINE_CHECK, interrupt_handler, 0x12);
EXCEPTION_WRAPPER(EXCEPTION_SIMD_FLOATING_POINT, interrupt_handler, 0x13);
EXCEPTION_WRAPPER(EXCEPTION_VIRTUALIZATION, interrupt_handler, 0x14);
EXCEPTION_WRAPPER(EXCEPTION_CONTROL_PROTECTION, interrupt_handler, 0x15);

/*
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
*/


