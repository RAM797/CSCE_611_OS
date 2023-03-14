/*
     File        : file_system.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File System class.
                   Has support for numerical file identifiers.
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "console.H"
#include "file_system.H"
#include "utils.H"
/*--------------------------------------------------------------------------*/
/* CLASS Inode */
/*--------------------------------------------------------------------------*/

/* You may need to add a few functions, for example to help read and store 
   inodes from and to disk. */

/*--------------------------------------------------------------------------*/
/* CLASS FileSystem */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
    disk = NULL;
    size = 0;
    inodes = new Inode[MAX_INODES];
    Console::puts("In file system constructor.\n");
    //assert(false);
}

FileSystem::~FileSystem() {
    Console::puts("unmounting file system\n");
    /* Make sure that the inode list and the free list are saved. */
    disk->write(1,free_blocks); 
    unsigned char* buf = (unsigned char*) inodes;
    disk->write(0,buf);
}


/*--------------------------------------------------------------------------*/
/* FILE SYSTEM FUNCTIONS */
/*--------------------------------------------------------------------------*/


bool FileSystem::Mount(SimpleDisk * _disk) {
    Console::puts("mounting file system from disk\n");

    /* Here you read the inode list and the free list into memory */
    disk = _disk;
    unsigned char* tmp;
    _disk->read(1,free_blocks);
    _disk->read(0,tmp);
    inodes = (Inode *) tmp;
    return true;
    // assert(false);
}

bool FileSystem::Format(SimpleDisk * _disk, unsigned int _size) { // static!
    Console::puts("formatting disk\n");
    /* Here you populate the disk with an initialized (probably empty) inode list
       and a free list. Make sure that blocks used for the inodes and for the free list
       are marked as used, otherwise they may get overwritten. */
    n_free = _size / SimpleDisk::BLOCK_SIZE;
    free_blocks = new unsigned char[n_free];
    free_blocks[0] = 'u' ;// inode map
    free_blocks[1] = 'u' ;// free block map
    for(int i = 2; i < n_free; i++)
        free_blocks[i] = 'f';

    for (int i = 0; i < MAX_INODES; i++)
        {
            inodes[i].is_free = true;
        }
    
    _disk->write(0, (unsigned char *)inodes);
    _disk->write(1, free_blocks);
    return true;
    // assert(false);
}

Inode* FileSystem::LookupFile(int _file_id) {
    Console::puts("looking up file with id = "); Console::puti(_file_id); Console::puts("\n");
    /* Here you go through the inode list to find the file. */
    for(int i=0; i < MAX_INODES; i++)
    {
        if (inodes[i].id == _file_id)
        {
            return &inodes[i];
        }

    }
    return NULL;
    // assert(false);
}

long   FileSystem::get_free_block()
{
    for(int i=2; i< n_free; i++)
    {
        if(free_blocks[i] == 'f')
        {
            return i;
        }
    }
    return -1;
}

long FileSystem::get_free_inode()
{
    for(int i=0; i< MAX_INODES ; i++)
    {
        if(inodes[i].is_free)
        {
            return i;
        }
    }
    return -1;
}


bool FileSystem::CreateFile(int _file_id) {
    Console::puts("creating file with id:"); Console::puti(_file_id); Console::puts("\n");
    /* Here you check if the file exists already. If so, throw an error.
       Then get yourself a free inode and initialize all the data needed for the
       new file. After this function there will be a new file on disk. */

    if(LookupFile(_file_id) == NULL)
    {
        long inode_ind = get_free_inode();
        long free_block_ind = get_free_block();
        
        inodes[inode_ind].id = _file_id;
        inodes[inode_ind].block_id = free_block_ind;
        inodes[inode_ind].is_free = false;
        inodes[inode_ind].f_size = 0; 

        free_blocks[free_block_ind] = 'u';
        return true;
    }
    else{
         Console::puti(_file_id);
         Console::puts(" already exists\n");
         return false;
    }
    // assert(false);
}

bool FileSystem::DeleteFile(int _file_id) {
    Console::puts("deleting file with id:"); Console::puti(_file_id); Console::puts("\n");
    Inode* del_node = LookupFile(_file_id);
    if(del_node != NULL)
    {
        long block_id = del_node->block_id;
        free_blocks[block_id] = 'f';

        del_node->id = NULL;
        del_node->block_id = NULL;
        del_node->is_free = true;
        del_node->f_size = 0;
        
        disk->write(block_id, NULL); // clearing the data on disk
        return true;
    }
    return false;
    /* First, check if the file exists. If not, throw an error. 
       Then free all blocks that belong to the file and delete/invalidate 
       (depending on your implementation of the inode list) the inode. */
    
}
