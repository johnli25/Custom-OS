/*

The read system call reads data from the keyboard, a file, device (RTC), or directory.
This call returns the number of bytes read. If the initial file position is at or beyond
the end of file, 0 shall be returned (for normal files and the directory). In the case
of the keyboard, read should return data from one line that has been terminated by 
pressing Enter, or as much as fits in the buffer from one such line. The line returned 
should include the line feed character.

*/

/*

The write system call writes data to the terminal or to a device (RTC). In the case of the 
terminal, all data should be displayed to the screen immediately.

*/

/*

stdin is a read-only file which corresponds to keyboard input. stdout is a write-only file 
corresponding to terminal output.

*/

/*

keyboard input is also printed to the screen from the interrupt handler).

*/

/*

A PS/2 Keyboard accepts many types of commands. A command is one byte. Some commands have data
byte/s which must be sent after the command byte. The keyboard typically responds to a command 
by sending either an "ACK" (to acknowledge the command) or a "Resend" (to say something was wrong
with the previous command) back. Don't forget to wait between the command, the data and the response 
from keyboard.

*/

/*

Use Scan Code set 1

*/


// handler has to deal with grabbing the specefic byte/data from the port - echo back to screen

#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

#define KEYBOARDIRQNUM  1    //corresponds to the keyboard in the IDT
#define KEYBOARDPORT    0x60    //corresponds to the keyboard port 
#define scancodesSize   84        //128
//#define keyboardPassPresses  87

//scancodes 1 - converts the character to the correct character
uint8_t scancodes1[scancodesSize] = {
    //have to initailize all 
    ' ', ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', ' ',
    ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    ' ', ' ', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 
    ' ', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
    '3', '0', '.'

};

void initialize_Keyboard(){

    enable_irq(KEYBOARDIRQNUM);     // pic stuff with keyboard
    
    // int z = 0;
    // for(z = keyboardPassPresses; z < scancodesSize; z++){
    //     scancodes1[z] = ' ';    //making everything that isn't a character to output a space for right now
    // }

    return;

}

void interrupt_keyboard(){      //keyboard handler

    cli();  //prevents interrupts 

    uint32_t myInput = inb(KEYBOARDPORT); // MAYBE CHANGE TO UINT8_T grabs the input data from the keyboard

    uint8_t myChar = scancodes1[(int)myInput];

    if(myChar != ' '){ //checks if its a valid character to print
        putc(myChar); //outputs the correct character after converting the data to a char
    }

    sti(); //interrupts can contnue

    return;

}
