/**
 * @file symtable.h
 * 
 * @brief header file for the symbol table
 * 
 * @date 20.11.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */
#include <stdbool.h>
#include "dynamic_string.h"
#ifndef SYMTABLE_H
#define SYMTABLE_H

#define MAX_SIZE_OF_SYM_TABLE 19 //random prime number
#define DEFAULT_PARAM_COUNT 5
typedef enum
{
    T_INT,              //integer
    T_FLOAT,            //floater
    T_STRING,           //string
    T_BOOL,             //true/false
    T_UNKNOWN, //used because it is otherwise unitialised value
    T_FUNC,
    T_EMPTY
} DataType;

typedef DataType TermType;  // compatibility

typedef struct
{
    int alloc_return;
    int alloc_param;

    int used_return;
    int used_param;

    bool var_param; // enable/disable variable number of parameters

    DataType *return_types;
    char **param_names;
    DataType *param_types;

} FuncItemData;

typedef union
{
    float floatnumber;
    int intnumber;
    Dynamic_string *dynamicstring;
    FuncItemData *funcitemptr;
} ItemData;

/**
 * @struct Single item representation
 */
typedef struct htab_tabitem
{
    char *key;
    struct htab_tabitem *next;
    DataType dataType;
    char *codename;
    ItemData itemData;

} Symtable_item;

typedef Symtable_item *Symtable[MAX_SIZE_OF_SYM_TABLE]; //define symbol table as array of symtable items

/**
 * @brief hash function 
 * 
 * @note http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)HashTables.html 4.2.multiplication
 * @param str keyword to be hashed
 * @return hash
 */
unsigned long hash(char *str);

/**
 * @brief initialiser of symbol table
 * 
 * @param symtable pointer to the initialised table
 */
void symtable_init(Symtable *symtable);

/**
 * @brief add symbol to symtable without any data. Datatype = Yet_unassigned
 * 
 * @param table pointer to the symbol table
 * @param key keyword
 * @param noerror true if no memory allocation failed
 * @return NULL+true if already existing,NULL+false if failed memory allocation, pointer+true if successful
 */
Symtable_item *symtable_add(Symtable *table, char *key, bool *noerror);

/**
 * @brief calls a symtable add and fills in integer
 * */
Symtable_item *symtable_add_int(Symtable *table, char *key, int value);

/**
 * @brief calls a symtable_add, makes a dynamic string and fills it as a symtable parameter
 * */
Symtable_item *symtable_add_float(Symtable *table, char *key, float value, bool *noerror);

/**
 * @brief add string to the symtable
 * */
Symtable_item *symtable_add_string(Symtable *table, char *key, char *value, bool *noerror);

/**
 * @brief initialise function symbol. call before putting in params and returns
*/
Symtable_item *symtable_add_function_init(Symtable *table, char *key);

/**
 * @brief insert an input parameter to the function
 * */
Symtable_item *Symtable_add_function_inparam(Symtable *table, char *key, char *paramname, DataType paramtype);

/**
 * @brief insert an output parameter to the function
 * */
Symtable_item *Symtable_add_function_outparam(Symtable *table, char *key, DataType returntype);

/**
 * @brief Set function as function with variable number of parameters
 * 
 * @param table Table in which the function is
 * @param key Name of the function
 */
void Symtable_set_var_param(Symtable *table, char *key);

/**
 * @brief find the tableitem by the keyword
 * 
 * @param table symbol table
 * @param key keyword
 * @return NULL if not found, pointer if found
 */
Symtable_item *symtable_search(Symtable *table, char *key);

/**
 * @brief remove tableitem by keyword
 * 
 * @param table symbol table
 * @param key keyword
 */
void symtable_remove(Symtable *table, char *key);

/**
 * @brief clear table and all items of it
 * @param table Symbol table
 */
void symtable_free(Symtable *table);

/**
 * @brief print out contents of the table. Used for debug
 */
void printouttable(Symtable *table);

/**
 * @brief assistive function
*/
void FuncItemData_free(FuncItemData *data);

#endif //_SYMTABLE_H
