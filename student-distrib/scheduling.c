#include "scheduling.h"
#include "pit.h"
#include "x86_desc.h"
#include "terminal.h"
#include "filesys.h"
#include "systemCalls.h"

void scheduler(){
    //int cur_process_id = getProgNum();
    int schedTermTemp = schedTerm;

    // pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (cur_process_id + 1))); //1st program PCB
    //pcb_t * mypcb = multi_terms[currTerm].curr_proc;
    
    pcb_t * mypcb = multi_terms[schedTermTemp].curr_proc; //current pcb (will be saved)
    schedTerm++;
    if(!(multi_terms[schedTerm].curr_proc)){
        if (schedTerm == 3)
            schedTerm = 1;
        schedTerm--;
        return;
    }    
    schedTerm = schedTerm % 3;
    pcb_t * nextpcb = multi_terms[schedTerm].curr_proc; //next pcb (will be next-load it)

    asm volatile( //save ebp and esp of scheduled terminal
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(mypcb->saved_esp), "=r"(mypcb->saved_ebp)
    );

    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHTMB - (EIGHTKB * (nextpcb->pid /*+ 1*/))) - 4; // magic -4: used to get the correct esp calculation

    vid_paging_helper();
    paging_helper(nextpcb->pid); //paging mapper-helper for next/scheduled terminal process

    asm volatile( //save ebp and esp of scheduled terminal
        "movl %0, %%esp;" //esp contains saved_esp
        "movl %1, %%ebp;" //ebp contains saved_ebp
        :
        : "r"(nextpcb->saved_esp), "r"(nextpcb->saved_ebp)
        :"esp", "ebp"   //clobbers esp, ebp
    );   

}





