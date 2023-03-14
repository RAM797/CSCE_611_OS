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


void Scheduler::yield() {
 
 
  Thread *next_thread   = 	ready_q.pop();
  Thread::dispatch_to(next_thread);
  

  // assert(false);
}

void Scheduler::resume(Thread * _thread) {
  // assert(false);
   
  ready_q.push(_thread);
  
}

void Scheduler::add(Thread * _thread) {
  // assert(false);

  ready_q.push(_thread);
 
 
}

void Scheduler::terminate(Thread * _thread) {
  // assert(false);
  delete (void *)_thread;
  yield();
  



}
