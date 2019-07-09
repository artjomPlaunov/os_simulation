/** \file
 *  print_utilities.c */

#include <stdio.h>
#include "print_utilities.h"

void print_system_CPU_time(SYSGEN * sys)
{
    printf("System average CPU time of completed processes:" 
            " %.3lfms.\n", 
            sys->CPU_avg);
}

void print_rq_header()
{
    char * PID              = "PID";
    char * BURST_AVG        = "BURST_AVG";
    char * CPU_TIME         = "CPU_TIME";
    char * TAU_NEXT         = "TAU_NEXT";
    char * TAU_REMAINING    = "TAU_REMAINING";

    printf("%-4s %-10s %-9s %-9s %-15s\n",  
            PID, 
            BURST_AVG,
            CPU_TIME,
            TAU_NEXT,
            TAU_REMAINING); 
}

void print_ready_queue(SYSGEN * sys)
{
    PCB * ptr = sys->READY_QUEUE->head;
    char * dummy = "";
    printf("----Ready Queue\n");
    while( ptr ){
        printf("%-4d "     ,     ptr->PID);
        printf("%-10.3lf " ,     ptr->BURST_avg);
        printf("%-9.3lf " ,     ptr->CPU_t);
        printf("%-9.3lf " ,     ptr->TAU_n_plus1);  
        printf("%-15.3lf " ,     ptr->TAU_r);
        printf("\n");
        ptr = ptr->LINK;
    }
}

void print_header()
{
    char * PID              = "PID";
    char * BURST_AVG        = "BURST_AVG";
    char * CPU_TIME         = "CPU_TIME";
    char * FILENAME         = "FILENAME";
    char * MEMSTART         = "MEMSTART";
    char * RW               = "R/W";
    char * FILELEN          = "FILE-LENGTH";

    printf("%-4s %-10s %-9s %-10s %-9s %-4s %-11s\n",  
            PID, 
            BURST_AVG,        
            CPU_TIME, 
            FILENAME, 
            MEMSTART, 
            RW, 
            FILELEN);
}

void print_printer_queues(SYSGEN * sys)
{
    for( int i = 0; i < sys->PRINTER_COUNT; i++){
        if( sys->PRINTERS[i]->head == NULL )
            continue;
        else{
            D_NODE * ptr = sys->PRINTERS[i]->head;
            printf("----p%d\n", i+1);
            while(ptr){
                printf("%-4d " ,        ptr->D_PCB->PID);
                printf("%-10.2f ",        ptr->D_PCB->BURST_avg);
                printf("%-9.2f ",        ptr->D_PCB->CPU_t);
                printf("%-10s ",        ptr->PROCESS_PARAMS.FILE_NAME);
                printf("%-9x ",        ptr->PROCESS_PARAMS.MEM_START);
                printf("%-4c " ,        ptr->PROCESS_PARAMS.READ_WRITE);
                printf("%-11x ",        ptr->PROCESS_PARAMS.FILE_LEN);
                printf("\n");
                ptr = ptr->LINK;
            
            }
        }
    }
}

void print_flashdrive_queues(SYSGEN * sys)
{
    for( int i = 0; i < sys->FLASHDRIVE_COUNT; i++){
        if( sys->FLASHDRIVES[i]->head == NULL )
            continue;
        else{
            D_NODE * ptr = sys->FLASHDRIVES[i]->head;
            printf("----f%d\n", i+1);
            while(ptr){
                printf("%-4d " ,        ptr->D_PCB->PID);
                printf("%-10.2f ",        ptr->D_PCB->BURST_avg);
                printf("%-9.2f ",        ptr->D_PCB->CPU_t);
                printf("%-10s ",        ptr->PROCESS_PARAMS.FILE_NAME);
                printf("%-9x ",        ptr->PROCESS_PARAMS.MEM_START);
                printf("%-4c " ,        ptr->PROCESS_PARAMS.READ_WRITE);
                printf("%-11x ",        ptr->PROCESS_PARAMS.FILE_LEN);
                printf("%-5d " ,       ptr->PROCESS_PARAMS.CYLINDER);
                printf("\n");
                ptr = ptr->LINK;
            }
        }
    }
}

void print_disk_header()
{
    char * PID              = "PID";
    char * BURST_AVG        = "BURST_AVG";
    char * CPU_TIME         = "CPU_TIME";
    char * FILENAME         = "FILENAME";
    char * MEMSTART         = "MEMSTART";
    char * RW               = "R/W";
    char * FILELEN          = "FILE-LENGTH";
    char * CYLINDER         = "CYLINDER"; 

    printf("%-4s %-10s %-9s %-10s %-9s %-4s %-11s %-5s \n",  
            PID, 
            BURST_AVG,        
            CPU_TIME, 
            FILENAME, 
            MEMSTART, 
            RW, 
            FILELEN,
            CYLINDER); 
}

void print_job_queue(SYSGEN * sys)
{
    printf("%-5s %-12s \n", "PID", "Process Size");  
    printf("----Job Pool\n");
    PCB * ptr = sys->JOB_QUEUE->head;
    while( ptr ){
        printf("%-5d %-12d \n", ptr->PID, ptr->proc_size);
        ptr = ptr->LINK;
    }
}

void print_disk_queues(SYSGEN * sys)
{
    for( int i = 0; i < sys->DISK_COUNT; i++){
        if( sys->DISKS[i]->head == NULL )
            continue;
        else{
            D_NODE * ptr = sys->DISKS[i]->head;
            printf("----d%d\n", i+1);
            while(ptr){
                printf("%-4d " ,        ptr->D_PCB->PID);
                printf("%-10.2f ",        ptr->D_PCB->BURST_avg);
                printf("%-9.2f ",        ptr->D_PCB->CPU_t);
                printf("%-10s ",        ptr->PROCESS_PARAMS.FILE_NAME);
                printf("%-9x ",        ptr->PROCESS_PARAMS.MEM_START);
                printf("%-4c " ,        ptr->PROCESS_PARAMS.READ_WRITE);
                printf("%-11x ",        ptr->PROCESS_PARAMS.FILE_LEN);
                printf("%-5d " ,       ptr->PROCESS_PARAMS.CYLINDER);
                printf("\n");
            ptr = ptr->LINK;
            if( ptr == sys->DISKS[i]->head )
                break;
            }
        }
    }
}

void print_cpu(SYSGEN * sys)
{
    PCB * ptr = sys->CPU->RUNNING_PROCESS;
    char * dummy = "";
    printf("----CPU\n");
    if(!ptr) return;
        printf("%-4d "     ,     ptr->PID);
        printf("%-10.3lf " ,     ptr->BURST_avg);
        printf("%-9.3lf " ,     ptr->CPU_t);
        printf("%-9.3lf " ,     ptr->TAU_n_plus1);  
        printf("%-15.3lf " ,     ptr->TAU_r);
        printf("\n");

}

void print_frame_table(SYSGEN * sys)
{
    printf("Free frames: ");
    for( int i = sys->frame_bag.counter; i >= 0; i--)
    {
        printf("%d, ", sys->frame_bag.frames[i]);
    }
    printf("\n\n");
    printf("%-10s %-10s %-10s %-10s %-15s\n", 
            "Frame", 
            "PID", 
            "Status",
            "Page Number",
            "Physical base address"); 
    for(int i = 0; i < sys->num_frames; i++){
        printf("%-10d %-10d %-10s %-10x   %-15x\n", 
                i, 
                sys->frame_table[i].PID,
                sys->frame_table[i].PID == -1 ? "Free" : "Taken", 
                sys->frame_table[i].PAGE_NUM, 
                i*sys->frame_size);
        }
}


