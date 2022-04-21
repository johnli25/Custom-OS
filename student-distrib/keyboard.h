#ifndef KEYBOARD_H
#define KEYBOARD_H

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
#define ALTPRESS 0x38
#define ALTRELEASE 0xB8
#define CCHARACTER 0x2E
#define FONE 0x3B
#define FTWO 0x3C
#define FTHREE 0x3D




#define INTTRUE 1
#define INTFALSE 0
//Used to initalize the Keyboard
extern void initialize_Keyboard(void);

//Used as the Handler for the Keyboard
extern void interrupt_keyboard(void);

//Used to grab the keyboard buffer
extern unsigned char* getKeyboardBuffer(void);
void clearKeyboardBuffer();
volatile int TERMINALFLAG; //used as a shared variable for the other 

#endif
