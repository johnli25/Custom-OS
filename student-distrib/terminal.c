#include "terminal.h"
#include "keyboard.h"
#include "lib.h"


//HAVE TO ADD IN INTERFACES

/* void terminal_init(void);
 * Inputs: none
 * Return Value: int
 * Function: initializes terminal (returning 0 for now) */
int32_t terminal_init(void){
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
int32_t terminal_read(int32_t fd, unsigned char * buf, int32_t n){

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
int32_t terminal_write(int32_t fd, unsigned char * buf, int32_t n){
    //check if the keyboard buffer size is greater thatn n or not 

    
    int charsPrinted = 0; 

    if(n > (keyboardBufferSize-1)){ //if n is too big, then resize n
        n = (keyboardBufferSize-1);
    }

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
            else if (((unsigned char *)(buf))[p] == '\b'){ //checks if it is Backspace
                putBackspace(((unsigned char *)(buf))[p]);
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
