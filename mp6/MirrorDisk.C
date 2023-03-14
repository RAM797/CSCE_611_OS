/*
     File        : mirroring_disk.c

     Author      : 
     Modified    : 

     Description : 

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "assert.H"
#include "utils.H"
#include "console.H"
#include "MirrorDisk.H"
#include "scheduler.H"
#include "simple_disk.H"
#include "blocking_disk.H"
#include "thread.H"
#include "machine.H"

extern Scheduler * SYSTEM_SCHEDULER;
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/

//#define ENABLE_THREAD_SYNC



MirrorDisk::MirrorDisk(unsigned int _size)
{
    MASTER_DISK = new BlockingDisk(DISK_ID::MASTER, _size);
    DEPENDENT_DISK = new BlockingDisk(DISK_ID::DEPENDENT, _size);

}

/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/



// void MirrorDisk::wait_until_ready()
// {
//     bool master_is_ready = MASTER_DISK->is_ready();
//     bool dependent_is_ready = DEPENDENT_DISK->is_ready();
// 	// while (! master_is_ready || ! dependent_is_ready) 
//     // {
// 	// 	SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
// 	// 	SYSTEM_SCHEDULER->yield();
// 	// }
    
// }

void MirrorDisk::read(unsigned long _block_no, unsigned char * _buf) 
{
    bool master_is_ready = MASTER_DISK->is_ready();
    bool dependent_is_ready = DEPENDENT_DISK->is_ready();
    if (master_is_ready)
    {
        Console::puts("reading from MASTER \n");
        MASTER_DISK->read(_block_no, _buf);

    }
    else if(dependent_is_ready)
   { 
        Console::puts("reading from DEPENDENT \n");
        DEPENDENT_DISK->read(_block_no, _buf);

   }
   {
        Console::puts("reading from MASTER \n");
        MASTER_DISK->read(_block_no, _buf);

   }
}

void MirrorDisk::write(unsigned long _block_no, unsigned char * _buf) 
{
    Console::puts("writing to master\n");
    MASTER_DISK->write (_block_no, _buf);
    Console::puts("writing to dependent\n");
    DEPENDENT_DISK->write (_block_no, _buf);

}
