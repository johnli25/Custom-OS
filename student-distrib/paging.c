#include "paging.h" //include header files

void setup_paging_structures(){
    int i;
    for (i = 0; i < TOTAL_ENTRIES; i++){
        
    }
}

void initialize_paging(){


    // asm volatile(
    //     "movl $page_directory, %eax ;"
    //     /*andl $0xfffffc00, %eax*/
    //     "movl %eax, %cr3            ;"

    //     /*set the paging (PG) and protection (PE) bits of CR0.*/
    //     "movl %cr4, %eax        ;"
    //     "orl $0x10, %eax        ;"
    //     "movl %eax, %cr4        ;"

    //     /*enable PSE (4 MiB pages)*/
    //     "movl %cr0, %eax        ;"
    //     "orl $0x80000001, %eax  ;"
    //     "movl %eax, %cr0        ;"
    // );
}

