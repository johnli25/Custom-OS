#include "terminal.h"
#include "keyboard.h"
#include "lib.h"


//HAVE TO ADD IN INTERFACES

int terminal_init(void){
    return 0;
}

int terminal_open(void){
    return 0;
}
int terminal_close(void){
    return 0;
}


//ADD IN NULL CHECKS 

int terminal_read(int n, unsigned char * buf){

    if(TERMINALFLAG == 1){

        if(n > 127){
            n = 127;
        }


        unsigned char * myKeyboardBuffer = getKeyboardBuffer();

        //check if the keyboard buffer size is greater thatn n or not 

        // buf = strncpy(buf, myKeyboardBuffer, n);

        int p = 0;
        for(p = 0; p < n; p++){
            if(buf[p] != '/0'){
                buf[p] = myKeyboardBuffer[p];
            }
        }
        clearKeyboardBuffer();
        TERMINALFLAG = 0;
        return n;
    }
}

int terminal_write(int n, unsigned char * buf){
    //check if the keyboard buffer size is greater thatn n or not 

    if(n > 127){
        n = 127;
    }

    int p = 0; 
    for(p = 0; p < n; p++){
        if(buf[p] != '/0'){
            if(buf[p] == '\n'){
                newLine();
            }
            else if (buf[p] == '\t'){
                putc(' ');
                putc(' ');
                putc(' ');
                putc(' ');
            }
            else if (buf[p] == '\b'){
                putBackspace(buf[p]);
            }
            else{
                putc(buf[p]);
            }
        }
        
    }

    return n;

}
