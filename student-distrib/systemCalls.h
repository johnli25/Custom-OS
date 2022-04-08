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

int32_t halt(unint8_t status);
int32_t execute(const uint8_t* command);

typedef struct pcb{
    unsigned int pid;
    unsigned int parent_id;
    unsigned int fileDescriptor;
    unsigned int saved_esp;
    unsigned int saved_ebp;
    unsigned int active;
} __attribute__((packed)) pcb_t;




