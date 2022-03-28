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

    TERMINALFLAG = 0;

    while(TERMINALFLAG == 0){

    }


    if(n > 127){
        n = 127;
    }


    unsigned char * myKeyboardBuffer = getKeyboardBuffer();

    //check if the keyboard buffer size is greater thatn n or not 
    //b terminal.c: 37
    //b terminal.c:48
    //b tests.c:343
    buf = strncpy(buf, myKeyboardBuffer, n);

    // int p = 0;
    // for(p = 0; p < n; p++){
    //     if(buf[p] != '\0'){
    //         buf[p] = myKeyboardBuffer[p];
    //     }
    // }

    clearKeyboardBuffer();
    TERMINALFLAG = 0;
    return n;
}

int terminal_write(int n, unsigned char * buf){
    //check if the keyboard buffer size is greater thatn n or not 

    
    int charsPrinted = 0; 

    if(n > 127){
        n = 127;
    }

    int p = 0; 
    for(p = 0; p < n; p++){
        if(p == 80){
            newLine();
        }
        if(buf[p] != '\0'){
            if(buf[p] == '\n'){
                newLine();
            }
            else if (buf[p] == '\t'){
                putc(' ');
                putc(' ');
                putc(' ');
                putc(' ');
                charsPrinted = charsPrinted + 4;
            }
            else if (buf[p] == '\b'){
                putBackspace(buf[p]);
                charsPrinted--;
            }
            else{
                putc(buf[p]);
                charsPrinted++;
            }
        }
        
    }
    if(charsPrinted > 80){
        newLine();
    }
    return n;

}
