#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "terminal.h"


int capsLock = INTFALSE;
int shift = INTFALSE;
int control = INTFALSE;
int alt = INTFALSE;

//int counter = 0; //starts off as zero

//static unsigned char keyboardBuffer[keyboardBufferSize]; //main keyboard buffer
static unsigned char keyboardBuffers[NUM_OF_KEYBOARDS][keyboardBufferSize];
int counters[NUM_OF_KEYBOARDS] = {0, 0, 0};

// static unsigned char keyboardBuffer0[keyboardBufferSize];
// static unsigned char keyboardBuffer1[keyboardBufferSize];
// static unsigned char keyboardBuffer2[keyboardBufferSize];


//static unsigned char keyboardBuffer2[NUM_OF_KEYBOARDS][keyboardBufferSize];
//static unsigned char keyboardBuffer3[NUM_OF_KEYBOARDS][keyboardBufferSize];

/* 
 *getKeyboardBuffer
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: allows other files to get the keyboard buffer
 */
unsigned char* getKeyboardBuffer(void){
    // if(terminalCount == 0){
    //     return keyboardBuffer0;
    // }
    // if(terminalCount == 1){
    //     return keyboardBuffer1;
    // }
    // return keyboardBuffer2;
    return keyboardBuffers[currTerm];
}

/* 
 *clearKeyboardBuffer
 *   DESCRIPTION: used to clear the keyboard buffer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: allows us to clear keyboard buffer
 */
void clearKeyboardBuffer(void){
    // if(terminalCount == 0){
    //     int i1 = 0;
    //     for(i1 = 0; i1 < keyboardBufferSize; i1++){
    //         keyboardBuffer0[i] = '\0'; //checks if NULL
    //     }
    //     return;
    // }

    // if(terminalCount == 1){
    //     int i2 = 0;
    //     for(i2 = 0; i2 < keyboardBufferSize; i2++){
    //         keyboardBuffer1[i] = '\0'; //checks if NULL
    //     }
    //     return;
    // }

    // int i3 = 0;
    // for(i3 = 0; i3 < keyboardBufferSize; i3++){
    //     keyboardBuffer2[i] = '\0'; //checks if NULL
    // }
    // return;

    int i = 0;
    for(i = 0; i < keyboardBufferSize; i++){
        keyboardBuffers[currTerm][i] = '\0'; //checks if NULL
    }
    return;
    
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

    if(counters[currTerm] == (keyboardBufferSize-1)){ //checks if the counter is equal to the max size 127
        //newLine();
        //TERMINALFLAG = INTTRUE;
        //counter = 0;
        // send_eoi(KEYBOARDIRQNUM);
        // sti();
        // return;
    }

    uint8_t myInput = inb(KEYBOARDPORT); // grabs the input data from the keyboard

    if(myInput == BACKSPACEPRESS){
        if (keyboardBuffers[currTerm][counters[currTerm] - 1] == '\t'){ //to show a tab
            putBackspace(keyboardBuffers[currTerm][counters[currTerm]-1], (unsigned char *)keyboardBuffers[currTerm]);
            putBackspace(keyboardBuffers[currTerm][counters[currTerm]-1], (unsigned char *)keyboardBuffers[currTerm]);
            putBackspace(keyboardBuffers[currTerm][counters[currTerm]-1], (unsigned char *)keyboardBuffers[currTerm]);
        }
        //put 4 back
        
        putBackspace(keyboardBuffers[currTerm][counters[currTerm]-1], (unsigned char *)keyboardBuffers[currTerm]); //video memory for backspace
       
        if(counters[currTerm] != 0){
            counters[currTerm]--;
            keyboardBuffers[currTerm][counters[currTerm]] = '\0'; //to show a NULL 
        }
    }

    if(myInput == SPACEPRESS){ //checks if it is a space
        if(counters[currTerm] != (keyboardBufferSize-1)){
            if(counters[currTerm] == NUM_COLS){
                newLine();
            }
            putc(' ');
            keyboardBuffers[currTerm][counters[currTerm]] = ' '; // to show a space 
            counters[currTerm]++;
        }
    }
    if (myInput == ENTERPRESS){
        //clearKeyboardBuffer();
        keyboardBuffers[currTerm][counters[currTerm]] = '\n'; //to show new line
        counters[currTerm]++; //added newline character when enter pressed
        TERMINALFLAG = INTTRUE;
        counters[currTerm] = 0; //resets counter - may or may not need to do!
        newLine();
    }
    if(myInput == TABPRESS){
        //flag 1
        //if counter < 124
        if(counters[currTerm] < (keyboardBufferSize-4)){
            //80 and 76
            if(counters[currTerm] < NUM_COLS && counters[currTerm] > NUM_COLS-4){
                newLine();
            }
            putc(' ');
            putc(' ');
            putc(' ');
            putc(' ');
            keyboardBuffers[currTerm][counters[currTerm]] = '\t'; // to show tab
            counters[currTerm]++;
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

    if(myInput == ALTPRESS){
        alt = INTTRUE;
    }

    if(myInput == ALTRELEASE){
        alt = INTFALSE;
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
        counters[currTerm] = 0;
        clearKeyboardBuffer(); //THIS IS FROM THE TA - OKAN - TO CLEAR BUFFER 
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    // NOT SURE if we need to implement CONTROL C  
    // if(control == INTTRUE && myInput == CCHARACTER){

    // }

    if(alt == INTTRUE && myInput == FONE){
        //memcpy(keyboardBuffers[currTerm], keyboardBuffer, sizeof(keyboardBuffer));
        currTerm = 0;
        switch_terms(0);
        //memcpy(keyboardBuffer, keyboardBuffers[0], sizeof(keyboardBuffers[0]));
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    if(alt == INTTRUE && myInput == FTWO){
        //memcpy(keyboardBuffers[currTerm], keyboardBuffer, sizeof(keyboardBuffer));
        currTerm = 1;
        switch_terms(1);
        //memcpy(keyboardBuffer, keyboardBuffers[1], sizeof(keyboardBuffers[1]));
        send_eoi(KEYBOARDIRQNUM);
        sti();
        return;
    }

    if(alt == INTTRUE && myInput == FTHREE){
        //memcpy(keyboardBuffers[currTerm], keyboardBuffer, sizeof(keyboardBuffer));
        currTerm = 2;
        switch_terms(2);
        //memcpy(keyboardBuffer, keyboardBuffers[2], sizeof(keyboardBuffers[2]));
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
                if(counters[currTerm] != (keyboardBufferSize-1)){
                    if(counters[currTerm] == NUM_COLS){
                        newLine();
                    }
                    putc(myChar); //outputs the correct character
                    keyboardBuffers[currTerm][counters[currTerm]] = myChar;
                    counters[currTerm]++;
                }
            }

        }
        else{
            myChar = scancodesCapLetters[myInput]; // the corresponding character (from the table)

            if(myChar != ' '){ //checks if its a valid character to print
                if(counters[currTerm] != (keyboardBufferSize-1)){
                    if(counters[currTerm] == NUM_COLS){
                        newLine();
                    }
                    putc(myChar); //outputs the correct character
                    keyboardBuffers[currTerm][counters[currTerm]] = myChar;
                    counters[currTerm]++;
                }
            }
        }
    }

    else if(shift == INTTRUE){
        myChar = scancodesShift[myInput]; // the corresponding character (from the table)

        if(myChar != ' '){ //checks if its a valid character to print
            if(counters[currTerm] != (keyboardBufferSize-1)){
                if(counters[currTerm] == NUM_COLS){
                    newLine();
                }
                putc(myChar); //outputs the correct character
                keyboardBuffers[currTerm][counters[currTerm]] = myChar;
                counters[currTerm]++;
            }
        }
    }
    else{
        myChar = scancodes1[myInput]; // the corresponding character (from the table)

        if(myChar != ' '){ //checks if its a valid character to print
            if(counters[currTerm] != (keyboardBufferSize-1)){
                if(counters[currTerm] == NUM_COLS){
                    newLine();
                }
                putc(myChar); //outputs the correct character
                keyboardBuffers[currTerm][counters[currTerm]] = myChar;
                counters[currTerm]++;
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

