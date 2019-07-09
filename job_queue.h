/** \file   job_queue.h
 *          Interface for JOBQ object. 
 *          Priority queue with higher priority being a larger requested 
 *          process size. */

#ifndef JOBQ_
#define JOBQ_

#include "pcb.h"

/** Struct JOBQ */
typedef struct JOBQ {
    PCB *   head;
    PCB *   tail; 
} JOBQ;

JOBQ *JOBQ_new(); 

void JOBQ_enqueue(JOBQ * jq, PCB * insert); 

/** This is a dequeueing operation intended for after memory space clears 
 *  up and it is possible that a process which hasn't fit before can perhaps
 *  now fit into memory. As such there is a search key, p_size, which 
 *  goes through the queue starting from the largest process and checks 
 *  to see if there is any item less than or equal to p_size, which is 
 *  the amount of memory available in the system. 
 *
 *  NULL is returned in dequeued if no such process is found. */
void JOBQ_dequeue(JOBQ * jq, PCB **  dequeued, int p_size);

void JOBQ_kill(JOBQ * jq, PCB ** dequeued, int pid);

void JOBQ_free(JOBQ * jq);

#endif
