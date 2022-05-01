#ifndef SCHEDULING
#define SCHEDULING_H

#include "idt.h"
#include "i8259.h"
#include "devicewrappers.h"
#include "x86_desc.h"


#define PIT_IRQ_NUM 0
#define IO_PORT_NUM 0x43

extern void initialize_PIT();


#endif


