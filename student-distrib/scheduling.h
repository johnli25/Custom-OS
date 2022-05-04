#ifndef SCHEDULING
#define SCHEDULING_H
#include "filesys.h"
#include "systemCalls.h"
//#include "pit.h"

extern void contextSwitch(pcb_t * mypcb, pcb_t * nextpcb);
extern void scheduler(int pit_count);

#endif


