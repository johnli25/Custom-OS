#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

#define TOTAL_ENTRIES 1024
#define KB_4 4096 //1 KB = 1024
#define paging_vidmem 0xB8000
#define terminal_back1 0xB9000
#define terminal_back2 0xBA000
#define terminal_back3 0xBB000

#define DATA_ALIGN_SHIFT 12

//create 1024 (use only first 2-ignore next 1022 entries) PDs entries, each 4 MB each
//each PD is either gonna point to 1) 1 page (4 MB) or 2) another page TABLE (1024 Entries, each PT entry is 4KB)
//each PT entry = page, will point to a 4 KB page (each page is 4 KB)


typedef struct PDE_kernel_4MB{
    unsigned int p : 1;
    unsigned int r_w : 1;
    unsigned int u_s : 1;
    unsigned int pwt : 1;
    unsigned int pcd : 1 ;
    unsigned int a : 1;
    unsigned int d : 1;
    unsigned int ps : 1; //set to 1
    unsigned int g: 1 ;
    unsigned int avl_3bits : 3;
    unsigned int pat : 1;
    unsigned int base_addr2 : 8; //these are actually reserved bits! lol
    unsigned int rsvd : 1;
    unsigned int base_address : 10;
} __attribute__((packed)) PDE_kernel_4MB;

typedef struct PDE_regular{
    unsigned int p : 1;
    unsigned int r_w : 1;
    unsigned int u_s : 1;
    unsigned int pwt : 1;
    unsigned int pcd : 1;
    unsigned int a : 1;
    unsigned int avl_1bit: 1;
    unsigned int ps : 1; //set to 0
    unsigned int avl_3bits : 4;
    unsigned int base_address : 20;
} __attribute__((packed)) PDE_regular;

typedef struct PTE{
    unsigned int p : 1;
    unsigned int r_w : 1;
    unsigned int u_s : 1;
    unsigned int pwt : 1;
    unsigned int pcd : 1;
    unsigned int a : 1;
    unsigned int d : 1;
    unsigned int pat : 1;
    unsigned int g : 1;
    unsigned int avl_3bits : 3;
    uint32_t base_address : 20;
} __attribute__((packed)) PTE;

typedef union PD_entry{
    PDE_kernel_4MB _PDE_kernel_4MB;
    PDE_regular _PDE_regular;
    uint32_t val;
} __attribute__((packed)) PD_entry;

PD_entry page_dir[TOTAL_ENTRIES] __attribute__((aligned(4096))); //magic number: data align by 4096 = 2^12 

PTE video_pt[TOTAL_ENTRIES] __attribute__((aligned(4096))); //4096 = 4 KB (attribute) aligned 

extern void setup_paging_structures(void);
extern void initialize_paging(void);


#endif

