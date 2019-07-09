#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sysgen.h"
#include "user_input_utilities.h"

#define BUF_SIZE 100

SYSGEN * SYSGEN_new()
{
    SYSGEN * sys_init = malloc(sizeof(SYSGEN));

    // Get SysGen values: 
    get_int("Enter printer device count:", &sys_init->PRINTER_COUNT); 
    get_int("Enter disk device count:", &sys_init->DISK_COUNT); 
    get_int("Enter flash drive device count:", &sys_init->FLASHDRIVE_COUNT);  
    get_alpha("Enter history parameter, a (alpha), 0 <= a <= 1:", &sys_init->a);
    get_double("Enter initial burst estimate, t (tau), in ms:", &sys_init->t);
    get_int("Enter size of memory (# of words):", &sys_init->mem_size);
    get_int("Enter maximum process size:", &sys_init->max_proc_size);
    get_page_size("Enter page size:", &sys_init->frame_size);
    sys_init->num_frames = (sys_init->mem_size)/(sys_init->frame_size);    

    // Set initial CPU statistics: 
    sys_init->CPU_avg = 0.0; 
    sys_init->CPU_n   = 0.0;

    /** Generate frame table and free frame list. */
    sys_init->frame_table = malloc( sizeof(frame) * sys_init->num_frames); 
    for(int i = 0; i < sys_init->num_frames; i++){
        sys_init->frame_table[i].NUM = i; 
        sys_init->frame_table[i].PID = -1; 
        sys_init->frame_table[i].PAGE_NUM = -1;
    }
    sys_init->num_free_frames = sys_init->num_frames;

    sys_init->frame_bag.frames = malloc( sizeof(int) * sys_init->num_frames); 
    for(int i = 0; i < sys_init->num_frames; i++){
        sys_init->frame_bag.frames[i] = i; 
    }
    /** Counter points to last position in int array which contains number 
     *  of an available frame. */
    sys_init->frame_bag.counter = sys_init->num_frames-1;
    

    /** Allocate array of disk cylinder counts and get user input for each 
     *  count. */
    sys_init->CYLINDER_COUNTS = malloc( sizeof(int) * sys_init->DISK_COUNT);
    for( int i = 0; i < sys_init->DISK_COUNT; i++){
        char prompt[BUF_SIZE];
        char * prompt1 = "Enter Disk"; 
        char * prompt2 = "cylinder count"; 
        snprintf(prompt, BUF_SIZE, "%s %d %s:", prompt1, i+1, prompt2); 
        get_int(prompt, &sys_init->CYLINDER_COUNTS[i]); 
    }

    // Allocate the ready queue:
    sys_init->READY_QUEUE = READYQ_new();

    // Allocate the printer queues:
    sys_init->PRINTERS = malloc(sizeof(DEVICEQ*) * sys_init->PRINTER_COUNT);
    for(int i = 0; i < sys_init->PRINTER_COUNT; i++)
        sys_init->PRINTERS[i] = DEVICEQ_new();

    // Allocate hard disk queues:
    sys_init->DISKS = malloc(sizeof(DISKQ*) * sys_init->DISK_COUNT);
    for(int i = 0; i < sys_init->DISK_COUNT; i++)
        sys_init->DISKS[i] = DISKQ_new();

    // Allocate flashdrive queues:
    sys_init->FLASHDRIVES = malloc(sizeof(DEVICEQ*) *
                                    sys_init->FLASHDRIVE_COUNT);
    for(int i = 0; i < sys_init->FLASHDRIVE_COUNT; i++)
        sys_init->FLASHDRIVES[i] = DEVICEQ_new();

    // Allocate the CPU:
    sys_init->CPU = PROCESSOR_new();

    // Allocate job queue:
    sys_init->JOB_QUEUE = JOBQ_new();

    return sys_init;

}

void SYSGEN_free(SYSGEN * recycle)
{
    // Free cylinder count array:
    free(recycle->CYLINDER_COUNTS);
    
    // Free the ready queue:
    READYQ_free(recycle->READY_QUEUE);

    // Free device queues:
    for(int i = 0; i < recycle->PRINTER_COUNT; i++)
        DEVICEQ_free(recycle->PRINTERS[i]);
    free(recycle->PRINTERS);

    // Free disk queues:
    for(int i = 0; i < recycle->DISK_COUNT; i++)
        DISKQ_free(recycle->DISKS[i]);
    free(recycle->DISKS);

    // Free flashdrive queues:
    for(int i = 0; i < recycle->FLASHDRIVE_COUNT; i++)
        DEVICEQ_free(recycle->FLASHDRIVES[i]);
    free(recycle->FLASHDRIVES);

    // Free the CPU:
    if( recycle->CPU->RUNNING_PROCESS != NULL){
      PCB_free(recycle->CPU->RUNNING_PROCESS);
    }
    PROCESSOR_free(recycle->CPU);

    /** Free the frame table and frame list. */ 
    free(recycle->frame_table); 
    free(recycle->frame_bag.frames);

    /** Free the job queue: */
    JOBQ_free(recycle->JOB_QUEUE);

    // Free the SYSGEN object:
    free(recycle);
}
