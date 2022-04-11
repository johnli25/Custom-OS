#ifndef SYSCALLS_H
#define SYSCALLS_H

#include "filesys.h"
#include "types.h"
#include "rtc.h"

#define EIGHTMB 0x0800000
#define FOURMB 0x400000
#define EIGHTKB 8192
#define MB128_START 0x8000000
#define MB128_OFFSET 0x0048000
#define POGRAM_MEM_START 128/4
#define PO3_OF_ENTRY 24
#define PROG_START_VIRTUAL_ADDR 0x8048000
#define MB_132 0x8400000

#define MAGIC0 0x7F
#define MAGIC1 0x45
#define MAGIC2 0x4C
#define MAGIC3 0x46

typedef struct pcb{
    unsigned int pid;
    //pcb_t* pcb_parent;
    unsigned int parent_id;
    fd_info_t myINFO[8];
    uint32_t saved_esp;
    uint32_t saved_ebp;
    unsigned int active;
} __attribute__((packed)) pcb_t;

extern void paging_helper(int processNum);

extern int32_t execute(const uint8_t * command);
extern int32_t halt(uint8_t status);
extern int32_t general_read(int32_t fd, void * buf, int32_t n);
extern int32_t general_write(int32_t fd, const void * buf, int32_t n);
extern int32_t general_open(const uint8_t * filename);
extern int32_t general_close(int32_t fd);
extern int32_t getargs(uint8_t * buf, int32_t n);
extern int32_t vidmap(uint8_t ** screen_start);
extern int32_t set_handler(int32_t signum, void * handler_addr);
extern int32_t sigreturn(void);

#endif


