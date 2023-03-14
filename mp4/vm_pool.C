/*
 File: vm_pool.C
 
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

#include "vm_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"
#include "simple_keyboard.H"
#include "page_table.H"


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
/* METHODS FOR CLASS   V M P o o l */
/*--------------------------------------------------------------------------*/

VMPool::VMPool(unsigned long  _base_address,
               unsigned long  _size,
               ContFramePool *_frame_pool,
               PageTable     *_page_table) 
{
   base_address = _base_address;
   size = _size;
   frame_pool = _frame_pool;
   page_table = _page_table;
   pool_next= NULL;
   count= 0;
   
   page_table->register_pool(this);
   
   vm_region *ptr_region = (vm_region*)base_address;
   ptr_region[0].base_address= base_address;
   ptr_region[0].length= PageTable::PAGE_SIZE;
   mem_regions = ptr_region;
   rem_size-=PageTable::PAGE_SIZE; 
   count++; 
     
   Console::puts("Constructed VMPool object.\n");
}

unsigned long VMPool::allocate(unsigned long _size) 
{
	if(_size > rem_size)
	{
	Console::puts("can't allocate requested memory size\n");
    assert(false);
	}
	
	unsigned long n_pages = (_size /PageTable::PAGE_SIZE) + (( _size %PageTable::PAGE_SIZE) > 0 ? 1 : 0);
	mem_regions[count].base_address = mem_regions[count-1].base_address +  mem_regions[count-1].length;
	mem_regions[count].length = n_pages*PageTable::PAGE_SIZE;
    count++;
    rem_size-=n_pages*PageTable::PAGE_SIZE;
 
    Console::puts("Allocated region of memory. \n");
	return mem_regions[count-1].base_address; 
}

void VMPool::release(unsigned long _start_address) {

	int region = -1;
	for(int	 i=1;i<count;i++)
	{
	    if(mem_regions[i].base_address  == _start_address)
		   region =i;	
	}	 
	
	unsigned long n_pages = mem_regions[region].length / PageTable::PAGE_SIZE ;
     while(n_pages >0)
	 {
        page_table->free_page(_start_address);
		n_pages--;
		_start_address+=PageTable::PAGE_SIZE;
     }
	 
	for(int i = region; i<count;i++ )
			mem_regions[i]=mem_regions[i+1];	
	count--;
	rem_size+=mem_regions[region].length;
	
	Console::puts("Released region of memory.\n");	
}

bool VMPool::is_legitimate(unsigned long _address) 
{
	if((_address > (base_address + size)) || (_address <  base_address))
	 return false;
    return true;
    Console::puts("Checked whether address is part of an allocated region.\n");
}

