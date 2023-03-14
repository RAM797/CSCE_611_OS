#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"


PageTable * PageTable::current_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;
VMPool  * PageTable::pool_list_head = NULL;


void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
    
    
	kernel_mem_pool = _kernel_mem_pool;
    process_mem_pool = _process_mem_pool;
    shared_size = _shared_size;
	Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
    
	page_directory =  (unsigned long *)(kernel_mem_pool->get_frames(1) * PAGE_SIZE);
	page_directory[1023] = (unsigned long)(page_directory )| 3; // recursive page table. mark it as valid
	

   unsigned long *ptbr = (unsigned long *) (process_mem_pool->get_frames(1) * PAGE_SIZE);
   unsigned long addr = 0;
   for(unsigned int i =0; i<1024; i++)
   {
	   ptbr[i] = addr |3; 
	   addr += PAGE_SIZE;
   }
   
      
   page_directory[0] = (unsigned long)ptbr;  
   page_directory[0] = page_directory[0] |3 ;// setting it to be supervisor, RW, exist (011)
     
   for(unsigned int i = 1; i<1023; i++)
   {
	 page_directory[i]= 0|2;  
   }   
  
 paging_enabled = 0;	
 Console::puts("Constructed Page Table object\n");
}

void PageTable::load()
{
	current_page_table = this;
    write_cr3((unsigned long)(current_page_table->page_directory)); // PTBR in x86
	Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
	write_cr0(read_cr0() | 0x80000000);
    paging_enabled = 1;
	Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
	unsigned long addr = read_cr2();   
	unsigned int addr_exist = 0;
	VMPool *ptr = PageTable::pool_list_head;
	for(;ptr!=NULL;ptr=ptr->pool_next)
	{
		if(ptr->is_legitimate(addr) == true)
		{
			addr_exist = 1;
		    break;
		}
	}
	
	if(addr_exist == 0 && ptr!= NULL)
	{
      assert(false);	  	
	}
	
    unsigned long* page_dir_ptr = current_page_table->page_directory;
	
	unsigned long pdi = addr >>22;
    unsigned long pti = (addr & (0x03FF << 12) ) >>12 ;
	
	unsigned long *ptbr; 
    unsigned long *pde; 
	
	if((page_dir_ptr[pdi] & 1) == 0) 
	{
	    
	   ptbr = (unsigned long *)(process_mem_pool->get_frames(1) * PAGE_SIZE);
	   unsigned long *pde = (unsigned long *)(0xFFFFF<<12);               // Point entry to 1023 1023 and then access offset
	   pde[pdi] = (unsigned long)(ptbr)|3;
	}
	
	pde = (unsigned long *) (process_mem_pool->get_frames(1) * PAGE_SIZE);
	unsigned long *page_entry = (unsigned long *)((0x3FF<< 22)| (pdi <<12)); // Point entry to 1023 PDE and then access offset
	page_entry[pti] = (unsigned long)(pde)|3;
			
	Console::puts("handled page fault\n");
}
	

void PageTable::register_pool(VMPool * _vm_pool)
{
 
	 if( PageTable::pool_list_head == NULL ) 
	{
        PageTable::pool_list_head= _vm_pool;
    }
    else 
	{
		VMPool *ptr = PageTable::pool_list_head;
        for(;ptr->pool_next!=NULL;ptr= ptr->pool_next);
        ptr->pool_next= _vm_pool;
    }
    
	Console::puts("registered VM pool\n");		
}

void PageTable::free_page(unsigned long _page_no) 
{

    /*From the addr, getting the page number and hence the frame number to release.*/
	unsigned long pdi =  ( _page_no & 0xFFC)>> 22; 
    unsigned long pti = (_page_no & 0x003FF ) >>12 ;

    unsigned long *pde= (unsigned long *) ( (0x000003FF << 22) | (pdi << 12) );
    unsigned long frame_no = (pde[pti] & 0xFFFFF000)/ PAGE_SIZE;
	
	process_mem_pool->release_frames(frame_no);
    pde[pti] |= 2; 
	Console::puts("freed page\n");
	
	load();
}
