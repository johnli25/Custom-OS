#include "systemCalls.h"
#include "filesys.h"
#include "paging.c"

int programNumber[6] = {0,0,0,0,0,0}; 

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
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.base_address = 2; //not sure if this correct? ((processNum * FOUR_MB) + EIGHT_MB)

    asm volatile (
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"  
        : 
        : 
        :"%eax" //saved "clobbered (?)" regs 
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
    read_data(myDentry.inode, 0, ELFBUFFER,  4); //why 4 and not 3?

    // uint8_t * ELFSTRING = " ELF"; //added a space
    // int32_t equalCheck = strncmp(ELFBUFFER, ELFSTRING, 4);

    // if(equalCheck != 0){ //checks if it is an ELF
    //     return -1; //FAILED TEST
    // }
    if (ELFBUFFER[0] != MAGIC0 || ELFBUFFER[1] != MAGIC1 ||
        ELFBUFFER[2] != MAGIC2 || ELFBUFFER[3] != MAGIC3) //ELF string beginning check
        return -1;

    //  Physical memory starts at
    // 8MB + (process number * 4MB)


    int physicalMemNum = EIGHTMB + (myProgramNumber * FOURMB); //from the slides 

    // PCB = 8MB - (8KB * (ProcessNumber + 1));
    pcb_t * mypcb = EIGHTMB - (EIGHTKB * (myProgramNumber + 1));
    // pcb-> pid = myprocessnumber;
   
}