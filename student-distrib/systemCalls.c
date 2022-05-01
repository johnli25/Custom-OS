#include "systemCalls.h"
#include "keyboard.h"
#include "paging.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"
#include "rtc.h"
#include "filesys.h"

static int program_arr[9] = {0,0,0,0,0,0,0,0,0};  
static int currentProgramNumber = 0;

static int vp_flag = 0; //vid paging (vp) flag 

int getProgNum(){
    return currentProgramNumber;
}
/* do_nothing_r(int32_t theres, void * nothing, int lol)
 *   DESCRIPTION: function to do nothing read for fops_none
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: placeholder
 */
int32_t do_nothing_r(int32_t theres, void * nothing, int lol){
    return ERRORRETURN;
}

/* do_nothing_w(int32_t theres, void * nothing, int lol)
 *   DESCRIPTION: function to do nothing write for fops_none
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: placeholder
 */
int32_t do_nothing_w(int32_t theres, const void * nothing, int lol){
    return ERRORRETURN;
}

/* do_nothing_open(int32_t theres, void * nothing, int lol)
 *   DESCRIPTION: function to do nothing open for fops_none
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: placeholder
 */
int32_t do_nothing_open(const uint8_t * none){
    return ERRORRETURN;
}

/* do_nothing_close(int32_t theres, void * nothing, int lol)
 *   DESCRIPTION: function to do nothing close for fops_none
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: placeholder
 */
int32_t do_nothing_close(int32_t none){
    return ERRORRETURN;
}

fops_t stdin = {terminal_open, terminal_close, terminal_read, do_nothing_w};
fops_t stdout = {terminal_open, terminal_close, do_nothing_r, terminal_write};
fops_t dir_fops = {dir_open, dir_close, dir_read, dir_write};
fops_t file_fops = {file_open, file_close, file_read, file_write};
fops_t rtc_fops = {open_RTC, close_RTC, read_RTC, write_RTC};
fops_t fops_none = {do_nothing_open, do_nothing_close, do_nothing_r, do_nothing_w};
// fops_t stdin;
// stdin.open = (int32_t)terminal_open;


/* paging_helper(int processNum)
 *   DESCRIPTION: sets up paging for execute
 *   INPUTS: processNum
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: alters paging at the program mem start
 */
void paging_helper(int processNum){
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.p = 1; // Magic Num: sets as present
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.r_w = 1; // Magic Num: sets as present
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.u_s = 1; // Magic Num: sets as present
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pwt = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pcd = 1; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.a = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.d = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.ps = 1; //page size = 1 for kernel
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.g = 0; //must enable global pages
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.avl_3bits = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pat = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.base_addr2 = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.rsvd = 0; //always set to 1
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.base_address = 2 + processNum; //not sure if this correct? MagicNum: ((processNum * FOUR_MB) + EIGHT_MB)

    asm volatile (
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"  
        : 
        : 
        :"%eax" //saved "clobbered" regs 
    );
}

/* paging_unhelper(int processNum)
 *   DESCRIPTION: restores paging for halt function
 *   INPUTS: processNum
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: alters paging at the program mem start
 */
void paging_unhelper(int processNum){
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.p = 1; // Magic Num: sets as present
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.r_w = 1; // Magic Num: sets as present
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.u_s = 1; // Magic Num: sets as present
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pwt = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pcd = 1; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.a = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.d = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.ps = 1; //page size = 1 for kernel
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.g = 0; //must enable global pages
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.avl_3bits = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.pat = 0; // Magic Num: sets as unpresent
    page_dir[POGRAM_MEM_START]._PDE_kernel_4MB.base_addr2 = 0; // Magic Num: sets as unpresent
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

void terminalPageSwitch(int newTerminal){
    //something currTerm
    if (currTerm == newTerminal)
        page_tab[paging_vidmem >> 12].base_address = (paging_vidmem >> 12);
    else
        page_tab[paging_vidmem >> 12].base_address = (paging_vidmem >> 12) + newTerminal + 1;
    
    asm volatile ( //flush tlb
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"  
        : 
        : 
        :"%eax" //saved "clobbered" regs 
    );
}

void vid_paging_helper(){
    video_pt[0].p = 1; // Magic Num: sets as present
    video_pt[0].r_w = 1; //set r_w bit to 1
    video_pt[0].u_s = 1; //set user privileges for video page table ON
    video_pt[0].pwt = 0; // Magic Num: sets as unpresent
    video_pt[0].pcd = 0; // Magic Num: sets as unpresent
    video_pt[0].a = 0; // Magic Num: sets as unpresent
    video_pt[0].d = 0; // Magic Num: sets as unpresent
    video_pt[0].pat = 0; // Magic Num: sets as unpresent
    video_pt[0].g = 0; // Magic Num: sets as unpresent
    video_pt[0].avl_3bits = 0;   // Magic Num: sets as unpresent
    video_pt[0].base_address = paging_vidmem >> DATA_ALIGN_SHIFT; //is this correct?

    page_dir[USER_VIDMEM]._PDE_regular.p = 1; // Magic Num: sets as present
    page_dir[USER_VIDMEM]._PDE_regular.r_w = 1; // Magic Num: sets as present
    page_dir[USER_VIDMEM]._PDE_regular.u_s = 1;//we are using a USER page for vid map //function that flips 1/0 
    page_dir[USER_VIDMEM]._PDE_regular.pwt = 0; // Magic Num: sets as unpresent
    page_dir[USER_VIDMEM]._PDE_regular.pcd = 0; // Magic Num: sets as unpresent
    page_dir[USER_VIDMEM]._PDE_regular.a = 0; // Magic Num: sets as unpresent
    page_dir[USER_VIDMEM]._PDE_regular.avl_1bit = 0; // Magic Num: sets as unpresent
    page_dir[USER_VIDMEM]._PDE_regular.ps = 0; //page size = 0 for page table 
    page_dir[USER_VIDMEM]._PDE_regular.avl_3bits = 0; // Magic Num: sets as unpresent
    page_dir[USER_VIDMEM]._PDE_regular.base_address = (unsigned int)video_pt >> DATA_ALIGN_SHIFT; //12 is the page-aligned/shift offset that contains the attributes for 4 KB pages (which I shift or offset away)

    asm volatile ( //flush tlb
        "movl %%cr3, %%eax;"
        "movl %%eax, %%cr3;"  
        : 
        : 
        :"%eax" //saved "clobbered" regs 
    );
    return;
}

/*execute (const uint8_t* command)
 *   DESCRIPTION: executes the passed in command 
 *   INPUTS: command
 *   OUTPUTS: none
 *   RETURN VALUE: ret which is populated in asm code
 *   SIDE EFFECTS: executes the command that is passed in by user.
 */
int32_t execute (const uint8_t* command){
    int32_t ret;
    //currTerm = 0;
    if (0 == strncmp((int8_t *)command, (int8_t*)("exit\n"), 5)){ //Magic Num: 5 Used for the correct function call
        ret = 0;
        halt(ret);
        return ret; 
        //strcpy((int8_t *)command, (int8_t *)("exit"));
    }
    if (!command)
        return ERRORRETURN; 

    //command is a string, have to parge (ex: shell)     
    //parse through the string - past 391OS> get after white space 
    int index = 0;
    while(command[index] == ' '){ //checks if a space
        index++;
    }

    uint8_t buffer[128]; //MAGIC NUM: 128 - size of the keyboard/terminal array 
    uint8_t arg_buf[128]; //MAGIC NUM: 128 - size of the keyboard/terminal array 

    int j; 
    for (j = 0; j<128; j++ ){ //MAGIC NUM: 128 - size of the keyboard/terminal array 
        buffer[j] = '\0'; //Magic Num NULL
        arg_buf[j] = '\0'; //MAgic Num NULL
    }
    int bufIndex = 0;
    while(command[index] != ' ' && command[index] != '\n'){ //checks if a space or a new line
        buffer[bufIndex] = command[index];
        index++;
        bufIndex++;
    }  //buffer is the command without all white space ^

    dentry_t myDentry;
    /*IMPORTANT: check and buffer read exec file name and store it in buffer + myDentry*/
    int check = read_dentry_name(buffer, &myDentry); 
    if(check == ERRORRETURN){
        return ERRORRETURN; //FAILED TEST
    }

    if (myDentry.file_type != 2){ //MAGIC NUM 2 - checks if valid type (executable)
        return ERRORRETURN; //FAILED TEST 
    }

    uint8_t ELFBUFFER[10]; //Magic num - holds size of the ELF buffer string 
    read_data(myDentry.inode, 0, ELFBUFFER, 4); //Magic Num - Grabs the ELF info of first 4 characters 

    if (ELFBUFFER[0] != MAGIC0 || ELFBUFFER[1] != MAGIC1 ||
        ELFBUFFER[2] != MAGIC2 || ELFBUFFER[3] != MAGIC3) //Magic Num: ELF string beginning check
        return ERRORRETURN;

    int myProgramNumber = 0; //starts off as zero
    for(myProgramNumber = 0; myProgramNumber < 9; myProgramNumber++){ //magic num: 3 is the max # of processes/files
        if(program_arr[myProgramNumber] == 0){ //checks if free 
            program_arr[myProgramNumber] = 1; //sets to filled
            //multi_terms[currTerm].shell_cnt++; //possibly deprecated-don't need anymore?
            break;
        }
        if(myProgramNumber == 8){ //MAGIC #: 2 = MAX NUMBER OF PROCESSES - 1 AKA we reached end of iteration and they were all filled (= 1)
            return ERRORRETURN; //all of the others are filled
        }
    }

    vp_flag = 0;

    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (myProgramNumber + 1))); //what's the hardcoded numerical addr?
    multi_terms[currTerm].curr_proc = mypcb;
    
    if(0 == strncmp((int8_t *)buffer, (int8_t*)("shell"), 5) ||
        0 == strncmp((int8_t *)buffer, (int8_t*)("hello"), 5)) // equal to shell or hello
       multi_terms[currTerm].progRunning = 0; //program not running on term
    else
        multi_terms[currTerm].progRunning = 1; //program running on term

    int arg_i;
    for (arg_i = 0; arg_i < MAX_ARG_SIZE; arg_i++){
        mypcb->arguments[arg_i] = '\0';//initialize pcb->args all to '\0' - NULL
    }
    index++;
    arg_i = 0;
    while (command[index] != '\0' && command[index] != ' ' && command[index] != '\n'){ //Magic Num: NULL or NewLine
//        if (command[index] != ' ')
        mypcb->arguments[arg_i] = command[index];
        index++;
        arg_i++;
    }  

    uint8_t POE_buf[4]; //Magic Num: size of POE buf to get the first 4 chars 
    read_data(myDentry.inode, PO3_OF_ENTRY, POE_buf, 4); // Magic Num 4 is total size of bytes 24-27
    uint32_t pt_of_entry = *((uint32_t*)POE_buf);
    //uint8_t * physicalMemNum = (uint8_t*) (EIGHTMB + (myProgramNumber * FOURMB));// Physical memory starts at 8MB + (process number * 4MB)

    //map to virtual mem:zer0padded by 22
    paging_helper(myProgramNumber);
    read_data(myDentry.inode, 0, (unsigned char *)PROG_START_VIRTUAL_ADDR,  FOURMB); //load file into memory 

    program_arr[myProgramNumber] = 1; //Magic Num sets as present

    //save user program bookkeeping info
    mypcb-> pid = myProgramNumber;
    mypcb -> parent_id = currentProgramNumber;
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

    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"
        : "=r"(mypcb->saved_esp), "=r"(mypcb->saved_ebp)
    );

    //save kernel stack bookkeeping info
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHTMB - (EIGHTKB * (myProgramNumber /*+ 1*/))) - 4; // magic -4: used to get the correct esp calculation

    mypcb -> active = 1;
    
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
        "movl %%eax, %4;" //move return value from register into 
        :
        : "r"(USER_DS), "r"(MB_132 - 4), "r"(USER_CS), "r"(pt_of_entry), "r"(ret)
        : "eax" //clobbered regs
    );
    printf("%d \n", currentProgramNumber);
    return ret;
}

/* halt(uint8_t status)
 *   DESCRIPTION: halts the process
 *   INPUTS: status
 *   OUTPUTS: none
 *   RETURN VALUE: haltReturn_stat
 *   SIDE EFFECTS: restores all processes in execute
 */
int32_t halt(uint8_t status){
    //first, grab esp and ebp pointers from the pcb 
    pcb_t * cHiLdPcB = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1))); //may need this again - may not + 1
    pcb_t * parentPcb = (pcb_t *)(EIGHTMB - (EIGHTKB * ((cHiLdPcB -> parent_id) + 1))); //may need this again - may not + 1
    int32_t haltReturn_stat = (int32_t)(status); //our return value, what do we return? this added as asm return
    if (haltReturn_stat == 255) //Magic Num: 255 - Checks if it is the last 
        haltReturn_stat = 256; //Magic Num: 266 - Sets to the last possible value
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (EIGHTMB - (EIGHTKB * (parentPcb->pid /*+ 1*/))) - 4; // magic -4: used to get the correct esp calculation

    int i;
    //close all opened file descriptors
    for (i = 2; i < 8; i++){ //used to instantiate the indeces from 2-7 
        if (cHiLdPcB->myINFO[i].flags){
            general_close(i);
            cHiLdPcB->myINFO[i].flags = 0; //flags to 0 
            cHiLdPcB -> myINFO[i].inode = 0; //inodes to 0
            cHiLdPcB -> myINFO[i].file_position = 0; //file position to 0
            cHiLdPcB -> myINFO[i].fops_table = &fops_none; //file position to 0
        }
    }

    program_arr[cHiLdPcB->pid] = 0; //sets to unpresent
    multi_terms[currTerm].progRunning = 0; //terminal not running program anymore
    
    // multi_terms[currTerm].shell_cnt--;
    // reload a new shell if childpcb's pid = childpcb's parent id
    if (currentProgramNumber == cHiLdPcB->parent_id || currentProgramNumber < 3) //if currentProgNum less than/within the 3 base shell programs
        execute((uint8_t*)"shell"); // executes shell 
    currentProgramNumber = cHiLdPcB->parent_id;    
    // parent process done - now paging 
    // parent paging (unpaging) - paging user program (paging.c?) 8 + (id * 4mb) or with flags into directory, flush

    cHiLdPcB -> active = 0; //unused? 
    parentPcb -> active = 1; //set active bits for comprehension/reference

    paging_unhelper(cHiLdPcB->parent_id); // - flushes TLB as well 
    if (vp_flag == 1) 
        vid_paging_helper();

    // assembluuuuu linkeage asm - for 
    // halt needs to jump to th eend of execute asm volitile :"-.globl LABEL" - able to return to this label in dif asm volatiles 
    // halt return value
     asm volatile( 
        "movl %0, %%esp;" //esp contains saved_esp
        "movl %1, %%ebp;" //ebp contains saved_ebp
        //check status
        // "cmp $1, %2;"
        // "jne myValid;"
        // "movl $1, %%eax;"
        //"jmp executeEnd;"
        "Valid:"
        "andl $0, %%eax;"
        "movl %2, %%eax;" //eax contains return value
        //"jmp executeContinue;"
        "leave;"
        "ret;"
        :
        : "r"(cHiLdPcB -> saved_esp), "r"(cHiLdPcB -> saved_ebp), "r"(haltReturn_stat)
        :"%eax" //saved "clobbered" regs 
    );

    return haltReturn_stat;
}

/* general_read(int32_t fd, void * buf, int32_t n)
 *   DESCRIPTION: reads n bytes from fd into buf
 *   INPUTS: status
 *   OUTPUTS: none
 *   RETURN VALUE: returns the read value or -1 if no read
 *   SIDE EFFECTS: none
 */
int32_t general_read(int32_t fd, void * buf, int32_t n){
    if (fd>=0 && fd < 8){ //Magic Nums: checks if it is in between 0 and 8 - valid 
        pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));
        if (mypcb->myINFO[fd].flags)
            return mypcb->myINFO[fd].fops_table->read(fd, buf, n); 
    }
    return ERRORRETURN; 
}

/* general_write(int32_t fd, void * buf, int32_t n)
 *   DESCRIPTION: writes n bytes from fd into buf
 *   INPUTS: status
 *   OUTPUTS: none
 *   RETURN VALUE: returns the write value or -1 if no write
 *   SIDE EFFECTS: none
 */
int32_t general_write(int32_t fd, const  void * buf, int32_t n){
    //n = 198;
    //198 is the number of chars that the frame0.txt file consists of
    if ( fd>=0 && fd < 8){ //Magic Nums: checks if it is in between 0 and 8 - valid 
        pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));
        if (mypcb->myINFO[fd].flags) //== 1
            return mypcb->myINFO[fd].fops_table->write(fd, buf, n); 
    }
    return ERRORRETURN; 
}

/* general_open(const uint8_t * filename)
 *   DESCRIPTION: opens the file
 *   INPUTS: const uint8_t * filename
 *   OUTPUTS: none
 *   RETURN VALUE: returns -1 if bad open, otherwise index of fd array
 *   SIDE EFFECTS: none
 */
int32_t general_open(const uint8_t * filename){
    dentry_t d;
    int i = 2; //magic num: sets it for an index for the loop 
    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));

    if (read_dentry_name(filename, &d) != 0)
        return ERRORRETURN;

    if (!filename)
        return ERRORRETURN;

    //read_dentry_name(filename, &d);
    if (d.file_type==0){ //rtc
        open_RTC (filename); 
        while (i < 8){ //8 is max size idx of fd array
            if (mypcb->myINFO[i].flags == 0) { //checks if unpresent
                mypcb->myINFO[i].flags = 1; //sets to present
                mypcb->myINFO[i].file_position = 0; //sets to unpresent
                mypcb->myINFO[i].inode = 0; //sets to unpresent
                mypcb->myINFO[i].fops_table = &rtc_fops;
                return i;
            }
            i++;
        }
    }
    if (d.file_type==1){ //dir
        dir_open(filename);
        i = 2; //magic num: checks for indexes of info
        while (i < 8){ //8 is max size idx of fd array
            if (mypcb->myINFO[i].flags == 0) { //checks if unpresent
                mypcb->myINFO[i].flags = 1; //sets to present 
                mypcb->myINFO[i].file_position = 0; //sets to unpresent
                mypcb->myINFO[i].inode = 0; //sets to unpresent
                mypcb->myINFO[i].fops_table = &dir_fops;
                return i; 
            }
            i++;
        }
    }
    if (d.file_type==2){ //checks if 2 - correct type 
        file_open(filename); 
        i = 2; //sets index as 2
        while (i < 8){
            if (mypcb->myINFO[i].flags == 0) { //checks if unpresent
                mypcb->myINFO[i].flags = 1; //sets as present
                mypcb->myINFO[i].file_position = 0; //sets as unpresent
                mypcb->myINFO[i].inode = d.inode; 
                mypcb->myINFO[i].fops_table = &file_fops;
                return i;
            }
            i++;
        }
    }
    return ERRORRETURN; //can't find
}

/* general_close(int32_t fd)
 *   DESCRIPTION: closes the file
 *   INPUTS: int32_t fd
 *   OUTPUTS: none
 *   RETURN VALUE: returns the close value or -1 if no close
 *   SIDE EFFECTS: none
 */
int32_t general_close(int32_t fd){
    if ( fd>=2 && fd < 8){ //checks if valid index
        pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));
        if (mypcb->myINFO[fd].flags){
            mypcb->myINFO[fd].flags = 0;
            /*is below necessary? */
            // mypcb->myINFO[fd].file_position = 0; //sets as unpresent
            // mypcb->myINFO[fd].inode = 0; 
            // mypcb->myINFO[fd].fops_table = &fops_none;
            return mypcb->myINFO[fd].fops_table->close(fd); 
        }
    }
    return ERRORRETURN; 
}

/* getargs(uint8_t * buf, int32_t n)
 *   DESCRIPTION: gets arguements from buf
 *   INPUTS: uint8_t * buf, int32_t n
 *   OUTPUTS: none
 *   RETURN VALUE: returns 0 if success
 *   SIDE EFFECTS: none
 */
int32_t getargs(uint8_t * buf, int32_t n){
    //error checks
    if (buf == NULL)
        return ERRORRETURN;
    
    if (n < MAX_ARG_SIZE)
        return ERRORRETURN;
    
    //initialize PCB
    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (currentProgramNumber + 1)));
    if (n == 0 || mypcb->arguments[0] == '\0') //if no args in input args/buffer - NULL
        return ERRORRETURN;

    /*populate buffer with mypcb->args*/
    int i;
    for (i = 0; i < n; i++){
        buf[i] = mypcb->arguments[i];
    }
    buf[n-1] = '\0'; //set last char in buffer to null
    return 0;
}

/* vidmap(uint8_t ** screen_start)
 *   DESCRIPTION: maps the text mode video memory into user space
 *   INPUTS: uint8_t ** screen_start
 *   OUTPUTS: none
 *   RETURN VALUE: returns 0 if success
 *   SIDE EFFECTS: none
 */
int32_t vidmap(uint8_t ** screen_start){
    if (screen_start == NULL || (uint32_t)screen_start < MB128_START || (uint32_t)screen_start >= MB_132)
        return ERRORRETURN;

    vp_flag = 1; //sets as 1
    vid_paging_helper();
    *screen_start = (uint8_t *)MB_132;
    return 0;
    //return MB_132; //return 132 (MB)
}

/* general_close(int32_t fd)
 *   DESCRIPTION: closes the file
 *   INPUTS: int32_t fd
 *   OUTPUTS: none
 *   RETURN VALUE: returns the close value or -1 if no close
 *   SIDE EFFECTS: none
 */
int32_t set_handler(int32_t signum, void * handler_addr){
    return 0;
}

/* general_close(int32_t fd)
 *   DESCRIPTION: closes the file
 *   INPUTS: int32_t fd
 *   OUTPUTS: none
 *   RETURN VALUE: returns the close value or -1 if no close
 *   SIDE EFFECTS: none
 */
int32_t sigreturn (void){

    return 0;
}
