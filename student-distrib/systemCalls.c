#include "systemCalls.h"
#include "keyboard.h"
#include "paging.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"
#include "rtc.h"

fd_info_t fd_array[8]; 

int32_t do_nothing_r(int32_t theres, void * nothing, int lol){
    return -1;
}

int32_t do_nothing_w(int32_t theres, const void * nothing, int lol){
    return -1;
}

int32_t do_nothing_open(const uint8_t * none){
    return -1;
}

int32_t do_nothing_close(int32_t none){
    return -1;
}

fops_t stdin = {terminal_open, terminal_close, terminal_read, do_nothing_w};
fops_t stdout = {terminal_open, terminal_close, do_nothing_r, terminal_write};
fops_t dir_fops = {dir_open, dir_close, dir_read, dir_write};
fops_t file_fops = {file_open, file_close, file_read, file_write};
fops_t rtc_fops = {open_RTC, close_RTC, read_RTC, write_RTC};
fops_t fops_none = {do_nothing_open, do_nothing_close, do_nothing_r, do_nothing_w};
// fops_t stdin;
// stdin.open = (int32_t)terminal_open;

int programNumber[6] = {0,0,0,0,0,0}; 
int currentProgramNumber = 0;


void paging_helper(int processNum){
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.p = 1;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.r_w = 1;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.u_s = 1;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pwt = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pcd = 1;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.a = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.d = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.ps = 1; //page size = 1 for kernel
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.g = 0; //must enable global pages
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.avl_3bits = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pat = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.base_addr2 = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.rsvd = 0; //always set to 1
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.base_address = 2 + processNum; //not sure if this correct? ((processNum * FOUR_MB) + EIGHT_MB)

    asm volatile (
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"  
        : 
        : 
        :"%eax" //saved "clobbered" regs 
    );

}

void paging_unhelper(int processNum){

    if(page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.p){
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.r_w = 1;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.u_s = 1;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pwt = 0;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pcd = 0;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.a = 0;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.d = 0;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.ps = 1; //page size = 1 for kernel
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.g = 0;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.avl_3bits = 0;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pat = 0;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.base_addr2 = 0;
        // page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.rsvd = 0; //always set to 1
        page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.base_address = 2 + processNum; //not sure if this correct? ((processNum * FOUR_MB) + EIGHT_MB)
    }
    asm volatile (
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"  
        : 
        : 
        :"%eax" //saved "clobbered" regs 
    );
    
}

int32_t execute (const uint8_t* command){
    if (!command)
        return -1; 
    int myProgramNumber = 0;
    for(myProgramNumber = 0; myProgramNumber < 6; myProgramNumber++){ //6 is the max # of processes/files
        if(programNumber[myProgramNumber] == 0){
            programNumber[myProgramNumber] = 1;
            break;
        }
        if(myProgramNumber == 5){ //MAGIC #: 5 = MAX NUMBER OF PROCESSES AKA we reached end iteration and they were all filled (= 1)
            return -1; //all of the others are filled
        }
    }
    //command is a string, have to parge (ex: shell)
    //            ls        
    //parse through the string - past 391OS> get after white space 
    int index = 0;
    while(command[index] == ' '){
        index++;
    }

    uint8_t buffer[128];
    int bufIndex = 0;
    while(command[index] != ' ' && command[index] != '\n'){
        buffer[bufIndex] = command[index];
        index++;
        bufIndex++;
    }
    //buffer is the command without all white space ^

    dentry_t myDentry;
    int check = read_dentry_name(buffer, &myDentry);
    if(check == -1){
        return -1; //FAILED TEST
    }

    if (myDentry.file_type != 2){ //checks if valid type (executable)
        return -1; //FAILED TEST 
    }

    uint8_t ELFBUFFER[10];
    read_data(myDentry.inode, 0, ELFBUFFER, 4); //why 4 and not 3?

    if (ELFBUFFER[0] != MAGIC0 || ELFBUFFER[1] != MAGIC1 ||
        ELFBUFFER[2] != MAGIC2 || ELFBUFFER[3] != MAGIC3) //ELF string beginning check
        return -1;

    uint8_t POE_buf[4];
    read_data(myDentry.inode, PO3_OF_ENTRY, POE_buf, 4); //4 is total size of bytes 24-27
    uint8_t POT_var0 = POE_buf[0];
    uint8_t POT_var1 = POE_buf[1];
    uint8_t POT_var2 = POE_buf[2];
    uint8_t POT_var3 = POE_buf[3];

    // POE_buf[0] = POT_var3;
    // POE_buf[1] = POT_var2;
    // POE_buf[2] = POT_var1;
    // POE_buf[3] = POT_var0;
    // int p = 0;
    // while (p < 4){
    //     address |= physicalMemNum[27 - p];
    //     address = address << 8; //may be wrong
    //     p++;
    // }
    uint32_t pt_of_entry = *((uint32_t*)POE_buf);
    
    uint8_t * physicalMemNum = (uint8_t*) (EIGHTMB + (myProgramNumber * FOURMB));// Physical memory starts at 8MB + (process number * 4MB)

    //map to virtual mem
    //zerpadded by 22
    paging_helper(myProgramNumber);
    read_data(myDentry.inode, 0, (unsigned char *)VIRTUAL_ADDR,  FOURMB); //load file into memory
    
    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (myProgramNumber + 1))); //what's the hardcoded numerical addr?
    programNumber[myProgramNumber] = 1;
    currentProgramNumber = myProgramNumber; //update parent number

    //initialize pcb's/fd aray
    mypcb-> myINFO[0].fops_table = &stdin;
    mypcb-> myINFO[1].fops_table = &stdout; 
    mypcb-> myINFO[0].flags = 1; //setting flag to 1
    mypcb-> myINFO[1].flags = 1; //setting flag to 1

    mypcb->myINFO[0].inode = 0; //inodes to 0
    mypcb->myINFO[1].inode = 0;
    mypcb->myINFO[0].file_position = 0; //file position to 0
    mypcb->myINFO[1].file_position = 0;

    int i = 2;
    for (i = 2; i<8; i++){ //magic #: fill in fd_array indices 2-8
        mypcb -> myINFO[i].flags = 0;
        mypcb -> myINFO[i].inode = 0; //inodes to 0
        mypcb -> myINFO[i].file_position = 0; //file position to 0
        mypcb -> myINFO[i].fops_table = &fops_none; //file position to 0

    }

    //save user program bookkeeping info
    mypcb-> pid = myProgramNumber;
    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(mypcb->saved_esp), "=r"(mypcb->saved_ebp)
    );

    //save kernel stack bookkeeping info
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHTMB - (EIGHTKB * (myProgramNumber /*+ 1*/))) - 4;
    
    /*context switch*/
    //enable interrupt on flags: or flags x200 in assembly w register
    asm volatile( 
        "pushl %0 \n" //push USER_DS
        "pushl %1 \n" //push esp
        "pushfl \n" //push eflags
        "popl %%eax \n" //popping eflags into eax
        "orl $0x200, %%eax \n" // trying to Or eax with 200
        "pushl %%eax \n" // pushing eax which contains flags
        "pushl %2 \n" //push USER_CS
        "pushl %3 \n" //push eip = point of entry = 24 onto stack
        "iret \n;"
        :
        : "r"(USER_DS), "r"(MB_132 - 4), "r"(USER_CS), "r"(pt_of_entry)
        : "eax" //clobbered regs
    );
    return 0;
}

int32_t halt(uint8_t status){
    //first, grab esp and ebp pointers from the pcb 
    pcb_t * cHiLdPcB = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1))); //may need this again - may not + 1
    pcb_t * parentPcb = (pcb_t *)(EIGHTMB - (EIGHTKB * ((cHiLdPcB -> parent_id) + 1))); //may need this again - may not + 1
    //pcb_t * mypcb; //FIGURE OUT HOW TO INSTANTIATE!!!
    //saved_ebp = childPcb -> saved_ebp; //what do we do w them?
    //saved_esp = childPcb -> saved_esp; //what do we do w them?
    int32_t haltReturn = (int32_t)(status); //our return value, what do we return? this added as asm return
    
    //set in the pcb array set the current term index to -1 - haltng the process so it goes away - later 5
    
    //process index - parent stuff (above)
    //pcb_t * parentPcb = childPcb -> pcb_parent; //Grabbing the parent PCB
    tss.ss0 = KERNEL_DS;
    //// tss.esp0 = parents - > tss.esp0

    tss.esp0 = (EIGHTMB - (EIGHTKB * (parentPcb->pid /*+ 1*/))) - 4;
    //tss.esp0 = parentPcb -> tss.esp0; //if this gives an error - then just do manually with correct pid of parent

    cHiLdPcB -> myINFO[0].flags = 0;
    cHiLdPcB -> myINFO[1].flags = 0;
    cHiLdPcB -> myINFO[2].flags = 0;
    cHiLdPcB -> myINFO[3].flags = 0;
    cHiLdPcB -> myINFO[4].flags = 0;
    cHiLdPcB -> myINFO[5].flags = 0;
    cHiLdPcB -> myINFO[6].flags = 0;
    cHiLdPcB -> myINFO[7].flags = 0;


    // reload a new shell if a at the root (index i sless than 0) reexecute a shell
    if (cHiLdPcB->pid == 0){
        execute((uint8_t*)"shell");
    }
    // parent process done - now paging 
    // parent paging (unpaging) - paging user program (paging.c?) 8 + (id * 4mb) or with flags into directory, flush
    // TA - set parent as active ? 

    cHiLdPcB -> active = 0; 
    parentPcb -> active = 1;

    paging_unhelper(parentPcb -> pid); // - flushes TLB as well 

    // assembluuuuu linkeage asm - for 
    // halt needs to jump to th eend of execute asm volitile :"-.globl LABEL" - able to return to this label in dif asm volatiles 
    // halt return valu
     asm volatile( 
        "mov %0, %%esp;" //esp contains saved_esp
        "mov %1, %%ebp;" //ebp contains saved_ebp
        "mov %2, %%eax;" //eax contains return value
        "jmp executeEnd;"
        :
        : "r"(cHiLdPcB -> saved_esp), "r"(cHiLdPcB -> saved_ebp), "r"(haltReturn)
        :"%eax" //saved "clobbered" regs 
    );
    //^^where do we jump in asm?
    return haltReturn;
    // return status
    //^^ not sure what to return exactly.

}



int32_t general_read(int32_t fd, void * buf, int32_t n){
    if (fd>=0 && fd < 8){
        pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));
        if (mypcb->myINFO[fd].flags)
            return mypcb->myINFO[fd].fops_table->read(fd, buf, n); 
    }
    return -1; 
}

int32_t general_write(int32_t fd, const  void * buf, int32_t n){
    if ( fd>=0 && fd < 8){
        pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));
        if (mypcb->myINFO[fd].flags) //== 1
            return mypcb->myINFO[fd].fops_table->write(fd, buf, n); 
    }
    return -1; 
}

int32_t general_open(const uint8_t * filename){
    dentry_t d;
    int i = 2;
    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));

    if (read_dentry_name(filename, &d) != 0)
        return -1;

    if (!filename)
        return -1;

    read_dentry_name(filename, &d);
    if (d.file_type==0){ //rtc
        open_RTC (filename); 
        while (i < 8){ //8 is max size idx of fd array
            if (mypcb->myINFO[i].flags == 0) {
                mypcb->myINFO[i].flags = 1;
                mypcb->myINFO[i].file_position = 0;
                mypcb->myINFO[i].inode = 0;
                mypcb->myINFO[i].fops_table = &rtc_fops;
                return i;
            }
            i++;
        }
    }
    if (d.file_type==1){ //dir
        dir_open(filename);
        i = 2;
        while (i < 8){ //8 is max size idx of fd array
            if (mypcb->myINFO[i].flags == 0) {
                mypcb->myINFO[i].flags = 1;
                mypcb->myINFO[i].file_position = 0;
                mypcb->myINFO[i].inode = 0;
                mypcb->myINFO[i].fops_table = &dir_fops;
                return i; 
            }
            i++;
        }
    }
    if (d.file_type==2){ //file
        file_open(filename); 
        i = 2;
        while (i < 8){
            if (mypcb->myINFO[i].flags == 0) {
                mypcb->myINFO[i].flags = 1;
                mypcb->myINFO[i].file_position = 0;
                mypcb->myINFO[i].inode = d.inode;
                mypcb->myINFO[i].fops_table = &file_fops;
                return i;
            }
            i++;
        }
    }
    return -1; //can't find
}

int32_t general_close(int32_t fd){
    if ( fd>=0 && fd < 8){
        pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));
        if (mypcb->myINFO[fd].flags)
            return mypcb->myINFO[fd].fops_table->close(fd); 
    }
    return -1; 
}

int32_t getargs(uint8_t * buf, int32_t n){
    return 0;
}

int32_t vidmap(uint8_t ** screen_start){
    return 0;
}

int32_t set_handler(int32_t signum, void * handler_addr){
    return 0;
}

int32_t sigreturn (void){

    return 0;
}
