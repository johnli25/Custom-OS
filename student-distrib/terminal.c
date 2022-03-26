#include "terminal.h"
#include "keyboard.h"

//HAVE TO ADD IN INTERFACES

int terminal_open(){
    return 0;
}
int terminal_close(){
    return 0;
}


//ADD IN NULL CHECKS 

int terminal_read(int n, unsigned char * buf){


    unsigned char * myKeyboardBuffer = getKeyboardBuffer();

//    strcpy(buf, getKeyboardBuffer());



    //check if the keyboard buffer size is greater thatn n or not 

    int p = 0;
    for(p = 0; p < n; p++){
        if(buf[p] != '/0'){
            buf[p] = myKeyboardBuffer[p];
        }
    }

    clearKeyboardBuffer();

    return n;

}

int terminal_write(int n, unsigned char * buf){
    //check if the keyboard buffer size is greater thatn n or not 
    int p = 0; 
    for(p = 0; p < n; p++){
        if(buf[p] != '/0'){
            putc(buf[p]);
        }
    }
}
