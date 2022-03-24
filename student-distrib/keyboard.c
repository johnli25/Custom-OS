#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

#include <stdio.h>
#include <stdbool.h>

#define KEYBOARDIRQNUM  1       //corresponds to the keyboard IRQ Number
#define KEYBOARDPORT    0x60    //corresponds to the keyboard port 
#define scancodesSize   85       //size of the scancodes1 array for now
//#define keyboardPassPresses  87



// SPECIAL CHARACTERS:
#define BACKSPACEPRESS   0X0E
#define TABPRESS     0X0F
#define ENTERPRESS  0X1C
#define LEFTCONTROLPRESS     0x1D
#define LEFTCONTROLRELEASE   0x9D
// #define RIGHTCONTROLPRESS1  0xE0
// #define RIGHTCONTROLRELEASE1    0xE0
#define RIGHTCONTROLPRESS  0x1D
#define RIGHTCONTROLRELEASE 0x9D
#define LEFTSHIFTPRESS   0x2A 
#define LEFTSHIFTRELEASE 0x12
#define RIGHTSHIFTPRESS     0x36
#define RIGHTSHIFTRELEASE   0xB6
#define SPACEPRESS  0x39
#define CAPSLOCKPRESS   0x3A




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
//scancodesShift array - converts the character to the correct character when shift key pressed
/* 
 * scancodesShift array
 *   DESCRIPTION: Used to convert the input data into a corresponding
 *          character - based off of scancodes set shift from OSDev

 */
uint8_t scancodesShift[scancodesSize] = {
    ' ', ' ', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', ' ',
    ' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
    ' ', ' ', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',
    ' ', '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '\?', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
    '3', '0', '.'

};

//scancodesCap array - converts the character to the correct character when cap is on
/* 
 * scancodesCap array
 *   DESCRIPTION: Used to convert the input data into a corresponding
 *          character - based off of scancodes set cap from OSDev

 */
uint8_t scancodesCapLetters[scancodesSize] = {
    ' ', ' ', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', ' ',
    ' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']',
    ' ', ' ', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',
    ' ', '\\', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', '7', '8', '9', '-', '4', '5', '6', '+', '1', '2',
    '3', '0', '.'

};

//scancodesCapShift array - converts the character to the correct character when cap and shift is on
/* 
 * scancodesCapShift array
 *   DESCRIPTION: Used to convert the input data into a corresponding
 *          character - based off of scancodes set cap and shift from OSDev

 */
uint8_t scancodesCapShift[scancodesSize] = {
    ' ', ' ', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', ' ',
    ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}',
    ' ', ' ', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', '\"', '~',
    ' ', '|', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<', '>', '\?', ' ',
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

    bool capsLock = false;
    bool shift = false;

    cli();  //prevents interrupts 

    uint8_t myInput = inb(KEYBOARDPORT); // grabs the input data from the keyboard

    if(myInput == BACKSPACEPRESS){
        putBackspace();
    }

    if(myInput == SPACEPRESS){
        putc(' ');
    }

    if(myInput == TABPRESS){
        putc(' ');
        putc(' ');
        putc(' ');
        putc(' ');
    }

    if(myInput == CAPSLOCKPRESS){
        capsLock = !capsLock;
    }

    if((myInput == LEFTSHIFTPRESS) || (myInput == RIGHTSHIFTPRESS)){ //DOUBLE CHECK ABOUT SHIFT 
        shift = true;
    }

    if(myInput == LEFTSHIFTRELEASE) || (myInput == RIGHTSHIFTRELEASE){
        shift = false;
    }



    uint8_t myChar;
    if(capsLock){
        if(shift){
            myChar = scancodesCapShift[myInput]; // the corresponding character (from the table)

            if(myChar != ' '){ //checks if its a valid character to print
                putc(myChar); //outputs the correct character
            }

        }
        else{
            myChar = scancodesCapLetters[myInput]; // the corresponding character (from the table)

            if(myChar != ' '){ //checks if its a valid character to print
                putc(myChar); //outputs the correct character
            }
        }
    }

    else if(shift){
        myChar = scancodesShift[myInput]; // the corresponding character (from the table)

        if(myChar != ' '){ //checks if its a valid character to print
            putc(myChar); //outputs the correct character
        }
    }
    else{
        myChar = scancodes1[myInput]; // the corresponding character (from the table)

        if(myChar != ' '){ //checks if its a valid character to print
            putc(myChar); //outputs the correct character
        }
    }

    // uint8_t myChar = scancodes1[myInput]; // the corresponding character (from the table)

    // if(myChar != ' '){ //checks if its a valid character to print
    //     putc(myChar); //outputs the correct character
    // }

    send_eoi(KEYBOARDIRQNUM); //end of interrupt

    sti(); //interrupts can contnue

    return;

}

