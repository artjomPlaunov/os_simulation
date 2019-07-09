/** \file
 *  simulation.c:   Main entry point for OS simulation. 
 *                  Calls a system generation function and contains the 
 *                  routine for user input. */

#include <stdio.h>
#include <string.h>
#include "sysgen.h"
#include "interrupts.h"
#include "system_calls.h"

#define BUF_SIZE 100

int main()
{

/********************************  SYS GEN ************************************/

    SYSGEN * os = SYSGEN_new();

printf("------------------------------------------------------------------\n");

    printf(
    "\n\n"
"    Welcome to the simulation. Some remarks about the program: Consider\n"
"    the cursor hovering over a line as a prompt for entering  commands.\n"
"    Press enter  to process  a command or  sequence of commands,  which\n"
"    must be separated  by whitespace. Valid  commands are executed with\n"
"    no output, and invalid commands are  skipped over  with a  message,\n"
"    for   example, if \"A A AA p1\" is  inputted, then each  command is\n"
"    executed from left to right, but   a message is printed saying that\n"
"    \"AA\" was an invalid command.\n\n"

"    Extra commands:\n"
"    - input \"Q\" to quit  the  running section and free up all dynamic\n"
"    memory allocation.\n"
"    - input \"C\" to erase the screen.\n\n");

printf("------------------------------------------------------------------\n");

/********************************* RUNNING ************************************/

    char * delim;
    char * save_ptr;
    char buff[BUF_SIZE];
    char running = 1;

    while(running){
        fgets(buff, BUF_SIZE, stdin);
        delim = strtok_r(buff, " \n\t", &save_ptr);
        for( ; delim != NULL ; delim = strtok_r(NULL, " \n\t", &save_ptr)){
            // CLEAR SCREEN
            if( *delim == 'C' ){
                if( strlen(delim) > 1 ){
                    printf("Skipping command %s\n", delim);
                    continue;
                }
                else{
                    printf("\033[2J\033[23A\033[m");
                    continue;
                }
            }
            // QUIT PROCESS
            else if( *delim == 'Q' ){
                if( strlen(delim) > 1 ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else{
                    delim = NULL;
                    running = 0;
                }
            }
            // NEW PROCESS
            else if( *delim == 'A' ){
                if( strlen(delim) > 1 ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else{
                    create_process(os);
                    continue;
                }
            }
            // SNAPSHOT
            else if( *delim == 'S'){
                if(strlen(delim) > 1){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else{
                    snapshot(os);
                    continue;
                }
            }
            // SYSTEM CALL TERMINATE
            else if( *delim == 't' ){
                if(strlen(delim) > 1){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else{
                    terminate_process(os);
                    continue;
                }
            }
            // PRINTER SYSCALL / INTERRUPT
            else if( *delim == 'p' || *delim == 'P'){
                delim++;
                long int dev_num = strtol(delim, NULL, 10);
                delim--;
                if( dev_num == 0 ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else if( dev_num > os->PRINTER_COUNT ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else{
                    if(*delim == 'p'){
                        printer_syscall(os, dev_num);
                        continue;
                    }
                    else if( *delim == 'P'){
                        printer_interrupt(os, dev_num);
                        continue;
                    }
                }
            }
            // FLASHDRIVE SYSCALL / INTERRUPT
            else if( *delim == 'f' || *delim == 'F'){
                delim++;
                long int dev_num = strtol(delim, NULL, 10);
                delim--;
                if( dev_num == 0 ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else if( dev_num > os->FLASHDRIVE_COUNT ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else{
                    if(*delim == 'f'){
                        flashdrive_syscall(os, dev_num);
                        continue;
                    }
                    else if( *delim == 'F'){
                        flashdrive_interrupt(os, dev_num);
                        continue;
                    }
                }
            }
            // DISK SYSCALL / INTERRUPT
            else if( *delim == 'd' || *delim == 'D'){
                delim++;
                long int dev_num = strtol(delim, NULL, 10);
                delim--;
                if( dev_num == 0 ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else if( dev_num > os->FLASHDRIVE_COUNT ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else{
                    if(*delim == 'd'){
                        disk_syscall(os, dev_num);
                        continue;
                    }
                    else if( *delim == 'D'){
                        disk_interrupt(os, dev_num);
                        continue;
                    }
                }
            }
            else if( *delim == 'K'){
                delim++;
                int pid = strtol(delim, NULL, 10);
                delim--;
                if( pid == 0 ){
                    printf("Skipping command: %s\n", delim);
                    continue;
                }
                else{
                    kill_process(os, pid);
                }        
            }
            // SKIP COMMAND
            else
                printf("Skipping command: %s\n", delim);
        }// End tokenizing
    }// End Running


    /* Free all memory dynamically allocated by the SYSGEN object. */
    SYSGEN_free(os);
    printf("System memory recycled.\n");
}
