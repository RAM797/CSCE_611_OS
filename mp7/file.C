/*
     File        : file.C

     Author      : Riccardo Bettati
     Modified    : 2021/11/28

     Description : Implementation of simple File class, with support for
                   sequential read/write operations.
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
#include "file.H"
#include "utils.H"
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR */
/*--------------------------------------------------------------------------*/

File::File(FileSystem *_fs, int _id) {
    Console::puts("Opening file.\n");
    cur  = 0;
    fs = _fs;
    int flag = 0;
    //Console::puti(fs->inodes[0].id); Console::puts("\n");
    for (int i=0;i<fs->MAX_INODES;i++){
        
        if (fs->inodes[i].id==_id){
            inode_ind = i;
            f_size = fs->inodes[i].f_size;
            block_no = fs->inodes[i].block_id;
            flag = 1;
            fs->disk->read(block_no, block_cache);
        }
    }
   

    if(! flag){
        Console::puts("could not find file "); Console::puti(_id);Console::puts("\n");
    }
}

File::~File() {
    Console::puts("Closing file.\n");
    fs->disk->write(block_no, block_cache);
    fs->inodes[inode_ind].f_size = f_size; //updating inode file size
    cur = 0;
    unsigned char * tmp = (unsigned char *)fs->inodes;
    fs->disk->write(1,tmp); // updating the inode list on the disk
    /* Make sure that you write any cached data to disk. */
    /* Also make sure that the inode in the inode list is updated. */
}

/*--------------------------------------------------------------------------*/
/* FILE FUNCTIONS */
/*--------------------------------------------------------------------------*/

int File::Read(unsigned int _n, char *_buf) {
    Console::puts("reading from file\n");
    int buf_pos = 0;

    while( !EoF() || buf_pos < _n )
    {
        _buf[buf_pos] = block_cache[cur];
        cur +=1;
        buf_pos += 1;
    }
    return buf_pos;
    // assert(false);
}

int File::Write(unsigned int _n, const char *_buf) {
    Console::puts("writing to file\n");
    int count = 0;
    while(count < _n)
    {
        block_cache[cur] = _buf[count];
        count ++;
        cur ++;
        if(cur >= f_size)
            f_size = cur;

    }
    return count;
    // assert(false);
}

void File::Reset() {
    Console::puts("resetting file\n");
    cur =0 ;
    // assert(false);
}

bool File::EoF() {
    Console::puts("checking for EoF\n");
    if (cur  < f_size){
        return false;
    }
    return true;
    // assert(false);
}
