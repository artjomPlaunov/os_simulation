/** \file
 *  print_utilities.h */

#ifndef PRINT_UTILITIES_
#define PRINT_UTILITIES_

#include "sysgen.h"

/** Printer utilities for the snapshot interrupt function. */
void print_system_CPU_time(SYSGEN * sys);
void print_rq_header();
void print_disk_header();
void print_header();
void print_cpu(SYSGEN * sys);
void print_ready_queue(SYSGEN * sys);
void print_printer_queues(SYSGEN * sys);
void print_flashdrive_queues(SYSGEN * sys);
void print_disk_queues(SYSGEN * sys);
void print_frame_table(SYSGEN * sys);
void print_job_queue(SYSGEN * sys);



#endif
