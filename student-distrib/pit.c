#include "pit.h"
// #include "i8259.h"
#include "lib.h"
#include "scheduling.h"
#include "systemCalls.h"
#include "terminal.h"

/*Check this link:
https://wiki.osdev.org/Pit#Operating_Modes */
static int pit_count = 0;

void initialize_PIT(void){
    outb(CMD_BYTE, IO_PORT_NUM);             /* Set our command byte 0x36 */
    outb(PIT_FREQ & 0xFF, CHANNEL_0);   /* Set low byte of divisor */
    outb((PIT_FREQ /*& 0xFF00*/)>> 8, CHANNEL_0);     /* Set high byte of divisor */
    enable_irq(PIT_IRQ_NUM);
}   

void interrupt_PIT(void){
    send_eoi(PIT_IRQ_NUM);
    pit_count++;
    // schedTerm++;
    // schedTerm = schedTerm % 3;
    //int current_pid = getProgNum();
    term_shell_cnt++;
    switch (pit_count)
    {
    // case 0:
    //     if (multi_terms[1].bootup_flag == 0){
    //         //currTerm = 1;
    //         switch_terms(1);
    //         execute((uint8_t*)"shell");
    //         multi_terms[1].bootup_flag = 1;
    //     }
    //     break;
    case 1:
        if (multi_terms[1].bootup_flag == 0){
            //currTerm = 1;
            switch_terms(1);
            execute((uint8_t*)"shell");
            multi_terms[1].bootup_flag = 1;
        }
        break;
        //return;
    case 2:
        if (multi_terms[2].bootup_flag == 0){
            //currTerm = 2;          
            switch_terms(2);
            execute((uint8_t*)"shell");

            /*context switch for terminal switch*/
            // int currProgram = getProgNum();

            // pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currProgram + 1))); //save current process PCB
            // multi_terms[currTerm].curr_proc = mypcb;

            // pcb_t * nextpcb = multi_terms[2].curr_proc;

            // multi_terms[currTerm].curr_proc = mypcb;
            // contextSwitch(mypcb, nextpcb);
            /*end of context switch for terminals*/
            multi_terms[2].bootup_flag = 1;
        }
        break;
        //return;
    case 3:
        if (multi_terms[0].bootup_flag == 0){
            //currTerm = 0;
            switch_terms(0);
            execute((uint8_t*)"shell");
            multi_terms[0].bootup_flag = 1;
        }
        break;
        //return;
    case 4: //swap term0 and term2 that were swapped upon initialization

        if(1){
            pcb_t * pcb0 = multi_terms[0].curr_proc;
            pcb_t * pcb2 = multi_terms[2].curr_proc;
            int shell0 = multi_terms[0].lastAssignedProcess;
            int shell2 = multi_terms[2].lastAssignedProcess;

            int rtc0 = multi_terms[0].rtc_counter;
            int rtc2 = multi_terms[2].rtc_counter;

            multi_terms[0].curr_proc = pcb2;
            multi_terms[0].lastAssignedProcess = shell2;
            multi_terms[0].rtc_counter = rtc2;

            multi_terms[2].curr_proc = pcb0;
            multi_terms[2].lastAssignedProcess = shell0;
            multi_terms[2].rtc_counter = rtc0;
        }

        break;       
    default:
        //return;
        break;
    }

    // if (multi_terms[schedTerm].progRunning == 1)
    scheduler();

}



