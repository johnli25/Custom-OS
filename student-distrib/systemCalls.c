#include "systemCalls.h"
#include "keyboard.h"
#include "paging.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"

int32_t do_nothing_r(int32_t theres, void * nothing, int lol){
    return 0;
}

int32_t do_nothing_w(int32_t theres, const void * nothing, int lol){
    return 0;
}

fops_t stdin = {(int32_t)terminal_open, (int32_t)terminal_close, (int32_t)terminal_read, (int32_t)terminal_write};
fops_t stdout = {(int32_t)terminal_open, (int32_t)terminal_close, (int32_t)terminal_read, (int32_t)terminal_write};

int programNumber[6] = {0,0,0,0,0,0}; 
int currentProgramNumber = 0;

void paging_helper(int processNum){
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.p = 1;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.r_w = 1;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.u_s = 1;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pwt = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pcd = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.a = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.d = 0;
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.ps = 1; //page size = 1 for kernel
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.g = 0;
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

int32_t execute (const uint8_t* command){
    int myProgramNumber = 0;
    for(myProgramNumber = 0; myProgramNumber < 6; myProgramNumber++){
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

    //  Physical memory starts at
    // 8MB + (process number * 4MB)
    read_data(myDentry.inode, PO3_OF_ENTRY, ELFBUFFER, 4);
    //read_data(myDentry.inode, POINT_OF_ENTRY, ELFBUFFER, 4);
    uint32_t pt_of_entry = *((uint32_t*)ELFBUFFER);
    
    uint8_t * physicalMemNum = (uint8_t*) (EIGHTMB + (myProgramNumber * FOURMB)); //from the slides 

    //map to virtual mem
    //zerpadded by 22
    paging_helper(myProgramNumber);
    read_data(myDentry.inode, 0, (unsigned char *)VIRTUAL_ADDR, 1000000); //load file into memory
    
    //uint32_t address = 0;
    // int p = 0;
    // while (p < 4){
    //     address |= physicalMemNum[27 - p];
    //     address = address << 8; //may be wrong
    //     p++;
    // }

    // PCB = 8MB - (8KB * (ProcessNumber + 1)) - IS THIS IS USED FOR PAGING - VIRTUAL ADDRESS???????????
    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (myProgramNumber + 1))); //what's the hardcoded numerical addr?
    mypcb->pid = myProgramNumber;

    //save user program bookkeeping info
    mypcb-> pid = myProgramNumber;
    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(mypcb->saved_esp), "=r"(mypcb->saved_ebp)
    );

    mypcb-> parent_id = currentProgramNumber;
    mypcb-> myINFO[0].fops_table = stdin;
    mypcb-> myINFO[1].fops_table = stdout; 
    mypcb-> myINFO[0].flags = 1; //setting flag to 1
    mypcb-> myINFO[1].flags = 1; //setting flag to 1
    int i = 2;
    for (i = 2; i<8; i++){
        mypcb -> myINFO[i].flags = 0;
        //mypcb -> myINFO[i].fops_table = NULL;
    }

    //save kernel stack bookkeeping info
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHTMB - (EIGHTKB * (myProgramNumber /*+ 1*/))) - 4;
    programNumber[myProgramNumber] = 1;
    //HAVE TO DO MORE WITH fileDescriptor[0]: PUT IN THE ACTUAL JUMP TABLE STUFF
    // mypcb -> fileDescriptor[0] = myDentry.file_type; //corresponds to the file operations file pointer

    // mypcb -> fileDescriptor[1] = myDentry.inode; //corresponds to the iNode /number?
    // mypcb -> fileDescriptor[2] = 0; //corresponds to the file position
    // mypcb -> fileDescriptor[3] = 0; //corresponds to the flags
    currentProgramNumber = myProgramNumber; //update parent number
    
    /*context switch*/
    //enable interrupt on flags: or flags x200 in assembly w register
    asm volatile( 
        "pushl %0;"
        "pushl %1;" //push esp
        "pushfl;" //push eflags
        "popl %%eax;" //popping eflags into eax
        "orl $0x200, %%eax;" // trying to Or eax with 200
        "pushl %%eax;" // pushing eax which contains flags
        "pushl %2;" //push USER_CS
        "pushl %3;" //push eip = point of entry = 24 onto stack
        "iret;"
        :
        : "r"(USER_DS), "r"(MB_132 - 4), "r"(USER_CS), "r"(pt_of_entry)
    );
    return 0;
}

int32_t halt(uint8_t status){
    return 0;
}

int32_t general_read(int32_t fd, void * buf, int32_t n){
    return 0;
}

int32_t general_write(int32_t fd, const  void * buf, int32_t n){
    return 0;
}

int32_t general_open(const uint8_t * filename){
    return 0;
}

int32_t general_close(int32_t fd){
    return 0;
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
