/** \file 

    User input utilities. */
#ifndef USER_INPUT_UTILITIES_
#define USER_INPUT_UTILITIES_

/** Prompt user for string and return input via pointer variable. 
    \param  string is a pointer to a string; this is intended for a pass 
            by reference. 
    \param  prompt is a prompt string for the user. 
    \post   *string points to the dynamically allocated string. */  
void get_string(char * prompt, char ** string);

/** Prompt user for hexadecimal input.
    \param  is a pointer to int where the user input will be stored. */
void get_hex(char * prompt, int * loc);

/** Prompt user for int input. 
    \param i is a pointer to int where user input will be stored. */ 
void get_int(char * prompt, int * i);

/** Prompt user for read/write char
    \param c is a pointer to int where the user char will be stored. Blocks 
            until user inputs either "r" or "w". */
void get_rw(int * c);

/** Prompt user for tau, the history parameter. 
 *  \param t is a pointer where the value of tau (double) is stored. */ 
void get_alpha(char * prompt, double * a);

/** Prompt the user for double inputs. 
 *  \param  prompt is the prompt to display for the user. 
 *  \param  t is the pass by ref double to store the answer in. The number is 
 *          rounder to three decimal places. */
void get_double(char * prompt, double * t);

void get_page_size(char * prompt, int * hex);


#endif
