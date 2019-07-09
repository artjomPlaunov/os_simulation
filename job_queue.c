#include <stdlib.h>
#include "job_queue.h" 

JOBQ * JOBQ_new()
{
    JOBQ *jq = malloc( sizeof(JOBQ) ); 
    *jq = (JOBQ){ .head = NULL, .tail = NULL}; 
    return jq;
}

void JOBQ_enqueue(JOBQ * jq, PCB * insert)
{
    int ins_key = insert->proc_size; 
    PCB * save_ptr;

    if( jq->head == NULL ){
        jq->head = insert;
        jq->tail = insert;
    }
    else{
        /** This is the same code I used in the READYQ file, just with the 
        *  priority comparison operators inclusively swapped, since we are 
        *  checking for a lower value in the queue. */ 
        if( jq->head->proc_size < ins_key ){
            save_ptr = jq->head;
            jq->head = insert;
            jq->head->LINK = save_ptr; 
        }
        else{
            PCB * curr_ptr = jq->head;
            while( curr_ptr->LINK ){
                if( curr_ptr->LINK->proc_size < ins_key ){
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
                jq->tail->LINK = insert;
                jq->tail = insert;
            }
        }
    }
    while( jq->tail->LINK )
        jq->tail->LINK = jq->tail->LINK->LINK;
}

void JOBQ_dequeue(JOBQ * jq, PCB ** dequeued, int p_size)
{
    if( jq->head == NULL ){
        *dequeued = NULL; 
        return;  
    } 
    else{
        if( jq->head->proc_size <= p_size ){
            *dequeued = jq->head; 
            if( jq->head == jq->tail ){
                jq->head = jq->tail = NULL;
            }
            else{
                jq->head = jq->head->LINK; 
            }
            (*dequeued)->LINK = NULL; 
        }
        else{
            PCB * curr_ptr = jq->head;
            while( curr_ptr->LINK ){
                if( curr_ptr->LINK->proc_size <= p_size ){
                    *dequeued = curr_ptr->LINK; 
                    if( curr_ptr->LINK == jq->tail ){
                        jq->tail = curr_ptr; 
                    }
                    else{
                       curr_ptr->LINK = (*dequeued)->LINK; 
                    }
                    (*dequeued)->LINK = NULL; 
                }
                else
                    curr_ptr = curr_ptr->LINK;
            }
            if( curr_ptr->LINK == NULL )
                *dequeued = NULL;
        }
    }
}

void JOBQ_kill(JOBQ * jq, PCB ** dequeued, int pid)
{
    if( jq->head == NULL ){
        *dequeued = NULL;
        return;
    }
    else{
        if( jq->head->PID == pid ){
            *dequeued = jq->head;
            if( jq->head == jq->tail ){
                jq->head = jq->tail = NULL;
            }
            else{
                jq->head = jq->head->LINK;
            }
            (*dequeued)->LINK = NULL;
        }
        else{
            PCB * curr_ptr = jq->head;
            while( curr_ptr->LINK ){
                if( curr_ptr->LINK->PID == pid ){
                    *dequeued = curr_ptr->LINK;
                    if( curr_ptr->LINK == jq->tail ){
                        jq->tail = curr_ptr;
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

void JOBQ_free(JOBQ * jq)
{
    PCB * curr_ptr = jq->head;
    PCB * prev_ptr = jq->head;
    while( curr_ptr ){
        curr_ptr = curr_ptr->LINK;
        PCB_free(prev_ptr);
        prev_ptr = curr_ptr;
    }
    free(jq);
}
