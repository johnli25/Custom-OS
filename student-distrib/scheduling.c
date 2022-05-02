#include "scheduling.h"
#include "pit.h"
#include "x86_desc.h"
#include "terminal.h"
#include "filesys.h"
#include "systemCalls.h"

/* contextSwitch(pcb_t * mypcb, pcb_t * nextpcb)
 * Description: context switch with mypcb and nextpcb
 * Inputs: pcb_t * mypcb, pcb_t * nextpcb
 * Outputs: None
 * Return Value: None
 * Side Effects: responsible for context switching 
*/ 
void contextSwitch(pcb_t * mypcb, pcb_t * nextpcb){
    asm volatile( //save ebp and esp of scheduled terminal
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(mypcb->saved_esp), "=r"(mypcb->saved_ebp)
    );

    vid_paging_helper();
    paging_helper(nextpcb->pid); //paging mapper-helper for next/scheduled terminal process

    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHTMB - (EIGHTKB * (nextpcb->pid /*+ 1*/))) - 4; // magic -4: used to get the correct esp calculation

    asm volatile( //taking esp ebp of nextpcb and storing into respective esp ebp registers
        "movl %0, %%esp;" //esp contains saved_esp
        "movl %1, %%ebp;" //ebp contains saved_ebp
        :
        : "r"(nextpcb->saved_esp), "r"(nextpcb->saved_ebp)
        :"esp", "ebp"   //clobbers esp, ebp
    );

}
/* scheduler()
 * Description: handles scheduling implementation
 * Inputs: none
 * Outputs: None
 * Return Value: None
 * Side Effects: calls contextSwitch function
*/ 
void scheduler(){
    //int cur_process_id = getProgNum();
    int schedTermTemp = schedTerm;
    
    pcb_t * mypcb = multi_terms[schedTermTemp].curr_proc; //current pcb (will be saved)
    
    schedTerm++;
    schedTerm = schedTerm % 3;
    // if (multi_terms[currTerm].progRunning == 1 && multi_terms[schedTerm].progRunning != 1)
    //     currTerm = currTerm; 
    if (multi_terms[schedTermTemp].progRunning != 1)// == 0
        return; 
    if (multi_terms[schedTerm].progRunning != 1) //== 0
        return; 

    if(!(multi_terms[schedTerm].curr_proc)){ //if scheduled terminal process == null
        //schedTerm = schedTermTemp; //set schedTerm back to original one before
        return;
    }    

    pcb_t * nextpcb = multi_terms[schedTerm].curr_proc; //next pcb (will be next-load it)

    contextSwitch(mypcb, nextpcb);

}





