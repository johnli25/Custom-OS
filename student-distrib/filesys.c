#include "filesys.h"
#include "types.h"
#include "lib.h"
#include "systemCalls.h"

/*Checkpoint 3.2 directory functions*/

/* 
 *initialize_filesys
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: init bootblock
 */
void initialize_filesys(uint32_t * addr)
{
    bootBlock = (bootBlock_t *)addr;
}

/* 
 *dir_open
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t dir_open(const uint8_t *filename)
{

    return 0;
}

/* 
 *dir_close
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t dir_close(int32_t fd)
{

    return 0;
}

/* 
 *dir_read
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: bytes read
 *   SIDE EFFECTS: reads from file memory
 */
int32_t dir_read(int32_t fd, void *buf, int n)
{
    if (!buf)
        return ERRORRETURN;

    int j, bytes_read;
    bytes_read = 0;
    dentry_t myDentry;

    int cur_process_id = getProgNum();
    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (cur_process_id + 1))); //Add 1 to get to the next one

    int file_check = read_dentry_index(mypcb->myINFO[fd].file_position, &myDentry);
    if (file_check == -1)
        return ERRORRETURN;

    int length = strlen(/*bootBlock->dentry_list[n]*/myDentry.fileName); //removed + 1
    if (length > FILE_NAME_LENGTH)
        length = FILE_NAME_LENGTH;
    for (j = 0; j < length; j++)
    {
        ((int8_t *)(buf))[bytes_read] = /*bootBlock->dentry_list[n]*/myDentry.fileName[j];
        bytes_read++;
    }
    mypcb->myINFO[fd].file_position++; //from OH: why do I have to increment file_posi by 1???
    
    return bytes_read;
}

/* 
 *dir_write
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: reads from file memory
 */
int32_t dir_write(int32_t fd, const void *buf, int nbytes)
{

    return -1; // read-only file system, so return -1 automatically and uncondiionally, return error 
}

/* 
 *file_open
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: reads from file memory
 */
/*Checkpoint 3.2 file functions*/
int32_t file_open(const uint8_t *filename)
{

    return 0;
}

/* 
 *file_close
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: reads from file memory
 */
int32_t file_close(int32_t fd)
{
    return 0;
}

/* 
 *file_read
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: reads from file memory
 */
int32_t file_read(int32_t fd, void *buf, int nbytes)
{
    if (!buf) //do I need to check? 
        return -1; //Magic: return error

    int cur_process_id = getProgNum();
    pcb_t * mypcb = (pcb_t *)(EIGHTMB - (EIGHTKB * (cur_process_id + 1))); //what's the hardcoded numerical addr?
    int32_t n = read_data(mypcb->myINFO[fd].inode, mypcb->myINFO[fd].file_position, (uint8_t *)buf, nbytes); //why is setting to a var => page fault? 

    if (-1 == read_data(mypcb->myINFO[fd].inode, mypcb->myINFO[fd].file_position, (uint8_t *)buf, nbytes)) //checks if error
        return -1; //Magic: return error

    mypcb->myINFO[fd].file_position += n; //update file position
    return n;
}

/* 
 *file_write
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: reads from file memory
 */
int32_t file_write(int32_t fd, const void *buf, int nbytes)
{

    return -1; // read-only file system, so return -1 automatically and uncondiionally
}

/* 
 *read_dentry_name
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 or -1
 *   SIDE EFFECTS: reads from file memory: inodes, dentries, bootblock, etc.
 */
int32_t read_dentry_name(const uint8_t *file_name, dentry_t *dentry)
{
    int i;
    if (file_name == NULL)
        return -1;//Magic: return error
    // int dentry_fname_len = strlen(bootBlock->dentry_list[i].fileName);
    int arg_fname_len = strlen((int8_t *)file_name);
    if (arg_fname_len > FILE_NAME_LENGTH) // if it's too long...
        return -1;//Magic: return error

    for (i = 0; i < bootBlock->numberOfDentries; i++)
    {
            if (strncmp((int8_t *)bootBlock->dentry_list[i].fileName, (int8_t *)file_name, 32) == 0) //Magic Number 32: Used for offset
            {
                //*dentry = bootBlock->dentry_list[i];
                strcpy(dentry->fileName, bootBlock->dentry_list[i].fileName);
                dentry->inode = bootBlock->dentry_list[i].inode;
                dentry->file_type = bootBlock->dentry_list[i].file_type;
                return 0;
            }
    }
    return ERRORRETURN;
}

/* 
 *read_dentry_index
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 or -1
 *   SIDE EFFECTS: reads from file memory: inodes, dentries, bootblock, etc.
 */
int32_t read_dentry_index(uint32_t index, dentry_t *dentry)
{
    if (dentry == NULL)
        return -1; //Magic: return error
    // printf(" # of inodes: %d \n", bootBlock->numberOfInodes); the # is currently 64 lol
    if (index < 0 || index >= bootBlock->numberOfInodes)
        return -1; //Magic: return error

    //if (index == bootBlock->dentry_list[index].inode) // this necessary?
    //dentry = &(bootBlock->dentry_list[index]);
    dentry->file_type = bootBlock->dentry_list[index].file_type;
    dentry->inode = bootBlock->dentry_list[index].inode;
    //dentry->fileName = bootBlock->dentry_list[index].fileName;
    int i;
    for (i = 0; i < FILE_NAME_LENGTH; i++){
        dentry->fileName[i] = bootBlock->dentry_list[index].fileName[i];
    }
    return 0;
}

/* 
 *read_dentry_name
 *   DESCRIPTION: Used to as a getter
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: bytes read
 *   SIDE EFFECTS: reads from file memory: inodes, dentries, bootblock, etc.
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length)
{
    int i, alt_i;
    alt_i = 0;
    int32_t count = 0;
    if (buf == NULL)
        return -1;
    inode_initial_ptr = (inode_t *)bootBlock + 1;
    inode_t *inode_ptr = (inode_t *)(inode_initial_ptr + inode); // declare + initialize inode_ptr

    uint32_t inode_db_idx = offset / KB_4;
    uint32_t data_block_idx = inode_ptr->data_block[offset / KB_4]; // calculate data block integer
    uint32_t data_block_offset = offset % KB_4;                     // calculate offset WITHINT data block now
    
    data_block_initial_ptr = (dataBlock_t *)(bootBlock + 1 + bootBlock->numberOfInodes);
    dataBlock_t * data_block_ptr = data_block_initial_ptr + data_block_idx;
    for (i = 0; i < length; i++)
    {
        if (inode_ptr->length == offset + i)
            break;
        if (data_block_offset + alt_i >= KB_4) //if it needs to overflow..
        {
            data_block_offset = 0;
            alt_i = 0;
            inode_db_idx++; //calculate new inode_db_idx in order to get...
            data_block_idx = inode_ptr->data_block[inode_db_idx]; //new data block idx
            data_block_ptr = data_block_initial_ptr + data_block_idx;
        }
        buf[i] = data_block_ptr->data[data_block_offset + alt_i];
        //putc(buf[i]);
        count++; //i == COUNT
        alt_i++; 
    }
    return count; // return # of bytes read AKA # of bytes placed in buffer
}
