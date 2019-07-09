/** \file
 *  Interface for system call utilities. */

#ifndef SYSTEM_CALLS_
#define SYSTEM_CALLS_

#include "sysgen.h"

/** Terminate a process. Print its PID, total CPU time, and avg. burst time to 
 *  the accounting module. 
 *  System's avg. total CPU time of completed processes is updated. 
 *  \param  sys is a pointer to a SYSGEN object. */
void terminate_process(SYSGEN * sys);

/** Printer system call. Query the process and update CPU accounting info, 
 *  request info about the syscall, then queue the process onto the 
 *  appropriate printer device queue. 
 *  \param  sys is a pointer to a SYSGEN object. 
 *  \param  num is the printer device queue being requested. */
void printer_syscall(SYSGEN * sys, long int num);

/** Flashdrive system call. Query the process and update CPU accounting info, 
 *  request info about the syscall, then queue the process onto the 
 *  appropriate flashdrive device queue. 
 *  \param  sys is a pointer to a SYSGEN object. 
 *  \param  num is the flashdrive device queue being requested. */
void flashdrive_syscall(SYSGEN * sys, long int num);

/** Disk system call. Query the process and update CPU accounting info, 
 *  request info about the syscall, then queue the process onto the 
 *  appropriate disk device queue. 
 *  \param  sys is a pointer to a SYSGEN object. 
 *  \param  num is the disk device queue being requested. */
void disk_syscall(SYSGEN * sys, long int num);

#endif
