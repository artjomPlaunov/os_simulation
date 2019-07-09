#include <stdio.h>
#include <stdlib.h>
#include "ready_queue.h"

READYQ *READYQ_new()
{
    READYQ *rq = malloc( sizeof(READYQ) );
    *rq = (READYQ){ .head = NULL, .tail = NULL};
    return rq;
}


/** READYQ is now a priority queue. TAU_r is used as a key for insertions, 
 *  with lower TAU_r values having a higher priority. */
void READYQ_enqueue(READYQ * rq, PCB *insert)
{
    double ins_key = insert->TAU_r; 
    PCB * save_ptr;

    if( READYQ_empty(rq) ){
        rq->head = insert;
        rq->tail = insert; 
    }
    else{
        /** If the head of the queue has a higher TAU_r than the insert key, 
         *  queue the process at the front. */
        if( rq->head->TAU_r > ins_key ){
            save_ptr = rq->head; 
            rq->head = insert; 
            rq->head->LINK = save_ptr; 
        }
        else{
            /** Otherwise iterate through starting from the head,
             *  checking for nodes which have a higher TAU_r value than 
             *  the insert key; insert before such a node. */
            PCB * curr_ptr = rq->head;
            while( curr_ptr->LINK ){
                if( curr_ptr->LINK->TAU_r > ins_key ){
                    save_ptr = curr_ptr->LINK;
                    curr_ptr->LINK = insert; 
                    insert->LINK = save_ptr; 
                    break;
                }
                else{
                    curr_ptr = curr_ptr->LINK;
                }
            }
            /** Reached the end of the list, insert at the end. */
            if( curr_ptr->LINK == NULL){
                rq->tail->LINK = insert;
                rq->tail = insert;
            }
        }
    }
    while( rq->tail->LINK ) 
        rq->tail->LINK = rq->tail->LINK->LINK;
}

void READYQ_dequeue(READYQ * rq, PCB** dequeued )
{
    if( !READYQ_empty(rq) )
    {
        *dequeued = rq->head;
        if( rq->head == rq->tail ){
            rq->head = NULL;
            rq->tail = NULL;
        }
        else
            rq->head = rq->head->LINK;
        
        (*dequeued)->LINK = NULL;
    }
    else
        *dequeued = NULL;
}

void READYQ_kill(READYQ * rq, PCB ** dequeued, int pid)
{
    if( rq->head == NULL ){
        *dequeued = NULL;
        return;
    }
    else{
        if( rq->head->PID == pid ){
            *dequeued = rq->head;
            if( rq->head == rq->tail ){
                rq->head = rq->tail = NULL;
            }
            else{
                rq->head = rq->head->LINK;
            }
            (*dequeued)->LINK = NULL;
        }
        else{
            PCB * curr_ptr = rq->head;
            while( curr_ptr->LINK ){
                if( curr_ptr->LINK->PID == pid ){
                    *dequeued = curr_ptr->LINK;
                    if( curr_ptr->LINK == rq->tail ){
                        rq->tail = curr_ptr;
                        curr_ptr->LINK = (*dequeued)->LINK; 
                    }
                    else{
                       curr_ptr->LINK = (*dequeued)->LINK;
                    }
                    (*dequeued)->LINK = NULL;
                    return;
                }
                else
                    curr_ptr = curr_ptr->LINK;
            }
            if( curr_ptr->LINK == NULL )
                *dequeued = NULL;
        }
    }
}


int READYQ_empty(READYQ * rq)
{
    if(rq->head == NULL)
        return 1;
    else
        return 0;
}

void READYQ_free(READYQ * rq)
{
    PCB * curr_ptr = rq->head;
    PCB * prev_ptr = rq->head;
    while( curr_ptr ){
        curr_ptr = curr_ptr->LINK;
        PCB_free(prev_ptr);
        prev_ptr = curr_ptr;
    }
    free(rq);
}
