/** \file
 *  device_queue.h: Interface for DEVICEQ object. FIFO queue storing 
 *  D_NODES, defined in device_node.h. */

#ifndef DEVICE_QUEUE_H_
#define DEVICE_QUEUE_H_

#include "device_node.h"

/** DEVICEQ (Device Queue) struct. */
typedef struct DEVICEQ {
    D_NODE * head;
    D_NODE * tail;
} DEVICEQ;

/** \return pointer to DEVICEQ struct. */
DEVICEQ *DEVICEQ_new();

/** DEVICEQ enqueueing operation. 
 *  \param  dq is a pointer to a DEVICEQ. 
 *  \param  insert is an allocated D_NODE to be queued up. */ 
void DEVICEQ_enqueue(DEVICEQ * dq, D_NODE* insert);

/** DEVICEQ dequeuing operation.
    \param  dq is a pointer to a DEVICEQ object to be dequeued.
    \param  dequeued is a pointer-to-PCB pointer. PCB of the D_NODE at the 
            front of the queue is passed into here, and the D_NODE is 
            deallocated. */
void DEVICEQ_dequeue(DEVICEQ * dq, PCB** dequeued);

void DEVICEQ_kill(DEVICEQ * dq, PCB ** dequeued, int pid);

/** Test for DEVICEQ emptiness. 
    \param  dq is a pointer to a DEVICEQ object which was dynamically
            allocated via a vall to DEVICEQ_new().
    \return 1 if queue is empty,
    \return 0 if non-empty */
int DEVICEQ_empty(DEVICEQ * dq);


/** DEVICEQ_free()
    \param  dq is a pointer to a DEVICEQ object which was created with a call to
            DEVICEQ_new(). Since there is no mechanism/need to handle the case
            where we want to free a device queue but save the PCB's, this call
            to free deallocates any PCB's stored on the device queue. */
void DEVICEQ_free(DEVICEQ * dq);

#endif
