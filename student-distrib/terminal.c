#include "terminal.h"
#include "keyboard.h"

//HAVE TO ADD IN INTERFACES

int terminal_open(){
    return 0;
}
int terminal_close(){
    return 0;
}

int terminal_read(int n, unsigned char * buf){


    unsigned char * myKeyboardBuffer = getKeyboardBuffer();

//    strcpy(buf, getKeyboardBuffer());

    int p = 0;
    for(p = 0; p < n; p++){
        buf[p] = myKeyboardBuffer[p];
    }

    clearKeyboardBuffer();

    return n;

}
