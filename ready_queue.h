/** \file   
 *  ready_queue.h:  Interface for the ready queue,
 *                  which is a preemptive history based scheduling queue 
 *                  for processes waiting to run. */

#ifndef READY_QUEUE_H_
#define READY_QUEUE_H_

#include "pcb.h"

/** READYQ struct. */
typedef struct READYQ{
    PCB *  head;  // Pointer to head of queue (front of the line).
    PCB *  tail;  // Pointer to tail of queue (back of the line).
} READYQ;

/** Generate and return a READYQ. */
READYQ *READYQ_new();

/** Enqueueing operation. 
    \param  rq is a pointer to a READYQ object. 
    \param  insert is a pointer to a PCB to be inserted. 
    \post   To implement a preemptive history based scheduling algorithm, the 
            RQ is implemented as a priority queue with the PCB's TAU_r, 
            remaining tau burst prediction value serving as a key. 
            Lower TAU_r values get pushed to the front of the queue. */
void READYQ_enqueue(READYQ * rq, PCB* insert);

/** Dequeueing operation.
    \param  rq is a pointer to a READYQ object to be dequeued.
    \param  dequeued is a PCB* passed by pointer(syntax to pass by reference
            in the program itself; the pointer to the dequeued PCB
            goes in here; PCB is NOT deallocated. */
void READYQ_dequeue(READYQ * rq, PCB** dequeued);

/** Dequeue process with PID pid and place it into dequeued. */
void READYQ_kill(READYQ * rq, PCB ** dequeued, int pid);

/** READYY_empty()
    \param  rq is a pointer to a READYQ object which was dynamically
            allocated via a vall to READYQ_new().
    \return 1 if queue is empty,
    \return 0 if non-empty */
int READYQ_empty(READYQ * rq);


/** READYQ_free()
    \param  rq is a pointer to a READYQ object which was created with a call to
            READYQ_new(), and whose contents are to be deallocated. PCB's
            within the queue are deallocated as well as the main READYQ
            object. */
void READYQ_free(READYQ * rq);

#endif
