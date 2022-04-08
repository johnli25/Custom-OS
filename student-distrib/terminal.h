//used to initialize terminal
extern int terminal_init(void);

//used to open terminal
extern int terminal_open(const uint8_t* filename);

//used to close terminal
extern int terminal_close(int32_t fd);

//used to read from terminal
extern int terminal_read(int32_t fd, int n, unsigned char * buf); //will add fd chkpt3

//used to write to terminal
extern int terminal_write(int32_t fd, int n, unsigned char * buf); //will add fd chkpt3

#define keyboardBufferSize 128  //size of the keyboardBuffer 
#define NUM_COLS    80

