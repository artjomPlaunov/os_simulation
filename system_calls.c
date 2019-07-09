/** \file
 *
 *  system_calls.c: Implementation for system call utility methods. */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "system_calls.h"
#include "user_input_utilities.h"


/** Compute and update a processes' burst average. 
 *  \param  old_avg is the processes' old average.
 *  \param  new_val is the length of the most recently completed burst. 
 *  \param  n is the number of CPU bursts (Should be updated to include 
 *          the most recent CPU burst completed. 
 *  \param  loc is a pointer to where the new burst avg should be stored. */
static void compute_average(double old_avg, double new_val, 
                            int n, double * loc)
{
    double new_avg = (n-1)*old_avg;
    new_avg += new_val;
    new_avg = new_avg / n; 
    *loc = new_avg;
}


/** Update a processes' CPU accounting info after is has completed a CPU 
 *  burst. 
 *  \param  sys is a pointer to a SYSGEN object. This function technically 
 *          performs the update while the process is still in the CPU, since
 *          the CPU's PCB pointer of the SYSGEN object is used to update 
 *          the accounting values in the PCB. */
static void update_accounting(SYSGEN * sys)
{
    /** CPU burst complete. Query timer and compute new accounting data. */
    double proc_bt; // process burst time. 
    get_double("CPU process requested syscall. Time query (ms):", &proc_bt);
    
    /** Add burst time to total CPU time and update value of 
     *  most recent burst time. */    
    sys->CPU->RUNNING_PROCESS->CPU_t += proc_bt; 

    /** Update burst time and number of completed burst. */
    proc_bt += sys->CPU->RUNNING_PROCESS->BURST_t;
    sys->CPU->RUNNING_PROCESS->BURST_n++; 
    
    compute_average(sys->CPU->RUNNING_PROCESS->BURST_avg,
                    proc_bt,
                    sys->CPU->RUNNING_PROCESS->BURST_n, 
                    &sys->CPU->RUNNING_PROCESS->BURST_avg); 

    /** Tau next is computed using an added weight between the system history, 
     *  which is simply the previous value of Tau next, and the most recent 
     *  process burst time. */ 
    sys->CPU->RUNNING_PROCESS->TAU_n_plus1 
        =       (sys->a)      * (sys->CPU->RUNNING_PROCESS->TAU_n_plus1) 
            +   (1 - sys->a)  * (proc_bt); 
   
    /** Set Tau remaining to new system history value. */ 
    sys->CPU->RUNNING_PROCESS->TAU_r = sys->CPU->RUNNING_PROCESS->TAU_n_plus1;

    /** Set CPU burst time back to zero. */
    sys->CPU->RUNNING_PROCESS->BURST_t = 0;
}


void terminate_process(SYSGEN * sys)
{
    int deallocated = 0;
    
    if( sys->CPU->RUNNING_PROCESS != NULL ){
      
        /** Process termination is considered as a completion: 
         *  1) Query timer for CPU burst length, update CPU time, 
         *     add up total burst time.  
         *  2) Compute new burst average,
         *  3) Update system's average total CPU time of completed processes. 
         *  4) Send report to accounting module. 
         *  6) Return frames occupied by the process to the system's free 
         *     frame list, and update the frame table accordingly. */
        
        /**   1   */
        double proc_bt; // process burst time. 
        get_double("Terminating CPU process. Time query:", &proc_bt);
        sys->CPU->RUNNING_PROCESS->CPU_t += proc_bt; 
        proc_bt += sys->CPU->RUNNING_PROCESS->BURST_t; 
        sys->CPU->RUNNING_PROCESS->BURST_n++; 
        
        /**   2   */
        compute_average(sys->CPU->RUNNING_PROCESS->BURST_avg,
                        proc_bt,
                        sys->CPU->RUNNING_PROCESS->BURST_n, 
                        &sys->CPU->RUNNING_PROCESS->BURST_avg); 

        /**   4    */
        sys->CPU_n++;
        compute_average(sys->CPU_avg,
                        sys->CPU->RUNNING_PROCESS->CPU_t,
                        sys->CPU_n, 
                        &sys->CPU_avg);
        /**   5   */
        printf("Proc with PID: %d, CPU time: %.3lfms, burst avg: %.3lfms," 
                " killed.\n",
                sys->CPU->RUNNING_PROCESS->PID,
                sys->CPU->RUNNING_PROCESS->CPU_t,
                sys->CPU->RUNNING_PROCESS->BURST_avg);

        /**   6   */ 
        for( int i = 0; i < sys->CPU->RUNNING_PROCESS->num_pages; i++){
            sys->frame_bag.counter++; 
            int frame_num = sys->CPU->RUNNING_PROCESS->page_table[i]; 
            sys->frame_bag.frames[sys->frame_bag.counter] = frame_num; 
            sys->frame_table[frame_num].PID = -1; 
            sys->frame_table[frame_num].PAGE_NUM = -1;
            sys->num_free_frames++;
        }

        /**   Free the process.  */
        PCB_free(sys->CPU->RUNNING_PROCESS);
        sys->CPU->RUNNING_PROCESS = NULL;
        deallocated = 1;
    }

    /** The previous process relinquished the frames it was using back to 
     *  the system, so queue up as many processes from the job pool as 
     *  possible, largest fit first. */
    PCB * does_it_blend; 
    int free_mem = sys->num_free_frames * sys->frame_size; 
    JOBQ_dequeue(sys->JOB_QUEUE, &does_it_blend, free_mem);
    while( does_it_blend ){
        /** Allocate free frames to processes' page table. */
        for(int i = 0; i < does_it_blend->num_pages; i++){
            int num = does_it_blend->page_table[i] 
                    = sys->frame_bag.frames[sys->frame_bag.counter];
            sys->frame_bag.counter--; 
            sys->frame_table[num].PID = does_it_blend->PID;
            sys->num_free_frames--;
        }
        /** queue process from job bool onto RQ. */
        READYQ_enqueue(sys->READY_QUEUE, does_it_blend);
        
        /** Update amount of free memory available and search the 
         *  job pool again. */ 
        free_mem = sys->num_free_frames * sys->frame_size;
        JOBQ_dequeue(sys->JOB_QUEUE, &does_it_blend, free_mem); 
    } 

    /** CPU is NULL, so if the RQ is non-empty dequeue a PCB and place it into
     *  the CPU. */
    if( !READYQ_empty(sys->READY_QUEUE) ){
        READYQ_dequeue(sys->READY_QUEUE, &sys->CPU->RUNNING_PROCESS);
    }

    /** Error case: If the CPU was not deallocated but the CPU is empty, 
     *  then display an error message since there is no process to 
     *  terminate. */
    if( sys->CPU->RUNNING_PROCESS == NULL && deallocated == 0)
        printf("The CPU is empty.\n");
printf("------------------------------------------------------------------\n");
}


void printer_syscall(SYSGEN * sys, long int num)
{
    /** If the CPU is empty, return. */
    if(sys->CPU->RUNNING_PROCESS == NULL){
        printf("CPU is empty.\n");
        return;
    }

    PCB * proc_ptr = sys->CPU->RUNNING_PROCESS;
    
    /** Update process accounting info. */
    update_accounting(sys);

    /** Get file name from user. */
    char * file_name;
    get_string("Enter file name", &file_name);
    
    /** Get starting location. */
    int loc; 
    get_hex("Enter starting location(hex):", &loc);
    
    while( loc/sys->frame_size >= proc_ptr->num_pages ){
        printf("Logical address index exceeds page table bounds. \n");
        get_hex("Enter starting location(hex):", &loc);
    }
    int offset = loc % sys->frame_size; 
    int base = loc/sys->frame_size; 
    base = proc_ptr->page_table[base]; 
    base *= sys->frame_size;
    loc = base + offset; 

    printf("Physical Address is: %x.\n", loc);

    /** Get read/write char. */
    char rw; 
    rw = 'w';
   
    /** Get file length. */ 
    int len; 
    get_hex("Enter file length(hex):", &len);

printf("------------------------------------------------------------------\n");

    /** Create PARAMS object. */
    PARAMS obj = {  .CYLINDER = 0,
                    .FILE_NAME = file_name, 
                    .MEM_START = loc,
                    .READ_WRITE = rw, 
                    .FILE_LEN= len};
    
    /** Create a device node. */
    D_NODE * ptr = D_NODE_new(sys->CPU->RUNNING_PROCESS, obj);

    /** Move a process from RQ to CPU if available. */
    if( !READYQ_empty(sys->READY_QUEUE) ){
        READYQ_dequeue(sys->READY_QUEUE, &sys->CPU->RUNNING_PROCESS);
    }
    else{
        sys->CPU->RUNNING_PROCESS = NULL;
    }
    /** Queue up the D_NODE. */
    DEVICEQ_enqueue(sys->PRINTERS[num-1], ptr);
}


void flashdrive_syscall(SYSGEN * sys, long int num)
{
    /** If the CPU is empty, return. */
    if(sys->CPU->RUNNING_PROCESS == NULL){
        printf("CPU is empty.\n");
        return;
    }
    PCB * proc_ptr = sys->CPU->RUNNING_PROCESS;
     
    /** Update process accounting info. */
    update_accounting(sys);

    /** Get file name from user. */
    char * file_name;
    get_string("Enter file name", &file_name);
    
    /** Get starting location. */
    int loc; 
    get_hex("Enter starting location(hex):", &loc);
    while( loc/sys->frame_size >= proc_ptr->num_pages ){
        printf("Logical address index exceeds page table bounds. \n");
        get_hex("Enter starting location(hex):", &loc);
    }
    int offset = loc % sys->frame_size; 
    int base = loc/sys->frame_size; 
    base = proc_ptr->page_table[base]; 
    base *= sys->frame_size;
    loc = base + offset; 

    printf("Physical Address is: %x.\n", loc);
    
    /** Get read/write char. */
    int rw;
    get_rw(&rw); 
    
    /** Flush input buffer. */
    int c;
    while ( (c = getchar()) != '\n' && c != EOF);
    
    /** If write, get file length. */
    int len;
    if( rw == 'w'){
        get_hex("Enter file length:", &len); 
    }
    else
        len = 0;
printf("------------------------------------------------------------------\n");

    /** Create PARAMS object. */
    PARAMS obj = {  .CYLINDER = 0,
                    .FILE_NAME = strdup(file_name), 
                    .MEM_START = loc,
                    .READ_WRITE = rw, 
                    .FILE_LEN= len};
    
    /** Create a D_NODE. */
    D_NODE * ptr = D_NODE_new(sys->CPU->RUNNING_PROCESS, obj);

    /** Move a process from RQ to CPU, if available. */
    if( !READYQ_empty(sys->READY_QUEUE) ){
        READYQ_dequeue(sys->READY_QUEUE, &sys->CPU->RUNNING_PROCESS);
    }
    else{
        sys->CPU->RUNNING_PROCESS = NULL;
    }

    /** Queue up the D_NODE. */
    DEVICEQ_enqueue(sys->FLASHDRIVES[num-1], ptr);
}


void disk_syscall(SYSGEN * sys, long int num)
{
    /** Return is CPU empty. */
    if(sys->CPU->RUNNING_PROCESS == NULL){
        printf("CPU is empty.\n");
        return;
    }
    PCB * proc_ptr = sys->CPU->RUNNING_PROCESS;
    
    /** Update process accounting info. */
    update_accounting(sys);

    /** Get cylinder # for syscall. */
    int cyl;
    get_int("Enter disk cylinder number ( x >= 1 ):", &cyl);
    while(    cyl < 1 
          ||  cyl > sys->CYLINDER_COUNTS[num-1] )
    {
        printf("Cylinder out of range.\n");
        get_int("Enter disk cylinder number ( x >= 1 ):", &cyl);
    }
    
    /** Get file name. */
    char * file_name;
    get_string("Enter file name", &file_name);
    
    /** Get starting location. */
    int loc;
    get_hex("Enter starting location(hex):", &loc);
    while( loc/sys->frame_size >= proc_ptr->num_pages ){
        printf("Logical address index exceeds page table bounds. \n");
        get_hex("Enter starting location(hex):", &loc);
    }
    int offset = loc % sys->frame_size; 
    int base = loc/sys->frame_size; 
    base = proc_ptr->page_table[base]; 
    base *= sys->frame_size;
    loc = base + offset; 

    printf("Physical Address is: %x.\n", loc);
    
    /** Get read/write char. */
    int rw;
    get_rw(&rw);
    
    /** Flush input buffer. */
    int c;
    while ( (c = getchar()) != '\n' && c != EOF);
    
    /** Get file length if write. */
    int len;
    if( rw == 'w'){
        get_hex("Enter file length:", &len);
    }
    else
        len = 0;
printf("------------------------------------------------------------------\n");

    /** Initialize PARAMS object. */
    PARAMS obj = {  .CYLINDER = cyl,
                    .FILE_NAME = strdup(file_name), 
                    .MEM_START = loc,
                    .READ_WRITE = rw, 
                    .FILE_LEN= len};

    /** Create a new D_NODE. */
    D_NODE * ptr = D_NODE_new(sys->CPU->RUNNING_PROCESS, obj);

    /** Move a process from RQ to CPU, if available. */
    if( !READYQ_empty(sys->READY_QUEUE) ){
        READYQ_dequeue(sys->READY_QUEUE, &sys->CPU->RUNNING_PROCESS);
    }
    else{
        sys->CPU->RUNNING_PROCESS = NULL;
    }
    
    /** Queue up the D_NODE. */
    DISKQ_enqueue(sys->DISKS[num-1], ptr);
}
