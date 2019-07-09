#include <math.h>
#include <stdio.h>
#include "interrupts.h"
#include "user_input_utilities.h"

/**  Local interrupt routine. 
 *   
 *   If the CPU contains a process, then:
 *   1)  Query timer for amount of time the running process was in CPU.
 *   2)  Add this time to total burst time for currently running process
 *       and subtract it from the TAU_remainder. Update total CPU time. 
 *   3)  If interrupting process has a lower TAU_remaining, queue
 *       the CPU process into RQ then place interrupter proc into CPU.
 *       
 *       Invariant: It's not possible for there to be a process on the 
 *                  rq with a lower TAU_r than the process in the CPU at this 
 *                  point, since any such process would have had to issue 
 *                  an interrupt, at which point it would have performed 
 *                  a query and comparison with the CPU process.   
 *                  Thus it is sufficient to only do a comparison between 
 *                  the interrupting process and CPU process, and not check 
 *                  processes on the RQ.   
 *  
 *   4)  Otherwise just queue interrupting process into the RQ. */
static void interrupt_routine(SYSGEN * sys, PCB * ptr)
{
    /** If CPU is empty, start running process. */
    if( sys->CPU->RUNNING_PROCESS == NULL){
        sys->CPU->RUNNING_PROCESS = ptr;
    }
    else{
        double burst_t; 
        get_double("CPU process interrupted. Time query (ms):", &burst_t); 
        sys->CPU->RUNNING_PROCESS->BURST_t  += burst_t; 
        sys->CPU->RUNNING_PROCESS->TAU_r    -= burst_t; 
        sys->CPU->RUNNING_PROCESS->CPU_t    += burst_t;

        if( ptr->TAU_r < sys->CPU->RUNNING_PROCESS->TAU_r){
            READYQ_enqueue(sys->READY_QUEUE, sys->CPU->RUNNING_PROCESS);
            sys->CPU->RUNNING_PROCESS = ptr; 
        }
        else{
            READYQ_enqueue(sys->READY_QUEUE, ptr);
        }
    }
}

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


void snapshot(SYSGEN * sys)
{
    int c;
    int invalid_cmd = 1;

    printf("Input [r|p|d|f|m|j|c]: ");
    while(invalid_cmd){
        c = getchar();
        if( c == 'r'){
            printf("\n");
            print_rq_header();
            print_ready_queue(sys);
            printf("\n");
            print_system_CPU_time(sys);
            printf("\n");
            invalid_cmd = 0;
        }
        else if( c == 'p'){
            printf("\n");
            print_header();
            print_printer_queues(sys);
            printf("\n");
            invalid_cmd = 0;
        }
        else if( c == 'f'){
            printf("\n");
            print_header();
            print_flashdrive_queues(sys);
            printf("\n");
            print_system_CPU_time(sys);
            printf("\n");
            invalid_cmd = 0;
        }
        else if( c == 'd'){
            printf("\n");
            print_disk_header();
            print_disk_queues(sys);
            printf("\n");
            print_system_CPU_time(sys);
            printf("\n");
            invalid_cmd = 0;
        }
        else if( c == 'c'){
            printf("\n");
            print_rq_header();
            print_cpu(sys);
            printf("\n");
            print_system_CPU_time(sys);
            printf("\n");
            invalid_cmd = 0;
        }
        else if( c == 'm'){
            printf("\n");
            print_frame_table(sys);
            printf("\n");
            invalid_cmd = 0;
        }
        else if( c == 'j'){
            printf("\n");
            print_job_queue(sys); 
            printf("\n");
            invalid_cmd = 0;
        }
        else
            printf("\033[2K\033[1GInput r,p,d,f: \033[m");
    }
printf("-------------------------------------------------------------------\n");
        while ( (c = getchar()) != '\n' && c != EOF);
}

void create_process(SYSGEN * sys)
{
    int p_size;
    get_int("Enter process size:", &p_size);
    
    if( p_size > sys->max_proc_size ){
        printf("Requested process size exceeds maximum process size.\n");
        return;
    }
   
    /** Number of pages is the ceiling of the process size divided by 
     *  frame size. Cast to double during operation. */ 
    int num_pages =  ceil( (double)p_size / (double)sys->frame_size ); 
    
    /** Create a new PCB with system's tau initial value. */
    PCB * new_proc = PCB_new(sys->t, p_size, sys->frame_size, num_pages);
    new_proc->page_table = malloc( sizeof(int) * num_pages ); 
    
    /** Routine if there are enough free-frames available. */
    if( num_pages <= sys->num_free_frames ){
        
        /** Allocate free frames to processes' page table. */
        for(int i = 0; i < num_pages; i++){
            int num = new_proc->page_table[i] 
                    = sys->frame_bag.frames[sys->frame_bag.counter]; 
            sys->frame_bag.counter--; 
            sys->frame_table[num].PID = new_proc->PID; 
            sys->frame_table[num].PAGE_NUM = i; 
            sys->num_free_frames--;
        }
    }
    /** Otherwise, queue the process into the job pool and return from the 
     *  create process routine. */
    else{
        JOBQ_enqueue(sys->JOB_QUEUE, new_proc);
        return;
    }

    /** If RQ and CPU empty, process goes into CPU. */
    if( READYQ_empty(sys->READY_QUEUE) && (sys->CPU->RUNNING_PROCESS == NULL)){
        sys->CPU->RUNNING_PROCESS = new_proc;
    }
    /** Call the interrupt routine. */
    else{
        interrupt_routine(sys, new_proc);
    }
}

void kill_process(SYSGEN * sys, int pid)
{
    /** Interrupt CPU process. */
    if( sys->CPU->RUNNING_PROCESS != NULL ){
        double burst_t; 
        get_double("CPU process interrupted. Time query (ms):", &burst_t);
        sys->CPU->RUNNING_PROCESS->BURST_t  += burst_t;
        sys->CPU->RUNNING_PROCESS->TAU_r    -= burst_t;
        sys->CPU->RUNNING_PROCESS->CPU_t    += burst_t;
    
        /** Queue CPU proc into RQ. */ 
        READYQ_enqueue(sys->READY_QUEUE, sys->CPU->RUNNING_PROCESS); 
        sys->CPU->RUNNING_PROCESS = NULL; 
    }
    
    PCB * kill_proc = NULL; 
    printf("%d\n", pid);
   
    int jq = 0; 

    if( !kill_proc ){
        for(int i = 0; i < sys->DISK_COUNT; i++){
            DISKQ_kill(sys->DISKS[i], &kill_proc, pid);      
            if(kill_proc) break;
        }
    }
        
    if( !kill_proc){
        for(int i = 0; i < sys->PRINTER_COUNT; i++){
            DEVICEQ_kill(sys->PRINTERS[i], &kill_proc, pid);      
            if(kill_proc) break; 
        }
    }
        
    if( !kill_proc){
        for(int i = 0; i < sys->FLASHDRIVE_COUNT; i++){
            DEVICEQ_kill(sys->FLASHDRIVES[i], &kill_proc, pid);      
            if(kill_proc) break; 
        }
    }

    if( !kill_proc ){
        READYQ_kill(sys->READY_QUEUE, &kill_proc, pid); 
    }

    if( !kill_proc ){ 
        JOBQ_kill(sys->JOB_QUEUE, &kill_proc, pid);
        jq = 1; 
    }

    if( kill_proc && jq == 1){
        printf( "Proc with PID: %d from job pool killed.\n", 
                kill_proc->PID);
        PCB_free(kill_proc);
        if( sys->READY_QUEUE->head ){
            PCB* new_proc;
            READYQ_dequeue(sys->READY_QUEUE, &new_proc);
            sys->CPU->RUNNING_PROCESS = new_proc;
        }
    }
    else if( kill_proc ){
        /** If the process was in the middle of a CPU burst then we 
         *  update processes' and system's CPU accounting info. */
        if( kill_proc->BURST_t > 0){ 
            kill_proc->BURST_n++;
            compute_average(kill_proc->BURST_avg,
                            kill_proc->BURST_t,
                            kill_proc->BURST_n,
                            &kill_proc->BURST_avg);
            sys->CPU_n++;
            compute_average(sys->CPU_avg,
                            kill_proc->CPU_t,
                            sys->CPU_n,
                            &sys->CPU_avg);
            printf( "Proc with PID: %d, CPU time: %.3lfms, burst avg: %.3lfms,"
                    " killed.\n",
                    kill_proc->PID,
                    kill_proc->CPU_t,
                    kill_proc->BURST_avg);
            /** Free up frame tables used by the process. */ 
            for( int i = 0; i < kill_proc->num_pages; i++){
                sys->frame_bag.counter++;
                int frame_num = kill_proc->page_table[i];
                sys->frame_bag.frames[sys->frame_bag.counter] = frame_num;
                sys->frame_table[frame_num].PID = -1;
                sys->frame_table[frame_num].PAGE_NUM = -1;
                sys->num_free_frames++;
            }
            PCB_free(kill_proc); 
            
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
                    sys->frame_table[num].PAGE_NUM = i; 
                    sys->num_free_frames--;
                }
                /** queue process from job bool onto RQ. */
                READYQ_enqueue(sys->READY_QUEUE, does_it_blend);

                /** Update amount of free memory available and search the
                *  job pool again. */
                free_mem = sys->num_free_frames * sys->frame_size;
                JOBQ_dequeue(sys->JOB_QUEUE, &does_it_blend, free_mem);
            }
            if( sys->READY_QUEUE->head ){
                PCB* new_proc;
                READYQ_dequeue(sys->READY_QUEUE, &new_proc); 
                sys->CPU->RUNNING_PROCESS = new_proc;   
            }
        }
        else{
            if( kill_proc->CPU_t > 0 ){ 
                sys->CPU_n++;
                compute_average(sys->CPU_avg,
                                kill_proc->CPU_t,
                                sys->CPU_n,
                                &sys->CPU_avg);
            }
            printf( "Proc with PID: %d, CPU time: %.3lfms, burst avg: %.3lfms,"
                    " killed.\n",
                    kill_proc->PID,
                    kill_proc->CPU_t,
                    kill_proc->BURST_avg);
            /** Free up frame tables used by the process. */ 
            for( int i = 0; i < kill_proc->num_pages; i++){
                sys->frame_bag.counter++;
                int frame_num = kill_proc->page_table[i];
                sys->frame_bag.frames[sys->frame_bag.counter] = frame_num;
                sys->frame_table[frame_num].PID = -1;
                sys->frame_table[frame_num].PAGE_NUM = -1;
                sys->num_free_frames++;
            }
            PCB_free(kill_proc); 
            
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
            if( sys->READY_QUEUE->head ){
                PCB* new_proc;
                READYQ_dequeue(sys->READY_QUEUE, &new_proc); 
                sys->CPU->RUNNING_PROCESS = new_proc;   
            }
        } // End kill_proc else branch 
    } // End kill_proc else if branch 
    
    if( !kill_proc) {
        /** Else print statement since K# is not in the system. */ 
        printf("Process with PID #%d not found.\n", pid); 
        PCB * enq; 
        READYQ_dequeue(sys->READY_QUEUE, &enq); 
        sys->CPU->RUNNING_PROCESS = enq;
    }
}

void printer_interrupt(SYSGEN * sys, long int num)
{
    /** Is the printer queue empty? */
    if( sys->PRINTERS[num-1]->head == NULL){
        printf("Device queue p%li is empty.\n", num);
        return;
    }

    /** Dequeue the PCB. */
    PCB * ptr;
    DEVICEQ_dequeue(sys->PRINTERS[num-1], &ptr);
    
    /** Call interrupt routine. */
    interrupt_routine(sys, ptr);

printf("------------------------------------------------------------------\n");
}

void flashdrive_interrupt(SYSGEN * sys, long int num)
{
    // Do a check to see if the printer queue is empty.
    if( sys->FLASHDRIVES[num-1]->head == NULL){
        printf("Device queue f%li is empty.\n", num);
        return;
    }

    // Dequeue the PCB/de-allocate the queues device node.
    PCB * ptr;
    DEVICEQ_dequeue(sys->FLASHDRIVES[num-1], &ptr);

    /** Call interrupt routine. */
    interrupt_routine(sys, ptr);

printf("------------------------------------------------------------------\n");
}

void disk_interrupt(SYSGEN * sys, long int num)
{
    // Do a check to see if the printer queue is empty.
    if( sys->DISKS[num-1]->head == NULL){
        printf("Device queue d%li is empty.\n", num);
        return;
    }

    // Dequeue the PCB/de-allocate the queues device node.
    PCB * ptr;
    DISKQ_dequeue(sys->DISKS[num-1], &ptr);

    interrupt_routine(sys, ptr);

printf("------------------------------------------------------------------\n");
}
