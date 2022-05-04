/**
 * @file dynamic_string.h
 * 
 * @brief header file for dynamic string. Struct Dynamic_string is made of 
 * a string and metadata, ensuring expandability and  of the string 
 * using the given functions. 
 * 
 * @date 25.11.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */
#ifndef DYNAMIC_STRING
#define DYNAMIC_STRING

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#define DEFAULT_STRING_LENGTH 30
typedef struct{
    char * string;
    unsigned int actual_size;
    unsigned int allocated_size;
    int error;
}Dynamic_string;
/**
 * @brief initialise the dynamic string and allocate the default length of the dynamic string
 * @return true if successful, false if memory operation failed
 * */
bool dynamic_string_init        (Dynamic_string * dynamicstring);


/**
 * @brief append the dynamic string by the given string. auto-increase size of string
 * @return true if successful, false if memory operation failed
 * */
bool dynamic_string_add_char    (Dynamic_string * dynamicstring, char c);


/**
 * @brief append the dynamic string by the given string. aut-increase size of string
 * @return true if successful, false if memory operation failed
 * */
bool dynamic_string_add_string  (Dynamic_string * dynamicstring, char * word);


/**
 * @brief double the allocated size of the dynamic string
 * @return true if successful, false if memory operation failed
 * */
bool dynamic_string_double      (Dynamic_string * dynamicstring);


/**
 * @brief free internals of dynamic string and set metadata
 * */
void dynamic_string_free        (Dynamic_string * dynamicstring);


/**
 * @brief set all characters of dynamic string to null and set metadata
 * */
void dynamic_string_delete      (Dynamic_string * dynamicstring);


/**
 * @brief is string on its max allocated length?
 * @return true if full, false if not full. 
 * */
bool dynamic_string_full        (Dynamic_string * dynamicstring);


/**
 * @brief delete last letter from Dynamic_string (that is: set to null and lower size of string)
 * */
void dynamic_string_backspace   (Dynamic_string * dynamicstring);


/**
 * @brief print string inside Dynamic_string
 * */
void dynamic_string_print       (Dynamic_string dynamicstring);




#endif
