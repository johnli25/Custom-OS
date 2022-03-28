extern int terminal_init(void);

extern int terminal_open(void);

extern int terminal_close(void);

extern int terminal_read(int n, unsigned char * buf); //will add fd chkpt3

extern int terminal_write(int n, unsigned char * buf); //will add fd chkpt3

#define keyboardBufferSize 128  //size of the keyboardBuffer 
#define NUM_COLS    80

