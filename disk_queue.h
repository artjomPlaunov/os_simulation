/** \file   
 *  disk_queue.h:   Interface for DISKQ object. This was created so as to 
 *                  implement a disk scheduling algorithm for processes 
 *                  requesting disk I/O. The algorithm I chose was C-LOOK, 
 *                  Cyclic-LOOK, which combines C-SCAN and C-LOOK. Part of the 
 *                  reason for choosing it asides from solving the starvation
 *                  issue (Dependent on implementation) 
 *                  and being pretty efficient was for ease of 
 *                  implementation: 
 *
 *                  The basic idea for implementation is that you have the disk
 *                  sweeping in one direction, say low->high, so from cylinders
 *                  1 to whatever the cylinder count is specified as. The 
 *                  data structure used is a circular singly linked list, so
 *                  the last node in the list points back to the first node. 
 *                  To simulate the disk sweep, the elements simply have to be
 *                  kept in order, however with the circularity of the list 
 *                  considered. i.e, the following would be a valid list, 
 *                  where the numbers represent the cylinder #:
 *
 *                  head -> 10 -> 12 -> 2 -> 8 -> head -> ...  
 *
 *                  And let's say we insert 9, which is *behind* the current 
 *                  sweep, then the list becomes:
 *
 *                  head -> 10 -> 12 -> 2 -> 8 -> 9 -> head -> ... 
 *
 *                  We could have inserted a new process requesting cylinder 
 *                  10 and it would have been placed where the 9 was placed. 
 *
 *                  The logic for the enqueueing is described in the 
 *                  implementation file. */
    

#ifndef DISK_QUEUE_
#define DISK_QUEUE_

#include "device_node.h"

/** DISKQ struct */
typedef struct DISKQ {
    D_NODE * head;
    D_NODE * tail;
} DISKQ;

DISKQ * DISKQ_new();

void DISKQ_enqueue(DISKQ * dq, D_NODE * insert); 

void DISKQ_dequeue(DISKQ * dq, PCB ** dequeued);

void DISKQ_kill(DISKQ * dq, PCB ** dequeued, int pid);

int DISKQ_empty(DISKQ * dq);

void DISKQ_free(DISKQ * recycle);

#endif
