
//Used to initalize the Keyboard
extern void initialize_Keyboard(void);

//Used as the Handler for the Keyboard
extern void interrupt_keyboard(void);

//Used to grab the keyboard buffer
extern unsigned char* getKeyboardBuffer(void);
 int TERMINALFLAG; //used as a shared variable for the other 

