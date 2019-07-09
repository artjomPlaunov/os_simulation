/** \file
 *  sysgen.h:   SYSGEN (System generation) interface. */

#ifndef SYSGEN_
#define SYSGEN_

#include "device_queue.h"
#include "disk_queue.h"
#include "ready_queue.h"
#include "cpu.h"
#include "job_queue.h"

typedef struct frame {
    int     NUM;
    int     PID; 
    int     PAGE_NUM;   /** If a process is using a frame, then this is the 
                        *   page number offset for the processes page table 
                        *   that gives this frame number. */
}frame;

typedef struct frame_list{
    int * frames;
    int counter;
} frame_list;

/** struct SYSGEN */ 
typedef struct SYSGEN{
    
    int             PRINTER_COUNT;
    int             DISK_COUNT;
    int             FLASHDRIVE_COUNT;
    int       *     CYLINDER_COUNTS;    // Array of cylinder counts for disks. 
    DEVICEQ   **    PRINTERS;           // Pointer to array of printer queues.
    DISKQ     **    DISKS;              // Pointer to array of disk queues.
    DEVICEQ   **    FLASHDRIVES;        // Pointer to array of flash queues.
    READYQ    *     READY_QUEUE;        // Pointer to the ready queue.
    PROCESSOR *     CPU;                // Pointer to processor object.
    JOBQ      *     JOB_QUEUE;          // Input queue for processes waiting to
                                        // get memory assigned and run. 
    
    /** System-wide CPU accounting info */ 
    
    double          a;                  // History param, 0 <= a <= 1.
    double          t;                  // Burst estimate in ms, t(tau).
    double          CPU_avg;            // System's average total CPU time 
                                        //   of completed processes.   
    double          CPU_n;              // Number of completed processes. 

    /** Memory info */
    int             mem_size;           // Total size of memory.
    int             max_proc_size;      // Maximum process size. 
    int             frame_size;         // Frame size. 
    int             num_frames;         // Number of frames in the system. 
    int             num_free_frames;    // NUmber of available frames. 

    frame       *   frame_table;        // Pointer to frame table. 
    frame_list      frame_bag;          // Free frame list. 

} SYSGEN;

/** System generation. 
    \return a pointer to a SYSGEN object, which contains
                1) Device counts
                2) Pointers to arrays of dynamically allocated queues
                   for each device.
                3) Pointer to the CPU object.
                4) History parameter, a (alpha), 0 <= a <= 1.
                5) Initial burst estimate in milliseconds, t (tau)
                   for all new processes.
                6) Number of cylinders on each disk. 
                7) System wide CPU accounting info. */
SYSGEN * SYSGEN_new();

/** Recycle all memory in use by the system. */
void SYSGEN_free(SYSGEN * recycle);

#endif
