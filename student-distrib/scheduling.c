#include "scheduling.h"
#include "pit.h"

void scheduler(){
    int cur_process_id = getProgNum();
    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (cur_process_id + 1))); //1st program PCB


    
}





