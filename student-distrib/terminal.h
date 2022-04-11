#ifndef TERMINAL_H
#define TERMINAL_H
#include "types.h"

//used to initialize terminal
extern int32_t terminal_init(void);

//used to open terminal
extern int32_t terminal_open(const uint8_t* filename);

//used to close terminal
extern int32_t terminal_close(int32_t fd);

//used to read from terminal
extern int32_t terminal_read(int32_t fd, unsigned char * buf, int32_t n); //will add fd chkpt3

//used to write to terminal
extern int32_t terminal_write(int32_t fd, unsigned char * buf, int32_t n); //will add fd chkpt3

#define keyboardBufferSize 128  //size of the keyboardBuffer 
#define NUM_COLS    80

#endif

