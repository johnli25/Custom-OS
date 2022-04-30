/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab */

#include "lib.h"
#include "terminal.h"
#include "systemCalls.h"

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7
#define BACKSPACE   0x0E
#define CURSORD4    0x3D4
#define CURSORD5    0x3D5
#define CURSOR0A    0x0A
#define CURSOR0B    0x0B
#define CURSORC0    0xC0
#define CURSORE0    0xE0
#define CURSOR20    0x20
#define CURSOR0E    0x0E
#define CURSOR0F    0x0F
#define CURSORFF    0xFF
#define KB_4 4096

int counterScreen = 0; //starts off as zero

static int screen_x;
static int screen_y;
static char* video_mem = (char *)VIDEO;


int get_screen_x(){
    return screen_x;
}

int get_screen_y(){
    return screen_y;
}

void set_screen_x(int new_x){
    screen_x = new_x;
}

void set_screen_y(int new_y){
    screen_y = new_y;
}

/* void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
 * Inputs: cursor_start, cursor_end
 * Return Value: none
 * Function: enables the cursor */
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end){
	//have to swap inputs and ouputs
	outb(CURSOR0A,CURSORD4);
	outb((inb(CURSORD5) & CURSORC0) | cursor_start, CURSORD5);
 
	outb(CURSOR0B, CURSORD4);
	outb((inb(CURSORD5) & CURSORE0) | cursor_end, CURSORD5);
}

/* void disable_cursor();
 * Inputs: none
 * Return Value: none
 * Function: disables the cursor */
void disable_cursor(){
//have to swap inputs and outputs
	outb(CURSOR0A, CURSORD4);
	outb(CURSOR20, CURSORD5);
}

/* void update_cursor();
 * Inputs: x, y
 * Return Value: none
 * Function: updates the location of the cursor corresponding to x and y vals */
void update_cursor(int x, int y){
	//have to swap inputs and outputs
	uint16_t pos = y * NUM_COLS+ x;
 
    screen_x = x;
    screen_y = y;

	outb(CURSOR0F, CURSORD4);
	outb( (uint8_t) (pos & CURSORFF), CURSORD5);
	outb( CURSOR0E, CURSORD4);
	outb((uint8_t) ((pos >> 8) & CURSORFF), CURSORD5); // OFFset 8 for cursor
}

/* void get_cursor_position(void);
 * Inputs: none
 * Return Value: none
 * Function: returns the cursors position */
uint16_t get_cursor_position(void){
    //have to swap inputs and outputs
    uint16_t pos = 0;
    outb(CURSOR0F, CURSORD4);
    pos |= inb(CURSORD5);
    outb(CURSOR0E, CURSORD4);
    pos |= ((uint16_t)inb(CURSORD5)) << 8; //Magic Num 8 Used for the correct position for the offset cursor
    return pos;
}



/* void clear(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory */
void clear(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }
    counterScreen = 0;
}

/* void clearText(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears text and video memory */
void clearText(void) {
    screen_x = 7;
    screen_y = 0;
    update_cursor(screen_x, screen_y);
    clear();

    /*for checkpoint 5: rewriting "391os> " here*/
    *(uint8_t *)(video_mem + (0 << 1)) = '3';
    *(uint8_t *)(video_mem + (0 << 1) + 1) = ATTRIB;
    *(uint8_t *)(video_mem + (1 << 1)) = '9';
    *(uint8_t *)(video_mem + (1 << 1) + 1) = ATTRIB;
    *(uint8_t *)(video_mem + (2 << 1)) = '1';
    *(uint8_t *)(video_mem + (2 << 1) + 1) = ATTRIB;
    *(uint8_t *)(video_mem + (3 << 1)) = 'O';
    *(uint8_t *)(video_mem + (3 << 1) + 1) = ATTRIB;
    *(uint8_t *)(video_mem + (4 << 1)) = 'S';
    *(uint8_t *)(video_mem + (4 << 1) + 1) = ATTRIB;
    *(uint8_t *)(video_mem + (5 << 1)) = '>';
    *(uint8_t *)(video_mem + (5 << 1) + 1) = ATTRIB;
    *(uint8_t *)(video_mem + (6 << 1)) = ' ';
    *(uint8_t *)(video_mem + (6 << 1) + 1) = ATTRIB;

    counterScreen = 0;
}

// /* void clearTop(void);
//  * Inputs: void
//  * Return Value: none
//  * Function: Clears top line */
// void clearTop(void) {
//     int32_t i;
//     for (i = 0; i < NUM_COLS; i++) {
//         *(uint8_t *)(video_mem + (i << 1)) = ' ';
//         *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
//     }
// }
/* void clearBottom(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears top line */
void clearBottom(void) {
    int32_t x;
    for (x = 0; x < NUM_COLS; x++) {
        // *(uint8_t *)(video_mem + ((NUM_COLS * 25 + x) << 1)) = ' ';
        // *(uint8_t *)(video_mem + ((NUM_COLS * 25 + x) << 1) + 1) = ATTRIB;
        //*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * (NUM_ROWS-1) + x) << 1)) = ' ';
        *(uint8_t *)(video_mem + ((NUM_COLS * (NUM_ROWS-1) + x) << 1) + 1) = ATTRIB;
    }
    counterScreen = 0;
}

/* void newLine(void);
 * Inputs: void
 * Return Value: none
 * Function: goes to next line */
void newLine(void) {
    if(screen_y == (NUM_ROWS-1)){
        verticalScroll();
        screen_x = 0;
        update_cursor(screen_x, screen_y);
        return;
    }
    screen_x = 0;
    screen_y++;

    update_cursor(screen_x, screen_y);

    counterScreen = 0;

    return;
}



/* void verticalScroll(void);
 * Inputs: void
 * Return Value: none
 * Function: Alters video memory as a means to "scroll" */
 void verticalScroll(void){
     //delete the top line
     //shift other lines all up
     //clear the bottom line
    //whenever screen y == 25 adn a newline is hit (enter or screen x > 80)
    //delte top
    //shift up
    //clear bottom
    //clearTop();

    int32_t x,y;
    // for(x = 0; x < NUM_COLS; x++){
    //     for(y = 0; y < NUM_ROWS; y++){
    //         *(uint8_t *)(video_mem + ((NUM_COLS * y + x) << 1)) = *(uint8_t *)(video_mem + ((NUM_COLS * (y+1) + x) << 1));
    //         //ASK ABOUT ATTRIB STUFF 
    //         //*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
    //     }
    // }
    // clearBottom();
    // screen_y = 25;
    // screen_x = 0;
    for(x = 0; x < NUM_COLS; x++){
        for(y = 0; y < NUM_ROWS - 1; y++){
            *(uint8_t *)(video_mem + ((NUM_COLS * y + x) << 1)) = *(uint8_t *)(video_mem + ((NUM_COLS * (y+1) + x) << 1));
            //ASK ABOUT ATTRIB STUFF 
            *(uint8_t *)(video_mem + ((NUM_COLS * (y+1) + x) << 1) + 1) = ATTRIB;
        }
    }
    clearBottom();
    //screen_y = 24;
    //screen_x = 0;
    
    return;

 }

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output. */
int32_t printf(int8_t *format, ...) {

    // if((strlen(format) + counterScreen) > NUM_COLS){
    //     newLine();
    //     counterScreen = strlen(format);
    // }
    // else{
    //     counterScreen = counterScreen + strlen(format);
    // }

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') { //checks if NULL 
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            putc('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64]; //Magic Num: Size is going to be at least 64 for the characters 
                                if (alternate == 0) {
                                    itoa(*((uint32_t *)esp), conv_buf, 16); //Magic Nums used as offsets
                                    puts(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16); //Magic Nums used as offsets
                                    i = starting_index = strlen(&conv_buf[8]); //Magic Nums used as offsets
                                    while(i < 8) {
                                        conv_buf[i] = '0'; //checks if NULL
                                        i++;
                                    }
                                    puts(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36]; //Size of the new buffer 
                                itoa(*((uint32_t *)esp), conv_buf, 10); //Magic Nums used as offsets
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36]; //36 size of the new buffer - amount of chars 
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10); //Magic Nums used as offsets
                                } else {
                                    itoa(value, conv_buf, 10); //Magic Nums used as offsets
                                }
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c': //checks for c 
                            putc((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's': //checks for s 
                            puts(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                putc(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t puts(int8_t* s) {
    // if((strlen(s) + counterScreen) > NUM_COLS){
    //     newLine();
    //     counterScreen = strlen(s);
    // }
    // else{
    //     counterScreen = counterScreen + strlen(s); 
    // }
    register int32_t index = 0;
    while (s[index] != '\0') { //checks for NULL
        putc(s[index]);
        index++;
    }
    return index;
}

/* void putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console */
void putc(uint8_t c) {
    // if(1 + counterScreen > NUM_COLS){ //1 because the character is size 1 
    //     newLine();
    //     counterScreen = 1; //1 because the character is size 1 
    // }
    // else{
    //     counterScreen = counterScreen + 1; //1 because the character is size 1 
    // }
    if (schedTerm != currTerm)
        return;
    // screen_x = get_cursor_x();
    // screen_y = get_cursor_y();
    // multi_terms[currTerm].cursor_x = get_screen_x();
    // multi_terms[currTerm].cursor_y = get_screen_y();
    // if (schedTerm != currTerm){
    //     putc_background(c, currTerm, schedTerm);
    //     screen_x = multi_terms[currTerm].cursor_x;
    //     screen_y = multi_terms[currTerm].cursor_y;
    //     return;
    // }
    // if (schedTerm != currTerm){
    //     multi_terms[currTerm].cursor_x = get_screen_x();
    //     multi_terms[currTerm].cursor_y = get_screen_y();
    //     terminalPageSwitch(schedTerm);
    //     screen_x = multi_terms[schedTerm].cursor_x;
    //     screen_y = multi_terms[schedTerm].cursor_x;
    // }

    if(c == '\n' || c == '\r') { //checks if Newline or r
        screen_y++;
        screen_x = 0;
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
        screen_x++;
        screen_x %= NUM_COLS;
        screen_y = (screen_y + (screen_x / NUM_COLS)) % NUM_ROWS;
    }
    // if (schedTerm != currTerm){
    //     screen_x = multi_terms[currTerm].cursor_x;
    //     screen_y = multi_terms[currTerm].cursor_x;
    // }
    multi_terms[currTerm].cursor_x = screen_x;
    multi_terms[currTerm].cursor_y = screen_y;
    update_cursor(screen_x, screen_y);
    
}

void putc_background(uint8_t c, int origTerminal, int newTerminal){
    int newAddr = (newTerminal + 1) * KB_4 + VIDEO;
    char* background_mem = (char *)newAddr;

    screen_x = multi_terms[newTerminal].cursor_x;
    screen_y = multi_terms[newTerminal].cursor_y;

    if(c == '\n' || c == '\r') { //checks if Newline or r
        screen_y++;
        screen_x = 0;
    } else {
        *(uint8_t *)(background_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(background_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
        screen_x++;
        screen_x %= NUM_COLS;
        screen_y = (screen_y + (screen_x / NUM_COLS)) % NUM_ROWS;
    }
    update_cursor(screen_x, screen_y);
}
/* void putBackspace(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a backspace to the console */
void putBackspace(uint8_t c, unsigned char * buf){
    //adding if statement for backspace
    if (buf[0] == '\0') //checks if NULL
        return;
    if(screen_y != 0 || screen_x != 0){
        counterScreen--;
        if (screen_x != 0){
            screen_x--;
        }
        else{
            screen_y--;
            screen_x = NUM_COLS; //can cause errors maybe 
            //get keyboard buf
            //parse thru find "\n"
            //-1 of that indice
            int x = 0;
            for(x = (NUM_COLS-1); x > 0; x--){
                if((*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + x) << 1))) != '\0'){
                    break;
                }
                if((*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + x) << 1))) != ' '){
                    break;
                }

            }
            screen_x = x;

            // while(*(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) == ' '){
            //     screen_x--;
            //     if(screen_x == 0){
            //         break;
            //     }
            // }
            // screen_x++;
        } 
    }
    
    // y edgecase 
    // vidmem 
    
    *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = '\0'; //make sure NULL is correct here, can lead to errors
    *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;

    update_cursor(screen_x, screen_y);

    return;

}

/* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
 * Inputs: uint32_t value = number to convert
 *            int8_t* buf = allocated buffer to place string in
 *          int32_t radix = base system. hex, oct, dec, etc.
 * Return Value: number of bytes written
 * Function: Convert a number to its ASCII representation, with base "radix" */
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix) {
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; //Magic NUm: The alphabet used as lookup array 
    int8_t *newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0'; //NULL
        return buf;
    }

    /* Go through the number one place value at a time, and add the
     * correct digit to "newbuf".  We actually add characters to the
     * ASCII string from lowest place value to highest, which is the
     * opposite of how the number should be printed.  We'll reverse the
     * characters later. */
    while (newval > 0) {
        i = newval % radix;
        *newbuf = lookup[i];
        newbuf++;
        newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0'; //NULL

    /* Reverse the string and return */
    return strrev(buf);
}

/* int8_t* strrev(int8_t* s);
 * Inputs: int8_t* s = string to reverse
 * Return Value: reversed string
 * Function: reverses a string s */
int8_t* strrev(int8_t* s) {
    register int8_t tmp;
    register int32_t beg = 0;
    register int32_t end = strlen(s) - 1;

    while (beg < end) {
        tmp = s[end];
        s[end] = s[beg];
        s[beg] = tmp;
        beg++;
        end--;
    }
    return s;
}

/* uint32_t strlen(const int8_t* s);
 * Inputs: const int8_t* s = string to take length of
 * Return Value: length of string s
 * Function: return length of string s */
uint32_t strlen(const int8_t* s) {
    register uint32_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* void* memset(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive bytes of pointer s to value c */
void* memset(void* s, int32_t c, uint32_t n) {
    c &= 0xFF;
    asm volatile ("                 \n\
            .memset_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memset_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memset_aligned \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memset_top     \n\
            .memset_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     stosl           \n\
            .memset_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memset_done    \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%edx       \n\
            jmp     .memset_bottom  \n\
            .memset_done:           \n\
            "
            :
            : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_word(void* s, int32_t c, uint32_t n);
 * Description: Optimized memset_word
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set lower 16 bits of n consecutive memory locations of pointer s to value c */
void* memset_word(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosw           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_dword(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive memory locations of pointer s to value c */
void* memset_dword(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosl           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memcpy(void* dest, const void* src, uint32_t n);
 * Inputs:      void* dest = destination of copy
 *         const void* src = source of copy
 *              uint32_t n = number of byets to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of src to dest */
void* memcpy(void* dest, const void* src, uint32_t n) {
    asm volatile ("                 \n\
            .memcpy_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memcpy_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memcpy_aligned \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memcpy_top     \n\
            .memcpy_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     movsl           \n\
            .memcpy_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memcpy_done    \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%edx       \n\
            jmp     .memcpy_bottom  \n\
            .memcpy_done:           \n\
            "
            :
            : "S"(src), "D"(dest), "c"(n)
            : "eax", "edx", "memory", "cc"
    );
    return dest;
}

/* void* memmove(void* dest, const void* src, uint32_t n);
 * Description: Optimized memmove (used for overlapping memory areas)
 * Inputs:      void* dest = destination of move
 *         const void* src = source of move
 *              uint32_t n = number of byets to move
 * Return Value: pointer to dest
 * Function: move n bytes of src to dest */
void* memmove(void* dest, const void* src, uint32_t n) {
    asm volatile ("                             \n\
            movw    %%ds, %%dx                  \n\
            movw    %%dx, %%es                  \n\
            cld                                 \n\
            cmp     %%edi, %%esi                \n\
            jae     .memmove_go                 \n\
            leal    -1(%%esi, %%ecx), %%esi     \n\
            leal    -1(%%edi, %%ecx), %%edi     \n\
            std                                 \n\
            .memmove_go:                        \n\
            rep     movsb                       \n\
            "
            :
            : "D"(dest), "S"(src), "c"(n)
            : "edx", "memory", "cc"
    );
    return dest;
}

/* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
 * Inputs: const int8_t* s1 = first string to compare
 *         const int8_t* s2 = second string to compare
 *               uint32_t n = number of bytes to compare
 * Return Value: A zero value indicates that the characters compared
 *               in both strings form the same string.
 *               A value greater than zero indicates that the first
 *               character that does not match has a greater value
 *               in str1 than in str2; And a value less than zero
 *               indicates the opposite.
 * Function: compares string 1 and string 2 for equality */
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n) {
    int32_t i;
    for (i = 0; i < n; i++) {
        if ((s1[i] != s2[i]) || (s1[i] == '\0') /* || s2[i] == '\0' */) {

            /* The s2[i] == '\0' is unnecessary because of the short-circuit
             * semantics of 'if' expressions in C.  If the first expression
             * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
             * s2[i], then we only need to test either s1[i] or s2[i] for
             * '\0', since we know they are equal. */
            return s1[i] - s2[i];
        }
    }
    return 0;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 * Return Value: pointer to dest
 * Function: copy the source string into the destination string */
int8_t* strcpy(int8_t* dest, const int8_t* src) {
    int32_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 *                uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n) {
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 *                uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
unsigned char* strncpyUnsignedChar(unsigned char * dest, const unsigned char * src, int n){
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/* void test_interrupts(void)
 * Inputs: void
 * Return Value: void
 * Function: increments video memory. To be used to test rtc */
void test_interrupts(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        video_mem[i << 1]++;
    }

}
