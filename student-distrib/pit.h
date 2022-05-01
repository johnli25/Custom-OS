#ifndef SCHEDULING
#define SCHEDULING_H

#include "idt.h"
#include "i8259.h"
#include "devicewrappers.h"
#include "x86_desc.h"
#include "systemCalls.h"

#define PIT_IRQ_NUM 0
#define IO_PORT_NUM 0x43
#define CHANNEL_0 0x40
#define CMD_BYTE 0x36 //mode 3
#define PIT_FREQ 11932 //regular PIT freq is 1.193182 MHz = 1,193,182 cycles/sec. 
//to get PIT_FREQ to operate btwn 10 ms and 50 ms, divide by 100 => 11931.82 cycles/10 ms. For ease, round to 11932 Hz

extern void initialize_PIT(void);
extern void interrupt_PIT(void);


#endif


