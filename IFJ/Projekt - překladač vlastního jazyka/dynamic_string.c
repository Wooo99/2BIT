/**
 * @file dynamic_string.c
 * 
 * @brief implementation of dynamic string. Struct Dynamic_string is made of 
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
#include "dynamic_string.h"
#include "newscanner.h"
/**
 * actual size means index of the dynamic array
 * */

bool dynamic_string_init(Dynamic_string *dynamicstring)
{
    dynamicstring->string = calloc(DEFAULT_STRING_LENGTH, sizeof(char));
    if (dynamicstring->string == NULL)
    {
        return false;
    }
    dynamicstring->actual_size = 0;
    dynamicstring->string[0]   = '\0';
    dynamicstring->allocated_size = DEFAULT_STRING_LENGTH;
    dynamicstring->error = 0;
    return true;
}
bool dynamic_string_add_char(Dynamic_string *dynamicstring, char c)
{
    if (dynamic_string_full(dynamicstring))
    {
        bool tmpboolean = dynamic_string_double(dynamicstring);
        if (tmpboolean == false)
        {
            return false;
        }
    }

    dynamicstring->string[dynamicstring->actual_size] = c;
    dynamicstring->actual_size++;
    dynamicstring->string[dynamicstring->actual_size] = '\0';
    return true;
}
bool dynamic_string_add_string(Dynamic_string *dynamicstring, char *word)
{
    int newlen = strlen(word);
    while (strlen(dynamicstring->string) + newlen > dynamicstring->allocated_size)
    {
        bool tmpboolean = dynamic_string_double(dynamicstring);
        if (tmpboolean == false)
        {
            return false;
        }
    }

    char *startnew = &(dynamicstring->string[dynamicstring->actual_size]);
    strcpy(startnew, word);
    dynamicstring->actual_size += newlen;
    return true;
}
bool dynamic_string_double(Dynamic_string *dynamicstring)
{
    char *tmp = realloc(dynamicstring->string, dynamicstring->allocated_size * 2);
    if (tmp != NULL)
    {
        dynamicstring->string = tmp;
        dynamicstring->allocated_size = dynamicstring->allocated_size * 2;
        return true;
    }
    else
    {
        return false;
    }
}
void dynamic_string_free(Dynamic_string *dynamicstring)
{
    free(dynamicstring->string);
    dynamicstring->allocated_size = 0;
    dynamicstring->actual_size = -1;
}
void dynamic_string_delete(Dynamic_string * dynamicstring){
    for (unsigned int i = 0; i < dynamicstring->allocated_size;i++){
        dynamicstring->string[i] = '\0';
    }
    dynamicstring->actual_size = 0;
}
bool dynamic_string_full(Dynamic_string *dynamicstring)
{
    if (dynamicstring->allocated_size == dynamicstring->actual_size + 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void dynamic_string_backspace(Dynamic_string *dynamicstring)
{
    dynamicstring->actual_size--;
    dynamicstring->string[dynamicstring->actual_size] = '\0';
    return;
}

void dynamic_string_print(Dynamic_string dynamicstring) {
    printf("str:\n%s\n", dynamicstring.string);
}


