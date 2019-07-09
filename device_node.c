/** \file
 *  device_node.c: Implementation for D_NODE object. */


#include <stdlib.h>
#include <stdio.h>
#include "device_node.h"

D_NODE *D_NODE_new(PCB * insert, PARAMS pcb_params)
{
    D_NODE *new_D_NODE = malloc( sizeof(D_NODE) );
    *new_D_NODE = (D_NODE){ .D_PCB = insert, 
                            .PROCESS_PARAMS = pcb_params,
                            .LINK = NULL};
    return new_D_NODE;
}

void D_NODE_free(D_NODE *recycle)
{
    /** The system call functions, implemented in system_calls.c, 
     *  dynamically allocate the FILE_NAME, which is freed here. */
    if(recycle->PROCESS_PARAMS.FILE_NAME != NULL)
        free(recycle->PROCESS_PARAMS.FILE_NAME);
    free(recycle);
}
