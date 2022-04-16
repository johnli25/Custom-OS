#include "idt.h"
#include "lib.h"
#include "x86_desc.h"
#include "rtc.h"
#include "keyboard.h"
#include "devicewrappers.h"
#include "systemCalls.h"

/*initialize_idt()
*Description: follows x86 standard OP and fills the IDT 
*Inputs: void
*Outputs: void
*Return Value: void
*Side Effects: initializes the IDT 
*/
void initialize_idt(void){
    int i; 
    for (i=0; i<NUM_VEC; i++){
        //set reserved and present bits
        /*MAGIC #s: reserved bits sequence for idt = 0111...*/
        idt[i].reserved0 = 0;
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].present = 1; 

        //set segment selector 
        idt[i].seg_selector = KERNEL_CS;

        // set size = 1 bc/ 32 bit
        idt[i].size = 1;

        //MAGIC #: reserved bits if 0x20 <= i <= 0x2F, which are the interrupt vector numbers (MAGIC #s)
        if ((i >= 0x20) && (i <= 0x2F)){
            idt[i].reserved3= 0;
        }
        else idt[i].reserved3 = 1;

        // if i==0x80 set descriptor privelege level to 3, else 0 
        if (i== 0x80) //0x80 is sys call number
            idt[i].dpl = 0x03; //magic #: highest privilege #
        else 
            idt[i].dpl = 0x00; //magic #: lowest privilege #

        //finally populate IDT w default entries
        SET_IDT_ENTRY(idt[i], DEFAULT_EXCEPTION);
    }

    //fill the specific entries we care about (taken from x86 standards)
    //MAGIC #s 0x0-0x15, 0x21, 0x28, 0x80 are defined by the IDT vector numbers!
    SET_IDT_ENTRY(idt[0x00], EXCEPTION_DIVIDE_BY_ZERO);
    SET_IDT_ENTRY(idt[0x01], SINGLE_STEP_INTERRUPT);
    SET_IDT_ENTRY(idt[0x02], EXCEPTION_NMI);
    SET_IDT_ENTRY(idt[0x03], EXCEPTION_BREAKPOINT);
    SET_IDT_ENTRY(idt[0x04], EXCEPTION_OVERFLOW);
    SET_IDT_ENTRY(idt[0x05], EXCEPTION_BOUND_RANGE_EXCEEDED);
    SET_IDT_ENTRY(idt[0x06], EXCEPTION_INVALID_OPCODE);
    SET_IDT_ENTRY(idt[0x07], EXCEPTION_COPROCESSOR_NOT_AVAILABLE);
    SET_IDT_ENTRY(idt[0x08], EXCEPTION_DOUBLE_FAULT);
    SET_IDT_ENTRY(idt[0x09], EXCEPTION_COPROCESSOR_SEG_OVERRUN);
    SET_IDT_ENTRY(idt[0x0A], EXCEPTION_INVALID_TSS);
    SET_IDT_ENTRY(idt[0x0B], EXCEPTION_SEGMENT_NOT_PRESENT);
    SET_IDT_ENTRY(idt[0x0C], EXCEPTION_STACK_SEGMENT_FAULT);
    SET_IDT_ENTRY(idt[0x0D], EXCEPTION_GENERAL_PROTECTION);
    SET_IDT_ENTRY(idt[0x0E], EXCEPTION_PAGE_FAULT);
    SET_IDT_ENTRY(idt[0x0F], EXCEPTION_RESERVED);
    SET_IDT_ENTRY(idt[0x10], EXCEPTION_FLOATING_POINT);
    SET_IDT_ENTRY(idt[0x11], EXCEPTION_ALIGNMENT_CHECK);
    SET_IDT_ENTRY(idt[0x12], EXCEPTION_MACHINE_CHECK);
    SET_IDT_ENTRY(idt[0x13], EXCEPTION_SIMD_FLOATING_POINT);
    SET_IDT_ENTRY(idt[0x14], EXCEPTION_VIRTUALIZATION);
    SET_IDT_ENTRY(idt[0x15], EXCEPTION_CONTROL_PROTECTION);

    SET_IDT_ENTRY(idt[0x80], syscall_wrapper);

    SET_IDT_ENTRY(idt[0x21], KEYBOARD_WRAPPER); 

    SET_IDT_ENTRY(idt[0x28], RTC_WRAPPER);

    lidt(idt_desc_ptr); //specify size of IDT and set base address 
}

/*
void NAME_OF_EXCEPTION_FUNCTION()
Description: Exception handler functions
Inputs: void
outputs: void
side effects: prints to kernel and then freezes it by infinite looping
*/
void DEFAULT_EXCEPTION(void){ 
    printf("Default exception! ");
    // while(1){

    // } //infinite while loop

    return;
}

void EXCEPTION_DIVIDE_BY_ZERO(void){ //0x0
    printf("Divide by 0 exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}

void SINGLE_STEP_INTERRUPT(void){//0x1
    printf("Single step interrupt exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}

void EXCEPTION_NMI(void){//0x2
    printf("NMI exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_BREAKPOINT(void){//0x3
    printf("Breakpoint exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_OVERFLOW(void){//0x4
    printf("Overflow exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}

void EXCEPTION_BOUND_RANGE_EXCEEDED(void){//0x5
    printf("Bound Range exceeded exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_INVALID_OPCODE(void){//0x6
    printf("Invalid Opcode exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_COPROCESSOR_NOT_AVAILABLE(void){ //0x7
    printf("Coprocessor not available exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_DOUBLE_FAULT(void){ //0x8
    printf("Double fault exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_COPROCESSOR_SEG_OVERRUN(void){ //0x9
    printf("Coprocessor Seg Overrun exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_INVALID_TSS(void){ //0xA
    printf("Invalid TSS exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_SEGMENT_NOT_PRESENT(void){ //0xB
    printf("Segment not present exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_STACK_SEGMENT_FAULT(void){ //0xC
    printf("Stack Segment fault exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}

void EXCEPTION_GENERAL_PROTECTION(void){ //0xD
    printf("General Protection exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions

    return;
}
void EXCEPTION_PAGE_FAULT(void){ //0xE
    printf("Page Fault exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_RESERVED(void){ //0xF
    printf("Reserved exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_FLOATING_POINT(void){ //0x10
    printf("Floating Point exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_ALIGNMENT_CHECK(void){ //0x11
    printf("Alignment Check exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_MACHINE_CHECK(void){ //0x12
    printf("Machine Check exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_SIMD_FLOATING_POINT(void){ //0x13
    printf("SIMD Floating Point exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return; 
}
void EXCEPTION_VIRTUALIZATION(void){ //0x14
    printf("Virtualization exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
void EXCEPTION_CONTROL_PROTECTION(void){ //0x15
    printf("Control Protection exception! ");
    // while(1) {

    // } //infinite while loop
    halt(255); //MAGIC #: 256 is return # for exceptions
    return;
}
// void SYS_CALL(void){ //0x80
//     printf("System Call! \n");
//     while(1) {

//     } //infinite while loop
//     return;
// }


