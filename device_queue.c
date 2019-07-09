#include <stdlib.h>
#include "device_queue.h"

DEVICEQ *DEVICEQ_new()
{
    DEVICEQ * dq = malloc( sizeof(DEVICEQ) );
    *dq = (DEVICEQ){  .head = NULL, 
                      .tail = NULL};
    return dq;
}

void DEVICEQ_enqueue(DEVICEQ * dq, D_NODE *insert)
{
    if( DEVICEQ_empty(dq) )
        dq->head = insert;
    else
        // Set link of tail D_NODE to point to the new D_NODE.
        dq->tail->LINK = insert;

    // Set the tail of the ready queue to point to the new D_NODE.
    dq->tail = insert;
}

void DEVICEQ_dequeue(DEVICEQ * dq, PCB** dequeued )
{
    if( !DEVICEQ_empty(dq) )
    {
        /* Return PCB into dequeued. */
        *dequeued = dq->head->D_PCB;
        /** Is there only one D_NODE in the queue? */
        if( dq->head == dq->tail ){
            D_NODE_free(dq->head);
            dq->head = NULL;
            dq->tail = NULL;
        }
        /** If not, save the head value, iterate it to point to the next 
         *  node queued up, and release the original head. */
        else{
            D_NODE * free_node = dq->head;
            dq->head = dq->head->LINK;
            D_NODE_free(free_node);
        }
        (*dequeued)->LINK = NULL;
    }
    else
        /** If the DEVICEQ is empty (Although it is technically not possible
         *  to get to this point of code in such a case, I think), then 
         *  set the pointer-to-PCB to NULL. */
        *dequeued = NULL;
}

void DEVICEQ_kill(DEVICEQ * dq, PCB ** dequeued, int pid)
{
    if( dq->head == NULL ){
        *dequeued = NULL;
        return;
    }
    else{
        if( dq->head->D_PCB->PID == pid ){
            *dequeued = dq->head->D_PCB;
            if( dq->head == dq->tail ){
                D_NODE_free(dq->head);
                dq->head = dq->tail = NULL;
            }
            else{
                dq->head = dq->head->LINK;
                dq->tail->LINK = NULL;
            }
            (*dequeued)->LINK = NULL;
        }
        else{
            D_NODE * curr_ptr = dq->head;
            while( curr_ptr->LINK ){
                if( curr_ptr->LINK->D_PCB->PID == pid ){
                    *dequeued = curr_ptr->LINK->D_PCB;
                    if( curr_ptr->LINK == dq->tail ){
                        dq->tail = curr_ptr;
                        curr_ptr->LINK = NULL;
                    }
                    else{
                       curr_ptr->LINK = curr_ptr->LINK->LINK;
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

int DEVICEQ_empty(DEVICEQ * dq)
{
    if(dq->head == NULL)
        return 1;
    else
        return 0;
}

void DEVICEQ_free(DEVICEQ * dq)
{
    D_NODE * curr_ptr = dq->head;
    D_NODE * prev_ptr = dq->head;
    while( curr_ptr ){
        curr_ptr = curr_ptr->LINK;
        PCB_free(prev_ptr->D_PCB);
        D_NODE_free(prev_ptr);
        prev_ptr = curr_ptr;
    }
    free(dq);
}
