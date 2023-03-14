/*
 File: ContFramePool.C
 
 Author:
 Date  : 
 
 */

/*--------------------------------------------------------------------------*/
/* 
 POSSIBLE IMPLEMENTATION
 -----------------------

 The class SimpleFramePool in file "simple_frame_pool.H/C" describes an
 incomplete vanilla implementation of a frame pool that allocates 
 *single* frames at a time. Because it does allocate one frame at a time, 
 it does not guarantee that a sequence of frames is allocated contiguously.
 This can cause problems.
 
 The class ContFramePool has the ability to allocate either single frames,
 or sequences of contiguous frames. This affects how we manage the
 free frames. In SimpleFramePool it is sufficient to maintain the free 
 frames.
 In ContFramePool we need to maintain free *sequences* of frames.
 
 This can be done in many ways, ranging from extensions to bitmaps to 
 free-lists of frames etc.
 
 IMPLEMENTATION:
 
 One simple way to manage sequences of free frames is to add a minor
 extension to the bitmap idea of SimpleFramePool: Instead of maintaining
 whether a frame is FREE or ALLOCATED, which requires one bit per frame, 
 we maintain whether the frame is FREE, or ALLOCATED, or HEAD-OF-SEQUENCE.
 The meaning of FREE is the same as in SimpleFramePool. 
 If a frame is marked as HEAD-OF-SEQUENCE, this means that it is allocated
 and that it is the first such frame in a sequence of frames. Allocated
 frames that are not first in a sequence are marked as ALLOCATED.
 
 NOTE: If we use this scheme to allocate only single frames, then all 
 frames are marked as either FREE or HEAD-OF-SEQUENCE.
 
 NOTE: In SimpleFramePool we needed only one bit to store the state of 
 each frame. Now we need two bits. In a first implementation you can choose
 to use one char per frame. This will allow you to check for a given status
 without having to do bit manipulations. Once you get this to work, 
 revisit the implementation and change it to using two bits. You will get 
 an efficiency penalty if you use one char (i.e., 8 bits) per frame when
 two bits do the trick.
 
 DETAILED IMPLEMENTATION:
 
 How can we use the HEAD-OF-SEQUENCE state to implement a contiguous
 allocator? Let's look a the individual functions:
 
 Constructor: Initialize all frames to FREE, except for any frames that you 
 need for the management of the frame pool, if any.
 
 get_frames(_n_frames): Traverse the "bitmap" of states and look for a 
 sequence of at least _n_frames entries that are FREE. If you find one, 
 mark the first one as HEAD-OF-SEQUENCE and the remaining _n_frames-1 as
 ALLOCATED.

 release_frames(_first_frame_no): Check whether the first frame is marked as
 HEAD-OF-SEQUENCE. If not, something went wrong. If it is, mark it as FREE.
 Traverse the subsequent frames until you reach one that is FREE or 
 HEAD-OF-SEQUENCE. Until then, mark the frames that you traverse as FREE.
 
 mark_inaccessible(_base_frame_no, _n_frames): This is no different than
 get_frames, without having to search for the free sequence. You tell the
 allocator exactly which frame to mark as HEAD-OF-SEQUENCE and how many
 frames after that to mark as ALLOCATED.
 
 needed_info_frames(_n_frames): This depends on how many bits you need 
 to store the state of each frame. If you use a char to represent the state
 of a frame, then you need one info frame for each FRAME_SIZE frames.
 
 A WORD ABOUT RELEASE_FRAMES():
 
 When we releae a frame, we only know its frame number. At the time
 of a frame's release, we don't know necessarily which pool it came
 from. Therefore, the function "release_frame" is static, i.e., 
 not associated with a particular frame pool.
 
 This problem is related to the lack of a so-called "placement delete" in
 C++. For a discussion of this see Stroustrup's FAQ:
 http://www.stroustrup.com/bs_faq2.html#placement-delete
 
 */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/
#define KB * (0x1 << 10)
/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "cont_frame_pool.H"
#include "console.H"
#include "utils.H"
#include "assert.H"

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
/* METHODS FOR CLASS   C o n t F r a m e P o o l */
/* 01 - Head of frame
 * 11 - ALLOCATED
 * 00 - FREE
 * 10 - Inaccessible
/*--------------------------------------------------------------------------*/
ContFramePool* ContFramePool::head;
ContFramePool* ContFramePool::list;

ContFramePool::ContFramePool(unsigned long _base_frame_no,
                             unsigned long _n_frames,
                             unsigned long _info_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::Constructor not implemented!\n");
    // assert(false);
    assert(_n_frames <= FRAME_SIZE*8);

    base_frame_no = _base_frame_no;
    nframes = _n_frames;
    nFreeFrames = _n_frames;
    info_frame_no = _info_frame_no;

    if(info_frame_no == 0) {
        bitmap = (unsigned char *) (base_frame_no * FRAME_SIZE); 
    } else {
        bitmap = (unsigned char *) (info_frame_no * FRAME_SIZE);
    }

    assert ((nframes % 8 ) == 0);

    //mark the bits as 0x00 as free for the whole frame pool
    for(int i=0; i*8 < _n_frames*2; i++) {
        bitmap[i] = 0x0;
    }

    if(_info_frame_no == 0) {
        bitmap[0] = 0x40;
        nFreeFrames--;
    }

    if (ContFramePool::head == NULL) {
        ContFramePool::head = this;
        ContFramePool::list = this;
    } else {
        ContFramePool::list->next = this;
        ContFramePool::list = this;
    }
    next = NULL;
 
    Console::puts("Continuous Frame Pool is initialized\n");
}

unsigned long ContFramePool:: get_frames(unsigned int _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::get_frames not implemented!\n");
    // assert(false);
    unsigned int required = _n_frames;
    unsigned int frame_no = base_frame_no;
    int search = 0;
    int found = 0;
    int p = 0;
    int q = 0;

    if(_n_frames > nFreeFrames) {
        Console::puts("can't allocate ");
        Console::puti(_n_frames);Console::puts("\n");
        return 0; 
    }
     for (unsigned int i = 0; i<nframes/4; i++) {
        unsigned char value = bitmap[i];
        unsigned char mask = 0xC0;
        for (int j = 0; j < 4; j++) {
            if((bitmap[i] & mask) == 0) {
                if(search == 1) {
                    required--;
                } else {
                    search = 1;
                    frame_no += i*4 + j;
                    p = i;
                    q = j;
                    required--;
                }
            } else {
                if(search == 1) {
                    frame_no = base_frame_no;
                    required = _n_frames;
                    p = 0;
                    q = 0;
                    search = 0;
                }
            }
            mask = mask>>2;
            if (required == 0) {
                found = 1;
                break;
            }
        } 
        if (required == 0) {
            found = 1;
            break;
        }
    } 

    
    if (found == 0 ) {
        Console::puts("No free frame found for length: ");Console::puti(_n_frames);Console::puts("\n");
        return 0;
    }

    // now set the sequence of frames as allocated with head pointer
    int remaining = _n_frames;
    unsigned char init_mask = 0x40;
    unsigned char sub_mask = 0xC0;
    init_mask = init_mask>>(q*2);
    sub_mask = sub_mask>>(q*2);
    bitmap[p] = (bitmap[p] & ~sub_mask)| init_mask; //first filter the bits and then set to 01
    
    q++;
    remaining--;

    unsigned char alloc_mask = 0xC0;
    alloc_mask = alloc_mask>>(q*2);
    while(remaining > 0 && q < 4) {
        bitmap[p] = bitmap[p] | alloc_mask;
        alloc_mask = alloc_mask>>2;
        remaining--;
        q++;
    }
    
    for(int i = p + 1; i< nframes/4; i++) {
        alloc_mask = 0xC0;
        for (int j = 0; j< 4 ; j++) {
            if (remaining == 0) {
                break;
            }
            bitmap[i] = bitmap[i] | alloc_mask;
            alloc_mask = alloc_mask>>2;
            remaining--;
        }
        if (remaining ==0){
            break;
        }
    }

    return frame_no;
}

void ContFramePool::mark_inaccessible(unsigned long _base_frame_no,
                                      unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::mark_inaccessible not implemented!\n");
    // assert(false);
    if (_base_frame_no < base_frame_no || base_frame_no + nframes < _base_frame_no + _n_frames) {
        Console::puts("range out of index, cannot mark inaccessible \n");
    } else {

        nFreeFrames -= _n_frames;
        int diff = (_base_frame_no - base_frame_no)*2;
        int p = diff / 8;
        int q = (diff % 8) /2;

        

        int remaining = _n_frames;

        unsigned char alloc_mask = 0x80;
        unsigned char sub_mask = 0xC0;
        alloc_mask = alloc_mask>>(q*2);
        sub_mask = sub_mask>>(q*2);
        while(remaining > 0 && q < 4) {
            bitmap[p] = (bitmap[p] & ~sub_mask) | alloc_mask; 
            alloc_mask = alloc_mask>>2;
            sub_mask = sub_mask>>2;
            remaining--;
            q++;
        }

        for(int i = p + 1; i< p + _n_frames/4; i++) {
            alloc_mask = 0xC0;
            sub_mask = 0xC0;
            for (int j = 0; j< 4 ; j++) {
                if (remaining == 0) {
                    break;
                }
                bitmap[i] = (bitmap[i] & ~sub_mask)| alloc_mask;
                alloc_mask = alloc_mask>>2;
                sub_mask = sub_mask>>2;
                remaining--;
            }
            if (remaining ==0){
                break;
            }
        }
        
    }

}

void ContFramePool::release_frames(unsigned long _first_frame_no)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::release_frames not implemented!\n");
    // assert(false);
    ContFramePool* current_pool = ContFramePool::head;
    while ( (current_pool->base_frame_no > _first_frame_no || current_pool->base_frame_no + current_pool->nframes <= _first_frame_no) ) {
        if (current_pool->next == NULL) {
            Console::puts("Frame not found in any pool, cannot release. \n");
            return;
        } else {
            current_pool = current_pool->next;
        }
    }

    unsigned char* ptr = current_pool->bitmap;
    int diff = (_first_frame_no - current_pool->base_frame_no)*2;
    int p = diff / 8;
    int q = (diff % 8) /2;

    unsigned char init_mask = 0x80;
    unsigned char alloc_mask = 0xC0;
    init_mask = init_mask>>q*2;
    alloc_mask = alloc_mask>>q*2;
    if (((ptr[p]^init_mask)&alloc_mask ) == alloc_mask) {
        ptr[p] = ptr[p] & (~alloc_mask); 
        q++;
        alloc_mask = alloc_mask>>2;
        current_pool->nFreeFrames++;

        while (q < 4) {
            if ((ptr[p] & alloc_mask) == alloc_mask) {
                ptr[p] = ptr[p] & (~alloc_mask);
                q++;
                alloc_mask = alloc_mask>>2;
                current_pool->nFreeFrames++;
            } else {

                return; // all frames in sequence released
            }
        } 

        for(int i = p+1; i < (current_pool->base_frame_no + current_pool->nframes)/4; i++ ) {
            alloc_mask = 0xC0;
            for (int j = 0; j < 4 ;j++) {
                if ((ptr[i] & alloc_mask) == alloc_mask) {
                    ptr[i] = ptr[i] & (~alloc_mask);
                    alloc_mask = alloc_mask>>2;
                    current_pool->nFreeFrames++;
                } 
            }
        } 


    } else {
        Console::puts("Frame not head of sequence, cannot release \n");
    }
}

unsigned long ContFramePool::needed_info_frames(unsigned long _n_frames)
{
    // TODO: IMPLEMENTATION NEEEDED!
    // Console::puts("ContframePool::need_info_frames not implemented!\n");
    // assert(false);
    return (_n_frames*2)/(8*4 KB) + ((_n_frames*2) % (8*4 KB) > 0 ? 1 : 0);
}

