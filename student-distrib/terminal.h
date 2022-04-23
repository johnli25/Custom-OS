#ifndef TERMINAL_H
#define TERMINAL_H
#include "types.h"
#include "filesys.h"

//used to initialize terminal
typedef struct terminal {
    int cursor_x; 
    int cursor_y;
    // int curr_proc;
    pcb_t * curr_proc;
    int bootup_flag;

} __attribute__((packed)) terminal_t;

extern void switch_terms(int terminalNum);

extern int32_t terminal_init(void);

//used to open terminal
extern int32_t terminal_open(const uint8_t* filename);

//used to close terminal
extern int32_t terminal_close(int32_t fd);

//used to read from terminal
extern int32_t terminal_read(int32_t fd, void * buf, int n); //will add fd chkpt3

//used to write to terminal
extern int32_t terminal_write(int32_t fd, const void * buf, int n); //will add fd chkpt3

#define keyboardBufferSize 128  //size of the keyboardBuffer 
#define NUM_COLS    80

terminal_t multi_terms[3];
static int currTerm = 0;
//static int newTerm = 0;


#endif

