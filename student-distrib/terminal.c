#include "terminal.h"
#include "keyboard.h"
#include "lib.h"
#include "paging.h"

//HAVE TO ADD IN INTERFACES
void terminal_remap_mem(int oldTerminalNum, int newTerminalNum){
    memcpy((void *)paging_vidmem + oldTerminalNum * KB_4, (void *)paging_vidmem, KB_4); //first, save old/current program memory

    memcpy((void *)paging_vidmem, (void *)paging_vidmem + newTerminalNum * KB_4, KB_4); //then put new program memory into current
    // asm volatile ( //flush TLB
    //     "movl %%cr3, %%eax;"
    //     "movl %%eax, %%cr3;"  
    //     : 
    //     : 
    //     :"%eax" //saved "clobbered" regs 
    // );
}

void switch_terms(int terminalNum){
    
    multi_terms[currTerm].cursor_x = get_cursor_x();
    multi_terms[currTerm].cursor_y = get_cursor_y();

    set_cursor_x(multi_terms[terminalNum].cursor_x);
    set_cursor_y(multi_terms[terminalNum].cursor_y);
    
    terminal_remap_mem(currTerm, terminalNum);

    currTerm = terminalNum;

}

/* void terminal_init(void);
 * Inputs: none
 * Return Value: int
 * Function: initializes terminal (returning 0 for now) */
int32_t terminal_init(void){
    int i;
    currTerm = 0;
    for (i = 0; i < 3; i++){ //MAGIC NUM: MAX # of terms = 3
        multi_terms[i].cursor_x = 0;
        multi_terms[i].cursor_y = 0;
        multi_terms[i].curr_proc = NULL;
        multi_terms[i].bootup_flag = 0;
    }
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

    int p = 0; 
    for(p = 0; p < n; p++){
        if(p == NUM_COLS){
            newLine();
        }
        if(((unsigned char *)(buf))[p] != '\0'){ //checks if it is NULL
            if(((unsigned char *)(buf))[p] == '\n'){ //checks if it is NewLine
                newLine();
            }
            else if (((unsigned char *)(buf))[p] == '\t'){ //checks if it is Tab
                putc(' ');
                putc(' ');
                putc(' ');
                putc(' ');
                charsPrinted = charsPrinted + 4;
            }
            else if (((unsigned char *)(buf))[p] == BACKSPACEPRESS){ //checks if it is Backspace
                printf("bwuh \n"); 
                putBackspace(((unsigned char *)(buf))[p], (unsigned char *)buf);
                charsPrinted--;
            }
            else{
                putc(((unsigned char *)(buf))[p]);
                charsPrinted++;
            }
        }
    }
    if(charsPrinted > NUM_COLS){
        newLine();
    }
    return n;
}
