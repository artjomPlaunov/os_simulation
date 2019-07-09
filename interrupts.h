/** \file 
 *  interrupts.h:   Interface for interrupt utilities. 
 *  Note:           The actual interrupt routine is a static function local 
 *                  within the interrupts.c file.     
 **/

#ifndef INTERRUPTS_
#define INTERRUPTS_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pcb.h"
#include "ready_queue.h"
#include "device_node.h"
#include "device_queue.h"
#include "sysgen.h"
#include "system_calls.h"
#include "print_utilities.h"

/** Snapshot interrupt routine. 
 *  Display information for various device/ready queues. */
void snapshot(SYSGEN * sys);

/** Create a process and queue it onto the ready queue. 
 *  \param  sys is a pointer to a SYSGEN object. */
void create_process(SYSGEN * sys);

void kill_process(SYSGEN * sys, int pid);

/** Device interrupt routines 
 *  See implementation file for details. */
void printer_interrupt(SYSGEN * sys, long int num);
void flashdrive_interrupt(SYSGEN * sys, long int num);
void disk_interrupt(SYSGEN * sys, long int num);








#endif
