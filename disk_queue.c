/** \file
 *  disk_queue.c: implementation file for a DISKQ. */ 

#include <stdlib.h>
#include "disk_queue.h"

DISKQ *DISKQ_new()
{
    /** Included a tail pointer for the DISKQ_free() routine; 
     *  allows a quick NULL set on the final node and a traversal to delete
     *  D_NODES until a null marker is hit. */
    DISKQ * dq = malloc( sizeof(DISKQ) );
    *dq = (DISKQ){  .head = NULL,
                    .tail = NULL}; 
    return dq; 
}


void DISKQ_enqueue(DISKQ * dq, D_NODE * insert)
{
    int key = insert->PROCESS_PARAMS.CYLINDER; 
    if( DISKQ_empty(dq) ){
        dq->head = insert; 
        dq->tail = insert; 
        dq->tail->LINK = dq->head; 
    }
    else{
        D_NODE * curr_ptr = dq->head->LINK;     
        D_NODE * prev_ptr = dq->head; 
        
        /** The basic gist of this logic check depends on the fact that the 
         *  queue is a Cyclic-LOOK which sweeps low to high: 
         *
         *  There must be a check done to see that:
         *       
         *          (-key is greater than the previous node's key)
         *      AND: 
         *          (-key is less than the next node's key 
         *      OR:  -the previous key is greater than the next key). 
         *  
         *  (i.e., inserting a 10 between a 7 and a 2 in a circular list; 
         *   we want the disk head to continue on to 10, not 2). 
         *
         *  If these conditions are not met, then we reach the end of the 
         *  list and insert at the tail.  
         *
         *  Note this also handles the case where we have multiple processes
         *  requesting the same cylinder. This is because in the first check, 
         *  we see if they key is greater than the previous key, not greater
         *  then or equal to. So if there is a situation such as the 
         *  following: 
         *
         *  head -> 10 -> 3 -> 7 -> 9 -> head -> ...
         *
         *  And we wish to insert another 10, the list would become:
         *
         *  head -> 10(old) -> 3 -> 7 -> 9 -> 10(new) -> head -> ... 
         *
         *
         *  */
        
        while( curr_ptr != dq->head ){
            if(     (key > prev_ptr->PROCESS_PARAMS.CYLINDER) 
                &&  
                    ((key < curr_ptr->PROCESS_PARAMS.CYLINDER)
                ||  (prev_ptr->PROCESS_PARAMS.CYLINDER 
                      > curr_ptr->PROCESS_PARAMS.CYLINDER) ))
            {
                /** Insert routine. */
                prev_ptr->LINK = insert; 
                insert->LINK = curr_ptr; 
                break; 
            }
            else{
                curr_ptr = curr_ptr->LINK;
                prev_ptr = prev_ptr->LINK;
            }
        }
        /** If curr_ptr = head, then perform an insert at the tail. */ 
        if( curr_ptr == dq->head ){
            prev_ptr->LINK = insert;
            insert->LINK = curr_ptr;
            dq->tail = insert; 
        } 
    }   
}

void DISKQ_dequeue(DISKQ * dq, PCB ** dequeued)
{
    if( !DISKQ_empty(dq) )
    {
        /** Place ptr to PCB into dequeued. */ 
        *dequeued = dq->head->D_PCB; 

        /** Was there only a single D_NODE? */
        if( dq->head == dq->tail ){
            D_NODE_free(dq->head);
            dq->head = NULL;
            dq->tail = NULL; 
        }    
        /** Otherwise save the current head value, iterate it to the next 
         *  node, free the saved head value, and set the final node in the 
         *  list to point to the new head. */
         else{
            D_NODE * free_node = dq->head; 
            dq->head = dq->head->LINK;
            D_NODE_free(free_node); 
            dq->tail->LINK = dq->head; 
        }
        (*dequeued)->LINK = NULL;
    }
    else
        *dequeued = NULL; 
}
void DISKQ_kill(DISKQ * dq, PCB ** dequeued, int pid)
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
                dq->tail->LINK = dq->head; 
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
                        curr_ptr->LINK = dq->head;
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

int DISKQ_empty(DISKQ * dq)
{
    if(dq->head == NULL)
        return 1;
    else
        return 0;
}


void DISKQ_free(DISKQ * recycle)
{
    /** Set tail node LINK field to null. */
    if( recycle->tail != NULL )
        recycle->tail->LINK = NULL;

    D_NODE * curr_ptr = recycle->head; 
    D_NODE * prev_ptr = recycle->head; 
    while( curr_ptr ){
        curr_ptr = curr_ptr->LINK; 
        PCB_free(prev_ptr->D_PCB);
        D_NODE_free(prev_ptr); 
        prev_ptr = curr_ptr;
    }
    free(recycle);
}
