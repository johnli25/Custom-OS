#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

#define KEYBOARDIRQNUM  1       //corresponds to the keyboard IRQ Number
#define KEYBOARDPORT    0x60    //corresponds to the keyboard port 
#define scancodesSize   85       //size of the scancodes1 array for now
//#define keyboardPassPresses  87
#define nonnumlet   0x33        // the last character/number (for checkpoint 1)

//scancodes1 array - converts the character to the correct character
/* 
 * scancodes1 array
 *   DESCRIPTION: Used to convert the input data into a corresponding
 *          character - based off of scancodes set 1 from OSDev

 */
uint8_t scancodes1[scancodesSize] = {
    ' ', ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', ' ',
    ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    ' ', ' ', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    ' ', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
    '3', '0', '.'

};



/* 
 * initialize_Keyboard
 *   DESCRIPTION: Used to initialize the keyboard
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes the keyboard on the PIC device
 */
void initialize_Keyboard(void){

    enable_irq(KEYBOARDIRQNUM);     // pic stuff with keyboard

    return;

}


/* 
 * interrupt_keyboard
 *   DESCRIPTION: Handler for the Keyboard
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Takes the input data and converts this 
 *                 data into the correspoing character on the keyboard
 */
void interrupt_keyboard(void){      

    cli();  //prevents interrupts 

    uint8_t myInput = inb(KEYBOARDPORT); // grabs the input data from the keyboard

    if(myInput < nonnumlet){ //checks if it is a character or number 

        uint8_t myChar = scancodes1[myInput]; // the corresponding character (from the table)

        if(myChar != ' '){ //checks if its a valid character to print
            putc(myChar); //outputs the correct character
        }
    }
    send_eoi(KEYBOARDIRQNUM); //end of interrupt

    sti(); //interrupts can contnue

    return;

}

