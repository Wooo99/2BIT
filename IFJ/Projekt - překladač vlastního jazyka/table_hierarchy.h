/**
 * @file table_hierarchy.h
 * 
 * @brief header file for hierarchical system of tables.
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
/**
 * The system of tables is implemented as a stack of Symtable structs, with dynamic allocation
 * */
#include "symtable.h"
#include "syntax_common.h"
#ifndef TABLE_HIERARCHY_H
#define TABLE_HIERARCHY_H
#define DEFAULT_STACK_SIZE 2
typedef struct {
    Symtable ** table;
    int top;
    int size;
}SymtableStack;


/**
 * @brief Initialises empty stack of tables, used only when declaring
 * @return false if failed operation, true if operation finished successfuly
 * @note initialises the stack to the default size, using dynamic allocation
 * function MUST be accompanied by stackfree, to prevent memory leaks
 * */
bool stackInit(SymtableStack *symtablestack);


/**
 * @brief double the size of dynamically allocated stack, DO NOT USE if unitilialised
 * @param symtablestack stack of symbol tables
 * @return true if done, false if failed
 * */
bool stackDoubleSize(SymtableStack * symtablestack);

/**
 * @brief is the stack empty? yes/no
 * @return true if empty stack, false if not
 * */
bool stackEmpty(SymtableStack * symtablestack);

/**
 * @brief is stack of tables full?
 * */
bool stackFull(SymtableStack * symtablestack);

/**
 * @brief push another table into stack. Can make stack larger
 * @return successfull operation
 * @note can use realloc
 * */
bool stackPush(SymtableStack * symtablestack, Symtable * table);


/**
 * @brief return top table from the table
 * */
Symtable * stackPop(SymtableStack * symtablestack);

/**
 * @brief frees the stack and all of its contents
 * */
void stackFree(SymtableStack * symtablestack);

/**
 * @brief Adds variable (not function) to the last symtable in the list
 * 
 * @note Expects list of symtables with at least 1 symtable
 * 
 * @param symlist List of symtables
 * @param id Name of the variable
 * @param var Token with the variable's value
 * @return Symtable_item* Pointer to the item if succeeded, NULL otherwise
 */
Symtable_item *symstack_add_to_last(SymtableStack *symlist, char *id, TermType type);//TODO rename

/**
 * @brief returns topmost table from the stack
 * 
 * @param symlist 
 * @return Symtable* 
 */
Symtable *stackTop(SymtableStack *symlist);

/**
 * @brief returns bottomost table from the stack
 * 
 * @param symlist 
 * @return Symtable* 
 */
Symtable *stackBottom(SymtableStack *symlist);

#endif
