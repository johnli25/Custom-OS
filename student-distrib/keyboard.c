#include "lib.h"
#include "i8259.h"
#include "keyboard.h"

#define KEYBOARDIRQNUM  1       //corresponds to the keyboard IRQ Number
#define KEYBOARDPORT    0x60    //corresponds to the keyboard port 
#define scancodesSize   85       //size of the scancodes1 array for now
#define keyboardBufferSize 128  //size of the keyboardBuffer 
#define NUM_COLS    80
#define NUM_ROWS    25


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
#define LEFTSHIFTRELEASE 0xAA
#define RIGHTSHIFTPRESS     0x36
#define RIGHTSHIFTRELEASE   0xB6
#define SPACEPRESS  0x39
#define CAPSLOCKPRESS   0x3A
#define RELEASEDCHAR 0x80
#define LCHARACTER 0x26
#define UPCHAR  0x48
#define DOWNCHAR 0x50
#define RIGHTCHAR 0x4D
#define LEFTCHAR 0x4B

#define INTTRUE 1
#define INTFALSE 0


int capsLock = INTFALSE;
int shift = INTFALSE;
int control = INTFALSE;

int counter = 0; //starts off as zero


static unsigned char keyboardBuffer[keyboardBufferSize];

/* 
 *getKeyboardBuffer
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: allows other files to get the keyboard buffer
 */
unsigned char* getKeyboardBuffer(){
    return keyboardBuffer;
}

/* 
 *clearKeyboardBuffer
 *   DESCRIPTION: used to clear the keyboard buffer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: allows us to clear keyboard buffer
 */
void clearKeyboardBuffer(){
    int i = 0;
    for(i = 0; i < keyboardBufferSize; i++){
        keyboardBuffer[i] = '\0'; //checks if NULL
    }
}


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

    enable_cursor(0, NUM_ROWS); //cursor goes from 0 to 25 - max rows 

    TERMINALFLAG = INTFALSE;

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

    if(counter == (keyboardBufferSize-1)){ //checks if the counter is equal to the max size 127
        //newLine();
        //TERMINALFLAG = INTTRUE;
        //counter = 0;
        // send_eoi(KEYBOARDIRQNUM);
        // sti();
        // return;
    }

    uint8_t myInput = inb(KEYBOARDPORT); // grabs the input data from the keyboard

    if(myInput == BACKSPACEPRESS){
        if (keyboardBuffer[counter - 1] == '\t'){ //to show a tab
            putBackspace(keyboardBuffer[counter-1]);
            putBackspace(keyboardBuffer[counter-1]);
            putBackspace(keyboardBuffer[counter-1]);
        }
        //put 4 back
        
        putBackspace(keyboardBuffer[counter-1]); //video memory for backspace
       
        if(counter != 0){
            counter--;
            keyboardBuffer[counter] = '\0'; //to show a NULL 
        }
    }

    if(myInput == SPACEPRESS){ //checks if it is a space
        if(counter != (keyboardBufferSize-1)){
            if(counter == NUM_COLS){
                newLine();
            }
            putc(' ');
            keyboardBuffer[counter] = ' '; // to show a space 
            counter++;
        }
    }
    if (myInput == ENTERPRESS){
        //clearKeyboardBuffer();
        keyboardBuffer[counter] = '\n'; //to show new line
        counter++; //added newline character when enter pressed
        TERMINALFLAG = INTTRUE;
        counter = 0; //resets counter - may or may not need to do!
        newLine();
    }
    if(myInput == TABPRESS){
        //flag 1
        //if counter < 124
        if(counter < (keyboardBufferSize-4)){
            //80 and 76
            if(counter < NUM_COLS && counter > NUM_COLS-4){
                newLine();
            }
            putc(' ');
            putc(' ');
            putc(' ');
            putc(' ');
            keyboardBuffer[counter] = '\t'; // to show tab
            counter++;
        }
    }

    if(myInput == CAPSLOCKPRESS){ //checks for caps lock
        if (capsLock == INTFALSE){
            capsLock = INTTRUE;
        }
        else{
            capsLock = INTFALSE;
        }
    }

    if((myInput == LEFTSHIFTPRESS) || (myInput == RIGHTSHIFTPRESS)){ //checks for shift 
        shift = INTTRUE;
    }

    if((myInput == LEFTSHIFTRELEASE) || (myInput == RIGHTSHIFTRELEASE)){ //checks for shift
        shift = INTFALSE;
    }

    if((myInput == LEFTCONTROLPRESS) || (myInput == RIGHTCONTROLPRESS)){ //checks for control 
        control = INTTRUE;
    }

    if((myInput == LEFTCONTROLRELEASE) || (myInput == RIGHTCONTROLRELEASE)){ //checks for control
        control = INTFALSE;
    }

    if(control == INTTRUE && myInput == LCHARACTER){ //control l clears screen
        clearText();
        counter = 0;
        clearKeyboardBuffer(); //THIS IS FROM THE TA - OKAN - TO CLEAR BUFFER 
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    //ignore the arrow keys
    if(myInput == UPCHAR){
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    if(myInput == DOWNCHAR){
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    if(myInput == LEFTCHAR){
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    if(myInput == RIGHTCHAR){
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    //if the keyboard isn't a pressed key, ignore
   if(myInput > RELEASEDCHAR){
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    uint8_t myChar;

    if(capsLock == INTTRUE){
        if(shift == INTTRUE){
            myChar = scancodesCapShift[myInput]; // the corresponding character (from the table)
            
            if(myChar != ' '){ //checks if its a valid character to print
                if(counter != (keyboardBufferSize-1)){
                    if(counter == NUM_COLS){
                        newLine();
                    }
                    putc(myChar); //outputs the correct character
                    keyboardBuffer[counter] = myChar;
                    counter++;
                }
            }

        }
        else{
            myChar = scancodesCapLetters[myInput]; // the corresponding character (from the table)

            if(myChar != ' '){ //checks if its a valid character to print
                if(counter != (keyboardBufferSize-1)){
                    if(counter == NUM_COLS){
                        newLine();
                    }
                    putc(myChar); //outputs the correct character
                    keyboardBuffer[counter] = myChar;
                    counter++;
                }
            }
        }
    }

    else if(shift == INTTRUE){
        myChar = scancodesShift[myInput]; // the corresponding character (from the table)

        if(myChar != ' '){ //checks if its a valid character to print
            if(counter != (keyboardBufferSize-1)){
                if(counter == NUM_COLS){
                    newLine();
                }
                putc(myChar); //outputs the correct character
                keyboardBuffer[counter] = myChar;
                counter++;
            }
        }
    }
    else{
        myChar = scancodes1[myInput]; // the corresponding character (from the table)

        if(myChar != ' '){ //checks if its a valid character to print
            if(counter != (keyboardBufferSize-1)){
                if(counter == NUM_COLS){
                    newLine();
                }
                putc(myChar); //outputs the correct character
                keyboardBuffer[counter] = myChar;
                counter++;
            }
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

