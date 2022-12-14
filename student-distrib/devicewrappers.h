#ifndef DEVICE_H
#define DEVICE_H
#include "systemCalls.h"

//used to call the assembly linkage for RTC
extern void RTC_WRAPPER(void);

//used to call the assembly linkage for Keyboard
extern void KEYBOARD_WRAPPER(void);

//used to call the assembly linkage for PIT
extern void PIT_WRAPPER(void);

extern void syscall_wrapper(void);

#endif


