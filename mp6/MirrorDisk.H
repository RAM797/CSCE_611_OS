/*
     File        : mirroring_disk.H

     Author      : 

     Date        : 
     Description : 

*/

#ifndef _MIRRORING_DISK_H_
#define _MIRRORING_DISK_H_

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "simple_disk.H"
#include "blocking_disk.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* B l o c k i n g D i s k  */
/*--------------------------------------------------------------------------*/

class MirrorDisk{

private:
    BlockingDisk * MASTER_DISK;
    BlockingDisk * DEPENDENT_DISK;
   //  void issue_io(DISK_OPERATION _op, unsigned long _block_no, int disk_id);
    
protected:
	// virtual void wait_until_ready();

public:
   MirrorDisk( unsigned int _size); 
   /* Creates a MirroringDisk device with the given size connected to the 
      MASTER or SLAVE slot of the primary ATA controller.
      NOTE: We are passing the _size argument out of laziness. 
      In a real system, we would infer this information from the 
      disk controller. */

   /* DISK OPERATIONS */

   virtual void read(unsigned long _block_no, unsigned char * _buf);
   /* Reads 512 Bytes from the given block of the disk and copies them 
      to the given buffer. No error check! */

   virtual void write(unsigned long _block_no, unsigned char * _buf);
   /* Writes 512 Bytes from the buffer to the given block on the disk. */

};

#endif