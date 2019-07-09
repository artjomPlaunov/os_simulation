/** \file
 *  cpu.c: Implementation for PROCESSOR object. */

#include <stdlib.h>
#include "cpu.h"

PROCESSOR * PROCESSOR_new()
{
    PROCESSOR *new_cpu = malloc (sizeof(PROCESSOR) ); 
    *new_cpu = (PROCESSOR){ .RUNNING_PROCESS = NULL }; 
    return new_cpu; 
}

void PROCESSOR_free(PROCESSOR * recycle)
{
    if( recycle != NULL )
        free(recycle); 
}
