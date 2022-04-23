#include "paging.h" //include header files
#include "lib.h"

//PD_entry page_dir[TOTAL_ENTRIES] __attribute__((aligned(4096))); //magic number: data align by 4096 = 2^12 
PTE page_tab[TOTAL_ENTRIES] __attribute__((aligned(4096))); 
/*above reference: https://wiki.osdev.org/Setting_Up_Paging*/

 /* setup_paging_structures()
 *   DESCRIPTION: Used to setup the paging structs-attributes, addresses, etc.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets up paging memory
 */
void setup_paging_structures(void){
    int i;
    for (i = 0; i < TOTAL_ENTRIES; i++){
        page_tab[i].p = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].r_w = 1; //set r_w bit to 1
        page_tab[i].u_s = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].pwt = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].pcd = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].a = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].d = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].pat = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].g = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].avl_3bits = 0;  //Magic Num: Setting as Unpresent
        page_tab[i].base_address = 0;   //Magic Num: Setting as Unpresent

        video_pt[i].p = 0;   //Magic Num: Setting as Unpresent
        video_pt[i].r_w = 1; //set r_w bit to 1
        video_pt[i].u_s = 0;  //Magic Num: Setting as Unpresent
        video_pt[i].pwt = 0;  //Magic Num: Setting as Unpresent
        video_pt[i].pcd = 0;  //Magic Num: Setting as Unpresent
        video_pt[i].a = 0;  //Magic Num: Setting as Unpresent
        video_pt[i].d = 0;  //Magic Num: Setting as Unpresent
        video_pt[i].pat = 0;  //Magic Num: Setting as Unpresent
        video_pt[i].g = 0;  //Magic Num: Setting as Unpresent
        video_pt[i].avl_3bits = 0;  //Magic Num: Setting as Unpresent
        video_pt[i].base_address = i; 

        /*initialize the page directory as all empty*/
        page_dir[i]._PDE_regular.p = 0;
        page_dir[i]._PDE_regular.r_w = 0;
        page_dir[i]._PDE_regular.u_s = 0;
        page_dir[i]._PDE_regular.pwt = 0;
        page_dir[i]._PDE_regular.pcd = 0;
        page_dir[i]._PDE_regular.a = 0;
        page_dir[i]._PDE_regular.avl_1bit = 0;
        page_dir[i]._PDE_regular.ps = 0;
        page_dir[i]._PDE_regular.avl_3bits = 0;
        page_dir[i]._PDE_regular.base_address = 0; 

        /*initialize the Page Table 4 MB portion of PD*/
        if (i == 0){
            page_dir[i]._PDE_regular.p = 1;  //Magic Num: Setting as present
            page_dir[i]._PDE_regular.r_w = 1;  //Magic Num: Setting as present
            page_dir[i]._PDE_regular.u_s = 0;
            page_dir[i]._PDE_regular.pwt = 0;
            page_dir[i]._PDE_regular.pcd = 0;
            page_dir[i]._PDE_regular.a = 0;
            page_dir[i]._PDE_regular.avl_1bit = 0;
            page_dir[i]._PDE_regular.ps = 0; //page size = 0 for page table 
            page_dir[i]._PDE_regular.avl_3bits = 0;
            page_dir[i]._PDE_regular.base_address = (unsigned int)page_tab >> 12; //12 is the page-aligned offset that contains the attributes (which I shift or offset away)
        }

        /*initialize the kernel 4 MB page portion of PD*/
        if (i == 1){
            page_dir[i]._PDE_kernel_4MB.p = 1;  //Magic Num: Setting as present
            page_dir[i]._PDE_kernel_4MB.r_w = 1;  //Magic Num: Setting as present
            page_dir[i]._PDE_kernel_4MB.u_s = 0;
            page_dir[i]._PDE_kernel_4MB.pwt = 0;
            page_dir[i]._PDE_kernel_4MB.pcd = 1;
            page_dir[i]._PDE_kernel_4MB.a = 0;
            page_dir[i]._PDE_kernel_4MB.d = 0;
            page_dir[i]._PDE_kernel_4MB.ps = 1; //page size = 1 for kernel
            page_dir[i]._PDE_kernel_4MB.g = 1; //global kernel page 3.3
            page_dir[i]._PDE_kernel_4MB.avl_3bits = 0;
            page_dir[i]._PDE_kernel_4MB.pat = 0;
            page_dir[i]._PDE_kernel_4MB.base_addr2 = 0;
            page_dir[i]._PDE_kernel_4MB.rsvd = 0; //always set to 1
            page_dir[i]._PDE_kernel_4MB.base_address = i; 
        }
    }
    
    page_tab[paging_vidmem >> DATA_ALIGN_SHIFT].base_address = paging_vidmem >> DATA_ALIGN_SHIFT;
    page_tab[paging_vidmem >> DATA_ALIGN_SHIFT].p = 1;
    page_tab[terminal_back1  >> DATA_ALIGN_SHIFT].base_address = terminal_back1 >> DATA_ALIGN_SHIFT;
    page_tab[terminal_back1 >> DATA_ALIGN_SHIFT].p = 1;
    page_tab[terminal_back2 >> DATA_ALIGN_SHIFT].base_address = terminal_back2 >> DATA_ALIGN_SHIFT;
    page_tab[terminal_back2 >> DATA_ALIGN_SHIFT].p = 1;
    page_tab[terminal_back3 >> DATA_ALIGN_SHIFT].base_address = terminal_back3 >> DATA_ALIGN_SHIFT;
    page_tab[terminal_back3 >> DATA_ALIGN_SHIFT].p = 1;
    // allocating page within startup
    return;
}

/* initialize_paging()
 *   DESCRIPTION: Used to initialize paging
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: writes to video memory, kernel memory-basically 0 MB to 8 MB, and clobbers registers eax, cr3, cr0, and cr4
 */
void initialize_paging(void){
    setup_paging_structures();
//    printf("plz print in test_debug\n");
    /*enable paging: below from wiki.osdev.org*/
    asm volatile(
        "movl $page_dir, %%eax;"
        "movl %%eax, %%cr3            ;"
        /*set the paging (PG) and protection (PE) bits of CR0.*/
        "movl %%cr4, %%eax        ;" 
        "orl $0x10, %%eax        ;" //0x10 is necessary to enable PSE (4 MiB pages), which sets very specific bits in register cr4.
        "movl %%eax, %%cr4        ;"
        /*enable PSE (4 MiB pages)*/
        "movl %%cr0, %%eax        ;"
        "orl $0x80000001, %%eax  ;" //0x80000001 sets paging (PG) and protection (PE) bits of CR0 (wiki.osdev)
        "movl %%eax, %%cr0        ;"
        : //savedd outputs
        : //saved inputs
        :"%eax" //"saved" clobbered regs
    );
    return;
}



