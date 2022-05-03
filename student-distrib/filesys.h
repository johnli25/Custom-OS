#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"
//#include "lib.h"

#define FILE_NAME_LENGTH 32
#define NUM_DATA_BLOCKS 1023
#define NUM_OF_FILES 63
#define KB_4 4096
#define MAX_ARG_SIZE 128

typedef struct dentry_struct {
    int8_t fileName[FILE_NAME_LENGTH];
    uint32_t file_type;
    uint32_t inode; 
    uint8_t reserved[24]; //MAGIC #: 24 represents # of Bytes reserved
} __attribute__((packed)) dentry_t;

typedef struct inode_struct{
    uint32_t length;
    uint32_t data_block[NUM_DATA_BLOCKS];
} __attribute__((packed)) inode_t;

typedef struct bootBlock{
    uint32_t numberOfDentries;
    uint32_t numberOfInodes;
    uint32_t numberOfDataBlocks;
    uint8_t reserved[52]; //MAGIC #: 52 represents # of Bytes reserved
    dentry_t dentry_list[NUM_OF_FILES];
} __attribute__((packed)) bootBlock_t;

typedef struct dataBlock {
    uint8_t data[KB_4]; //how many BYTES OF DATA is contained within each block!
} __attribute__((packed)) dataBlock_t;

typedef struct fops{
    int32_t (*open)(const uint8_t *);
    int32_t (*close)(int32_t);
    int32_t (*read)(int32_t, void *, int);
    int32_t (*write)(int32_t, const void *, int);
} fops_t;

typedef struct fd_info{
    fops_t * fops_table; // FOPS jump table
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags;
} __attribute__((packed)) fd_info_t;

typedef struct pcb{
    unsigned int pid;
    // struct pcb_t* pcb_parent;
    unsigned int parent_id;
    fd_info_t myINFO[8];
    uint32_t saved_esp;
    uint32_t saved_ebp;
    uint16_t ss0; 
    uint16_t esp0;
    unsigned int active;
    uint8_t arguments[MAX_ARG_SIZE];

} __attribute__((packed)) pcb_t;

bootBlock_t * bootBlock;
dataBlock_t * data_block_initial_ptr;
inode_t * inode_initial_ptr;

//fd_info_t fd_array[8]; 

void initialize_filesys(uint32_t * addr);

extern int32_t dir_open(const uint8_t *filename);
extern int32_t dir_close(int32_t fd);
extern int32_t dir_read(int32_t fd, void *buf, int n);
extern int32_t dir_write(int32_t fd, const void *buf, int nbytes);

extern int32_t file_open(const uint8_t *filename);
extern int32_t file_close(int32_t fd);
extern int32_t file_read(int32_t fd, void *buf, int nbytes);
extern int32_t file_write(int32_t fd, const void *buf, int nbytes);

extern int32_t read_dentry_name (const uint8_t* file_name, dentry_t * dentry);
extern int32_t read_dentry_index (uint32_t index, dentry_t* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif


















