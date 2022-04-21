#ifndef SCHEDULING
#define SCHEDULING_H

#include "idt.h"
#include "i8259.h"
#include "deviceWrappers.h"
#include "x86_desc.h"


#define PIT_IRQ_NUM 0

extern void initialize_PIT();


#endif


