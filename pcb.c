#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <math.h>
#include "pcb.h" 

/** PCB_new() */ 
PCB *PCB_new(double tau_init, int p_size, int pg_size, int n_pages)
{
    /** new_pid holds the next available pid value; a post-increment 
     *  assignment assigns this value while bumping up the value for the 
     *  next process. */
    static int new_pid = 1; 
    PCB *new_PCB = malloc( sizeof(PCB) ); 
    
    // Initialize PCB values: 
    *new_PCB = (PCB){   .PID = new_pid++, 
                        .LINK = NULL,
                        .TAU_n_plus1 = tau_init, 
                        .TAU_r = tau_init, 
                        .CPU_t = 0.00, 
                        .BURST_n = 0,
                        .BURST_avg = 0,
                        .BURST_t = 0,
                        .page_table = NULL,
                        .proc_size = p_size,
                        .page_size = pg_size,
                        .num_pages = n_pages};
    return new_PCB;      
}

/** PCB_free() */ 
void PCB_free(PCB *recycle)
{
    if( recycle->page_table != NULL )
        free(recycle->page_table);
    free(recycle);
}
