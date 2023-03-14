/*
 File: scheduler.C
 
 Author:
 Date  :
 
 */

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "scheduler.H"
#include "thread.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "machine.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* METHODS FOR CLASS   S c h e d u l e r  */
/*--------------------------------------------------------------------------*/

Scheduler::Scheduler() {
  // assert(false);
  Console::puts("Constructed Scheduler.\n");
}

void Scheduler::check_and_enable_interrupts()
{
    if(!Machine::interrupts_enabled()){
		Machine::enable_interrupts();
  }
}


void Scheduler::check_and_disable_interrupts()
{
  if (Machine::interrupts_enabled())	
	  Machine::disable_interrupts();
}


void Scheduler::yield() {
 
  check_and_disable_interrupts();
  Thread *next_thread   = 	ready_q.pop();

  check_and_enable_interrupts();
  Thread::dispatch_to(next_thread);
  

  // assert(false);
}

void Scheduler::resume(Thread * _thread) {
  // assert(false);
   
  check_and_disable_interrupts(); 
  ready_q.push(_thread);
  check_and_enable_interrupts();
  
}

void Scheduler::add(Thread * _thread) {
  // assert(false);
  check_and_disable_interrupts();

  ready_q.push(_thread);
  check_and_enable_interrupts();
 
}

void Scheduler::terminate(Thread * _thread) {
  // assert(false);
  check_and_disable_interrupts();
  int size = ready_q.qsize;
  for (int i=0;i<size;++i)
{
       Thread* t =ready_q.pop();
      if (t->ThreadId() == _thread->ThreadId())
      {
           delete t; 
           break;
      }
      ready_q.push(t);
  }
  check_and_enable_interrupts();
  yield();
  



}
