#include "terminal.h"
#include "keyboard.h"
#include "lib.h"
#include "paging.h"
#include "systemCalls.h"
#include "scheduling.h"


/* getCurrTerm()
 *   DESCRIPTION: gets current Terminal number
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: currTerm
 *   SIDE EFFECTS: none
 */
int getCurrTerm(){
    return currTerm;
}

/* getSchedTerm()
 *   DESCRIPTION: gets schedTerm number
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: schedTerm
 *   SIDE EFFECTS: none
 */
int getSchedTerm(){
    return schedTerm;
}

/* terminal_remap_mem(int oldTerminalNum, int newTerminalNum)
 *   DESCRIPTION: remaps memory with corresponding terminal
 *   INPUTS: int oldTerminalNum, int newTerminalNum
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void terminal_remap_mem(int oldTerminalNum, int newTerminalNum){
    memcpy((void *)paging_vidmem + (oldTerminalNum + 1) * KB_4, (void *)paging_vidmem, KB_4); //first, save old = current program memory

    memcpy((void *)paging_vidmem, (void *)paging_vidmem + (newTerminalNum + 1) * KB_4, KB_4); //then put new program memory into current
    asm volatile ( //flush TLB
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"  
        : 
        : 
        :"%eax" //saved "clobbered" regs 
    );
}

/* switch_terms(int terminalNum)
 *   DESCRIPTION: switches terminals
 *   INPUTS: int terminalNum
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: switches terminal
 */
void switch_terms(int terminalNum){
    //cli();
    terminal_remap_mem(currTerm, terminalNum);

    multi_terms[currTerm].cursor_x = get_screen_x(); //updates terminal cursor pos
    multi_terms[currTerm].cursor_y = get_screen_y(); // updates terminal cursor pos

    // screen_x = get_cursor_x();
    // screen_y = get_cursor_y();

    // set_screen_x(multi_terms[terminalNum].cursor_x);
    // set_screen_x(multi_terms[terminalNum].cursor_y);
    update_cursor(multi_terms[terminalNum].cursor_x, multi_terms[terminalNum].cursor_y);

    currTerm = terminalNum; //updating currTerm to the passed in terminal #

    //sti();
}

/* void terminal_init(void);
 * Inputs: none
 * Return Value: int
 * Function: initializes terminal (returning 0 for now) */
// int currTerm = 0;
// int schedTerm = 
int32_t terminal_init(void){
    int i;
    for (i = 0; i < 3; i++){ //MAGIC NUM: MAX # of terms = 3
        multi_terms[i].cursor_x = 0; //0 is the shell start 
        multi_terms[i].cursor_y = NUM_ROWS - 1;
        multi_terms[i].curr_proc = NULL;
        multi_terms[i].lastAssignedProcess = -1;

        multi_terms[i].bootup_flag = 0;
        //multi_terms[i].shell_cnt = 0;
        multi_terms[i].progRunning = 0;
        multi_terms[i].rtc_counter = 0;
        multi_terms[i].relative_frequency = 0;
        //rewrite_shell();
    }
    currTerm = 0;
    schedTerm = 0;
    term_shell_cnt = 0;
    return 0;
}

/* void terminal_open(void);
 * Inputs: none
 * Return Value: int
 * Function: opens the terminal (returning 0 for now) */
int32_t terminal_open(const uint8_t* filename){
    return 0;
}

/* void terminal_close(void);
 * Inputs: none
 * Return Value: int
 * Function: closes the terminal (returning 0 for now) */
int32_t terminal_close(int32_t fd){
    return -1;
}


//ADD IN NULL CHECKS

/* void terminal_read(int n, unsigned char * buf);
 * Inputs: int n, unsigned char * buf
 * Return Value: num bytes copied
 * Function: reads from keyboardBuffer to passed in buf */
int32_t terminal_read(int32_t fd, void * buf, int n){

    TERMINALFLAG = 0; //shared varable from the keyboard

    while(TERMINALFLAG == 0){

    }

    if(n > (keyboardBufferSize-1)){ //sets n to 127 if its too big
        n = (keyboardBufferSize-1);
    }

    unsigned char * myKeyboardBuffer = getKeyboardBuffer();

    //check if the keyboard buffer size is greater thatn n or not 

    buf = strncpyUnsignedChar(buf, myKeyboardBuffer, n); //copies the keyboard buffer to the new one

    // int p = 0;
    // for(p = 0; p < n; p++){
    //     if(buf[p] != '\0'){
    //         buf[p] = myKeyboardBuffer[p];
    //     }
    // }

    clearKeyboardBuffer(); //clears buffer 
    TERMINALFLAG = 0;
    return n;
}

/* void terminal_write(int n, unsigned char * buf);
 * Inputs: int n, unsigned char * buf
 * Return Value: num bytes copied
 * Function: writes from buf to screen */
int32_t terminal_write(int32_t fd, const void * buf, int n){
    //check if the keyboard buffer size is greater thatn n or not 
    //n = 198 is the number of chars that the frame0.txt file consists of
    int charsPrinted = 0; 

    // if(n > (keyboardBufferSize-1)){ //if n is too big, then resize n
    //     n = (keyboardBufferSize-1);
    // }

    int positiontest = 0;
    int p = 0; 
    for(p = 0; p < n; p++){
        positiontest = get_screen_x();
        if(positiontest == NUM_COLS - 1){
            if((((unsigned char *)(buf))[p-1]) != '|'){
                newLine();
            }
        }
        // if(p == NUM_COLS){
        //     newLine();
        // }

        if(((unsigned char *)(buf))[p] != '\0'){ //checks if it is NULL
            if(((unsigned char *)(buf))[p] == '\n'){ //checks if it is NewLine
                newLine();
            }
            else if (((unsigned char *)(buf))[p] == '\t'){ //checks if it is Tab
                // putc(' ');
                // putc(' ');
                // putc(' ');
                // putc(' ');
                putc('\t');
                charsPrinted = charsPrinted + 4;
            }
            else if (((unsigned char *)(buf))[p] == BACKSPACEPRESS){ //checks if it is Backspace
                putBackspace(((unsigned char *)(buf))[p], (unsigned char *)buf);
                charsPrinted--;
            }
            else{
                if (currTerm == schedTerm || term_shell_cnt <= 3){
                    putc(((unsigned char *)(buf))[p]);
                    charsPrinted++;
                }
                else if (currTerm != schedTerm && term_shell_cnt > 3){
                    putc_background(((unsigned char *)(buf))[p], currTerm, schedTerm);
                    charsPrinted++;
                }
            }
        }
    }
    // if(charsPrinted > NUM_COLS){
    //     newLine();
    // }
    return n;
}
