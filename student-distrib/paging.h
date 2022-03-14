#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

#define TOTAL_ENTRIES 1024

//create 1024 (use only first 2-ignore next 1022 entries) PDs entries, each 4 MB each
//each PD is either gonna point to 1) 1 page (4 MB) or 2) another page TABLE (1024 Entries, each PT is 4KB)
//each PT entry = page, will point to a 4 KB page (each page is 4 KB)

typedef struct PDE_kernel_4MB{

} PDE_kernel_4MB;

extern void setup_paging_structures();
extern void initialize_paging();

#endif

