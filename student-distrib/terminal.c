#include "terminal.h"
#include "keyboard.h"
#include "lib.h"


//HAVE TO ADD IN INTERFACES

/* void terminal_init(void);
 * Inputs: none
 * Return Value: int
 * Function: initializes terminal (returning 0 for now) */
int terminal_init(void){
    return 0;
}

/* void terminal_open(void);
 * Inputs: none
 * Return Value: int
 * Function: opens the terminal (returning 0 for now) */
int terminal_open(void){
    return 0;
}

/* void terminal_close(void);
 * Inputs: none
 * Return Value: int
 * Function: closes the terminal (returning 0 for now) */
int terminal_close(void){
    return 0;
}


//ADD IN NULL CHECKS

/* void terminal_read(int n, unsigned char * buf);
 * Inputs: int n, unsigned char * buf
 * Return Value: num bytes copied
 * Function: reads from keyboardBuffer to passed in buf */
int terminal_read(int n, unsigned char * buf){

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
int terminal_write(int n, unsigned char * buf){
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
        if(buf[p] != '\0'){ //checks if it is NULL
            if(buf[p] == '\n'){ //checks if it is NewLine
                newLine();
            }
            else if (buf[p] == '\t'){ //checks if it is Tab
                putc(' ');
                putc(' ');
                putc(' ');
                putc(' ');
                charsPrinted = charsPrinted + 4;
            }
            else if (buf[p] == '\b'){ //checks if it is Backspace
                putBackspace(buf[p]);
                charsPrinted--;
            }
            else{
                putc(buf[p]);
                charsPrinted++;
            }
        }
        
    }
    if(charsPrinted > NUM_COLS){
        newLine();
    }
    return n;

}
