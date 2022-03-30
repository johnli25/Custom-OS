int32_t execute (const uint8_t* command);

typedef struct pcb_t{
    
    unsigned int pid;
    unsigned int parent_id;
    unsigned int fileDescriptor;
    unsigned int saved_esp;
    unsigned int saved_ebp;
    unsigned int active;
}

