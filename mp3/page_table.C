#include "assert.H"
#include "exceptions.H"
#include "console.H"
#include "paging_low.H"
#include "page_table.H"


#define P_FRAME_SIZE        1
#define P_PRESENT           1
#define P_WRITE             2
#define P_LEVEL             4
#define PD_SHIFT            22
#define PT_SHIFT            12
#define PDE_MASK            0xFFFFF000
#define PT_MASK             0x3FF


PageTable * PageTable::cur_page_table = NULL;
unsigned int PageTable::paging_enabled = 0;
ContFramePool * PageTable::kernel_mem_pool = NULL;
ContFramePool * PageTable::process_mem_pool = NULL;
unsigned long PageTable::shared_size = 0;



void PageTable::init_paging(ContFramePool * _kernel_mem_pool,
                            ContFramePool * _process_mem_pool,
                            const unsigned long _shared_size)
{
  
   //assert(false);
    PageTable::kernel_mem_pool = _kernel_mem_pool;
    PageTable::process_mem_pool = _process_mem_pool;
    PageTable::shared_size = _shared_size;
    Console::puts("Initialized Paging System\n");
}

PageTable::PageTable()
{
   
   //assert(false);
    page_dir = (unsigned long *)(kernel_mem_pool->get_frames
                                (P_FRAME_SIZE)*PAGE_SIZE);
    unsigned long mask_addr = 0;
    unsigned long * direct_map_pt = (unsigned long *)(kernel_mem_pool->get_frames
                                                        (P_FRAME_SIZE)*PAGE_SIZE);
    unsigned long shared_frames = ( PageTable::shared_size / PAGE_SIZE);
    //Console::puts("shared frames : "); Console::puti(shared_frames);
    /*
     * Mark the shared memory with
     * kernel mode | read & write | present
     */
    for(int i = 0; i < shared_frames; i++) {
        direct_map_pt[i] = mask_addr | P_WRITE | P_PRESENT ;
        mask_addr += PAGE_SIZE;
    }
    //set the first page entry
    page_dir[0] = (unsigned long)direct_map_pt | P_WRITE | P_PRESENT;
    mask_addr = 0;
    //mark the non shared memory page entries
    for(int i = 1; i< shared_frames; i++) {
        page_dir[i] = mask_addr | P_WRITE;
    }
    Console::puts("Constructed Page Table object\n");

}


void PageTable::load()
{
     //assert(false);
    cur_page_table = this;
    write_cr3((unsigned long) page_dir);
    Console::puts("Loaded page table\n");
}

void PageTable::enable_paging()
{
   
   //assert(false);
    paging_enabled = 1;
    write_cr0(read_cr0() | 0x80000000);
    
    Console::puts("Enabled paging\n");
}

void PageTable::handle_fault(REGS * _r)
{
  	unsigned long * cur_pdir = (unsigned long *) read_cr3();
    // read the page address where fault occurred
    unsigned long p_addr = read_cr2();
    unsigned long pdir_addr   = p_addr >> PD_SHIFT;
    unsigned long ptab_addr   = p_addr >> PT_SHIFT;
    unsigned long * page_table = NULL;
    unsigned long err_code = _r->err_code;
    unsigned long mask_addr = 0;
    /*
     * --10bits for PD-- --10bits for PT-- --12bit offset--
     * 0000 0000 00       0000 0000 00      00 0000 0000
     */
    page_table = (unsigned long *)(cur_pdir[pdir_addr] & PDE_MASK);
    if ((err_code & P_PRESENT) == 0 ) {

        page_table[ptab_addr & PT_MASK] = (PageTable::process_mem_pool->get_frames
                                                (P_FRAME_SIZE)*PAGE_SIZE) | P_WRITE | P_PRESENT ;

        if ((cur_pdir[pdir_addr] & P_PRESENT ) != 1) {  //fault in Page table
           
            cur_pdir[pdir_addr] = (unsigned long) ((kernel_mem_pool->get_frames
                                                (P_FRAME_SIZE)*PAGE_SIZE) | P_WRITE | P_PRESENT);
            for (int i = 0; i<1024; i++) {
                page_table[i] = mask_addr  ; // set the pages as user page
        }
        page_table[ptab_addr & PT_MASK] = (PageTable::process_mem_pool->get_frames
                                                (P_FRAME_SIZE)*PAGE_SIZE) | P_WRITE | P_PRESENT ;
           
        }
    }
    Console::puts("handled page fault\n");

}

