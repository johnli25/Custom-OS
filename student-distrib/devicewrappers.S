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
        pushal                    ;\
        pushfl                    ;\
        call handler                 ;\
        popfl                     ;\
        popal                     ;\
        iret

// device wrappers     
DEVICE_WRAPPER(RTC_WRAPPER, interrupt_RTC);

DEVICE_WRAPPER(KEYBOARD_WRAPPER, interrupt_keyboard);



