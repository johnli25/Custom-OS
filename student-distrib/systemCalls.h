int32_t execute (const uint8_t* command);

#define EIGHTMB 0x0800000
#define FOURMB 0x400000
#define EIGHTKB 8192
#define MB128_START 0x8000000
#define MB128_OFFSET 0x0048000
#define POGRAM_MEM_START 128/4

#define MAGIC0 0x7F
#define MAGIC1 0x45
#define MAGIC2 0x4C
#define MAGIC3 0x46

typedef struct pcb{
    unsigned int pid;
    pcb_t* pcb_parent;
    unsigned int parent_id;
    fd_info_t myINFO[8];
    unsigned int saved_esp;
    unsigned int saved_ebp;
    unsigned int active;
} __attribute__((packed)) pcb_t;




