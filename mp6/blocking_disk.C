/*
     File        : blocking_disk.c

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
#include "blocking_disk.H"
#include "thread.H"
#include "scheduler.H"
#include "machine.H"
/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR */
/*--------------------------------------------------------------------------*/
int mutex;
int TAS(int *mutex) 
{
    int tmp = *mutex;
    *mutex = 1;
    return tmp;
}

void init(int *mutex) 
{
   *mutex = 0;
}

void acquire() 
{
   while(TAS(&mutex));
}

void release()
{
   mutex = 0;
}

int get_fibonacii(int n)
{
  if(n==1 || n ==0 )
  return 1;

  return get_fibonacii(n-1)+get_fibonacii(n-2);
}

extern Scheduler * SYSTEM_SCHEDULER;
BlockingDisk::BlockingDisk(DISK_ID _disk_id, unsigned int _size) 
  : SimpleDisk(_disk_id, _size) {
    init(&mutex);
}


/*--------------------------------------------------------------------------*/
/* SIMPLE_DISK FUNCTIONS */
/*--------------------------------------------------------------------------*/

void BlockingDisk::read(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  acquire();
  SimpleDisk::read(_block_no, _buf);
  release();


}

bool BlockingDisk::is_ready()
{ 
  //running a fake loop as bochs isn't emulating disk behaviour appropriately
  // Console::puts("inside is ready\n");
  // for(int i=0; i<50;i++)
  // {
  //     int x = get_fibonacii(i);
  //     // Console::puti(x);

  // }
 return ((Machine::inportb(0x1F7) & 0x08) != 0);
}


void BlockingDisk::write(unsigned long _block_no, unsigned char * _buf) {
  // -- REPLACE THIS!!!
  acquire();
  SimpleDisk::write(_block_no, _buf);
  release();
}


void BlockingDisk::handle_interrupt(REGS *_r) {
    //Thread * next_thread_id = blocking_q.pop();
    //SYSTEM_SCHEDULER->resume(next_thread_id->CurrentThread());
    Console::puts("handled interrupt 14");
}

void BlockingDisk::wait_until_ready()
{

while (!SimpleDisk::is_ready()) 
{
  Console::puts("yielding the CPU\n");
#ifdef ENABLE_INTERRUPTS
      //blocking_q.push(Thread::CurrentThread());
#else
		SYSTEM_SCHEDULER->resume(Thread::CurrentThread());
#endif
		SYSTEM_SCHEDULER->yield();
}

}
