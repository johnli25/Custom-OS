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

#define KEYBOARDIDTNUM 0x21

void initialize_Keyboard(){

    enable_irq(KEYBOARDIDTNUM); //double check 0x21 - course notes - change magic num

    //set_IDT_entry(handler?)

    //scan - something to grab the character ? - inb(port = ?)? 

    //scancodes??

    //putc(character) - output the character
}

// void interrupt_keyboard(){

//     return;
// }