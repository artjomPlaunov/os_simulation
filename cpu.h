/** \file 
 *  cpu.h: Interface for the CPU object. */

#ifndef CPU_
#define CPU_

#include "pcb.h"

/** The CPU contains a pointer to a PCB. */ 
typedef struct PROCESSOR {
    PCB *   RUNNING_PROCESS;
} PROCESSOR;

/** Return a pointer to a new PROCESSOR object. */
PROCESSOR *PROCESSOR_new();

/** Free dynamically allocated PROCESSOR. 
 *  This does not free any PCB that may be pointed to by the 
 *  RUNNING_PROCESS pointer. */
void PROCESSOR_free(PROCESSOR * recycle);

#endif
