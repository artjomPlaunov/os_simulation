#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "user_input_utilities.h"

#define BUF_SIZE 100

static int is_pow_two(int val){
    if( val == 0 )
        return 0;
    else
        return ( (val & (val-1)) == 0);
}

void get_string(char * prompt, char ** string)
{
    char user_string[BUF_SIZE];
    char return_string[BUF_SIZE];
    char *delim;
    int valid;

    printf("%s: ", prompt);
    fgets(user_string, BUF_SIZE, stdin);

    if( (delim = strtok(user_string, " \n\t")) == NULL)
        valid = 0;
    else
        valid = sscanf(delim, "%s", user_string);

    while( valid == 0 ){
        printf("\033[1A\033[2K%s: \033[m", prompt);
        fgets(user_string, BUF_SIZE, stdin);
        if( (delim = strtok(user_string, "\n\t")) == NULL)
            valid = 0;
        else
            valid = sscanf(delim, "%s", user_string);
    }

    // Dynamically allocate the string, set the pass by ref string param 
    // to point to it. 
    asprintf(string, "%s", user_string);         
}

void get_hex(char * prompt, int * hex)
{
    char user_string[BUF_SIZE]; 
    char *delim;
    int valid; 

    printf("%s ", prompt); 
    fgets(user_string, BUF_SIZE, stdin); 
    if ( (delim = strtok(user_string, " \n\t")) == NULL)
        valid = 0; 
    else
        valid = sscanf(delim, "%x", hex); 
    
    while( valid == 0 ){
        printf("\033[1A\033[2K%s \033[m", prompt); 
        fgets(user_string, BUF_SIZE, stdin); 
        if( (delim = strtok(user_string, " \n\t")) == NULL)
            valid = 0;
        else
            valid = sscanf(delim, "%x", hex);
    }       
}

void get_int(char * prompt, int * i)
{
    char user_string[BUF_SIZE]; 
    char *delim;
    int valid; 

    printf("%s ", prompt); 
    fgets(user_string, BUF_SIZE, stdin); 
    if ( (delim = strtok(user_string, " \n\t")) == NULL)
        valid = 0; 
    else
        valid = sscanf(delim, "%d", i); 
    
    while( valid == 0 ){
        printf("\033[1A\033[2K%s \033[m", prompt); 
        fgets(user_string, BUF_SIZE, stdin); 
        if( (delim = strtok(user_string, " \n\t")) == NULL)
            valid = 0;
        else
            valid = sscanf(delim, "%d", i);
    }       
}
void get_rw(int * c)
{
    int invalid_cmd = 1; 
    printf("Input r/w: ");
    while(invalid_cmd){
        *c = getchar();
        if( *c == 'r'){
            invalid_cmd = 0;
        }
        else if( *c == 'w'){
            invalid_cmd = 0;
        }
        else
            printf("\033[2K\033[1GInput r/w: \033[m");
    }
}

void get_alpha(char * prompt, double * a)
{
    char user_string[BUF_SIZE]; 
    char *delim;
    int valid; 

    printf("%s ", prompt); 
    fgets(user_string, BUF_SIZE, stdin); 
    if ( (delim = strtok(user_string, " \n\t")) == NULL)
        valid = 0; 
    else{
        valid = sscanf(delim, "%lf", a); 
        if( *a < 0 || *a > 1){
            valid = 0;
        }
        else{
            *a = roundf(*a*100) / 100;
        }

    }
    while( valid == 0 ){
        printf("\033[1A\033[2K%s \033[m", prompt); 
        fgets(user_string, BUF_SIZE, stdin); 
        if( (delim = strtok(user_string, " \n\t")) == NULL)
            valid = 0;
        else{
            valid = sscanf(delim, "%lf", a);
            if( *a < 0 || *a > 1){
                valid = 0;
            }
            else{
                *a = roundf(*a*100) / 100;
            }
        }    
    }       
}

void get_double(char * prompt, double * t)
{
    char user_string[BUF_SIZE]; 
    char *delim;
    int valid; 

    printf("%s ", prompt); 
    fgets(user_string, BUF_SIZE, stdin); 
    if ( (delim = strtok(user_string, " \n\t")) == NULL)
        valid = 0; 
    else{
        valid = sscanf(delim, "%lf", t); 
        *t = roundf(*t*1000) / 1000; 
    }
    while( valid == 0 ){
        printf("\033[1A\033[2K%s \033[m", prompt); 
        fgets(user_string, BUF_SIZE, stdin); 
        if( (delim = strtok(user_string, " \n\t")) == NULL)
            valid = 0;
        else{
            valid = sscanf(delim, "%lf", t);
            *t = roundf(*t*1000) / 1000;
        }    
    }       
}

void get_page_size(char * prompt, int * hex)
{
    char user_string[BUF_SIZE];
    char *delim;
    int valid;

    printf("%s ", prompt);
    fgets(user_string, BUF_SIZE, stdin);
    if ( (delim = strtok(user_string, " \n\t")) == NULL)
        valid = 0;
    else{
        valid = sscanf(delim, "%d", hex);
        valid = is_pow_two(*hex);
    }

    while( valid == 0 ){
        printf("\033[1A\033[2K%s \033[m", prompt);
        fgets(user_string, BUF_SIZE, stdin);
        if( (delim = strtok(user_string, " \n\t")) == NULL)
            valid = 0;
        else{
            valid = sscanf(delim, "%d", hex);
            valid = is_pow_two(*hex);
        }
    }

}
