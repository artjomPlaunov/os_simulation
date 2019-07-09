/** \file:  pcb.h
 *          Interface for PCB object. */

#ifndef PCB_H_
#define PCB_H_

/** struct PCB is a Process Control Block. */ 
typedef struct PCB{ 
    
    int             PID;        // Process ID. 
    struct PCB  *   LINK;       // Link to next PCB. 

    /** CPU accounting info*/
    double          TAU_n_plus1;// Prediction for next CPU burst. 
    double          TAU_r;      // Remaining tau value(if interrupt occurs).
    double          CPU_t;      // Total CPU time. 
    double          BURST_n;    // Number of CPU bursts.    
    double          BURST_avg;  // Average CPU burst time. 
    double          BURST_t;    // Current burst time.

    /** Paging info. */ 
    int *           page_table; // Pointer to page table entries. Very simple 
                                // in this software implementation: 
                                // each entry just holds the number of a frame 
                                // in memory. 
    int             proc_size;  // Process size.
    int             page_size;  // Page size. 
    int             num_pages;  // Number of pages.  
} PCB; 

/** Return a pointer to a new PCB object. 
 *  \param tau_init is the system's initial value for estimated burst time. */
PCB *PCB_new(double tau_init, int p_size, int page_size, int num_pages);

/** Free an allocated PCB object. */
void PCB_free(PCB *recycle); 


#endif

