/** \file
 *  device_node.h : Interface for a D_NODE object. This is what gets 
 *                  queued up in the DEVICEQ's and DISKQ's. 
 *                  Contains a pointer to a PCB and a PARAMS struct which 
 *                  contains syscall params for the process, as well as a 
 *                  pointer field for the next possible D_NODE in a queue. */

#ifndef DEVICE_NODE_
#define DEVICE_NODE_

#include "pcb.h"

/** System call parameters. */
typedef struct PARAMS {
    int                 CYLINDER;       // Cylinder request number. 
    char          *     FILE_NAME;      // Filename. 
    unsigned int        MEM_START;      // Starting location in memory.
    char                READ_WRITE;     // Read or Write character.
    unsigned int        FILE_LEN;       // File length.
} PARAMS;

/** Device node. */
typedef struct D_NODE {
    PCB             *   D_PCB;              // Pointer to PCB.
    PARAMS              PROCESS_PARAMS;     // Process system call parameters.
    struct D_NODE   *   LINK;               // Link to next D_NODE.
} D_NODE;

/** Generate and return a pointer to a new D_NODE. 
 *  \param  insert is a pointer to a PCB which is inserted into the D_PCB 
 *          D_NODE field. 
 *  \param  pcb_params is a struct that contains all associated system call
 *          parameters for the process. */
D_NODE *D_NODE_new(PCB * insert, PARAMS pcb_params);

/** Release D_NODE memory. PCB pointed to by the D_PCB pointer field is not 
 *  released. 
 *  \param recycle is a pointer to an allocated D_NODE. */
void D_NODE_free(D_NODE *recycle);


#endif
