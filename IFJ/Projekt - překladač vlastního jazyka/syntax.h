/**
 * @file syntax.h
 * 
 * @brief Header file for syntax analysis
 * 
 * @date 26.11.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * 
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */
#ifndef SYNTAX_H
#define SYNTAX_H
// ---------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>
#include "newscanner.h"
#include "expression.h"
#include "symtable.h"
#include "syntax_common.h"
#include "table_hierarchy.h"
// ########################## global variables #########################

/**
 * @brief Used for counting how many items are on the left of the list assignment
 * Initialised on 1 because id needs to pass 1 item before entering required state.
 */
extern unsigned int assign_list_id_n;

/**
 * @brief Used for counting how many items are on the right of the list assignment
 * 
 */
extern unsigned int assign_list_expr_n;

/**
 * @brief Temporary holding space for IDs on the left side of assignment.
 * 
 */
extern Symtable_item id_list[MAX_N_RET_VAL];

/**
 * @brief Number of elements in id_list
 * 
 */
extern int id_list_n;

/**
 * @brief Body of which function is curently being analysed
 * 
 */
extern char *curr_func;

// ################### end of global variables #################

// ############################# helper functions ###########################

/**
 * @brief Check if next token is eol and increment line
 * 
 */
bool eol_required();

/**
 * @brief Put built in functions to the table
 * 
 * @param table Table for variables to be put in
 * @return int Error code, 0 if ok
 */
int initialise_predefined(Symtable *table);

/**
 * @brief Copies return parameters from the curr_func to the id_list. 
 * 
 * Used in s_return()
 * 
 * @param symlist List of symtables
 * @return int Error code, 0 if ok
 */
int copy_to_id(SymtableStack *symlist);

/**
 * @brief Frees all strings allocated by copy_to_id()
 * 
 */
void free_copied_id();

// ################### end of helper functions #################

// ############################# STATES ##################################
/**
 * @brief Beginning of the program
 * 
 * <prolog> -> package main <eol> <eols> <f_list> <eols> <eof>
 * 
 * @return int 
 */
int s_prolog(SymtableStack *symlist);

/**
 * @brief List of functions
 * 
 * <f_list> -> <eols> func <func> <eols> <f_list>
 * <f_list> -> e
 * 
 * @return int 
 */
int s_f_list(SymtableStack *symlist);

/**
 * @brief Function
 * 
 * <func> -> id <f_init> <body>
 * <func> -> main <f_init> <body>
 * 
 * @return int 
 */
int s_func(SymtableStack *symlist);

/**
 * @brief Lists defining function parameters and return types
 * 
 * <f_init> -> (<param_def_list>) <ret_t_list>
 * 
 * @return int 
 */
int s_f_init(SymtableStack *symlist, char *func_id);

/**
 * @brief Function call
 * 
 * <f_call> -> <param_list>
 * 
 * @return int 
 */
int s_f_call(SymtableStack *symlist, Symtable_item *func_def);

/**
 * @brief Body of the function/loop/condition
 * 
 * <body> -> {<eol> <eols> <stat_list>} <eol> <eols>
 * 
 * @return int 
 */
int s_body(SymtableStack *symlist, FuncItemData *func_ptr);

/**
 * @brief List of function parameters - defining
 * 
 * <param_def_list> -> id <type> <param_def_list_n>
 * <param_def_list> -> e
 * 
 * @return int
 */
int s_param_def_list(SymtableStack *symlist, char *func_id);

/**
 * @brief Multi-parameter list continuation - definition
 * 
 * <param_def_list_n> -> , id <type> <eols> <param_def_list_n>
 * <param_def_list_n> -> e
 *
 * @return int 
 */
int s_param_def_list_n(SymtableStack *symlist, char *func_id);

/**
 * @brief List of function return parameters
 * 
 * <ret_t_list> -> (<type> <ret_t_list_n>)
 * <ret_t_list> -> e
 * 
 * @return int 
 */
int s_ret_t_list(SymtableStack *symlist, char *func_id);

/**
 * @brief Multi-return values list continuation - definition
 * 
 * <ret_t_list_n> -> , <type> <ret_t_list_n>
 * <ret_t_list_n> -> e
 *
 * @return int 
 */
int s_ret_t_list_n(SymtableStack *symlist, char *func_id);

/**
 * @brief Statement: f_call/id_n/for/if
 * 
 * <stat> -> <if>
 * <stat> -> <for>
 * <stat> -> <return>
 * <stat> -> id <id_n>
 * <stat> -> e
 * 
 * @return int 
 */
int s_stat(SymtableStack *symlist);

/**
 * @brief List of statements
 * 
 * <stat_list> -> <stat> <eol> <eols> <stat_list>
 * <stat_list> -> e
 * 
 * @return int 
 */
int s_stat_list(SymtableStack *symlist);

/**
 * @brief Condition
 * 
 * <if> -> if <expr><body><else>
 * 
 * @return int 
 */
int s_if(SymtableStack *symlist);

/**
 * @brief Else statement after condition
 * 
 * <else> -> else <body>
 * <else> -> e
 * 
 * @return int 
 */
int s_else(SymtableStack *symlist);

/**
 * @brief For loop
 * 
 * <for> -> for <id_def_v>; <expr>; <id_assign_v> <body>
 * 
 * @return int 
 */
int s_for(SymtableStack *symlist);

/**
 * @brief Return statement
 * 
 * <return> -> return <expr_list>
 * 
 * @return int 
 */
int s_return(SymtableStack *symlist);

/**
 * @brief List of expressions
 * 
 * <expr_list> -> <expr> <expr_list_n>
 * <expr_list> -> e
 * 
 * @return int 
 */
int s_expr_list(SymtableStack *symlist);

/**
 * @brief Continuation of expression list
 * 
 * <expr_list_n> -> ,<expr> <expr_list_n>
 * <expr_list_n> -> e
 * 
 * @return int 
 */
int s_expr_list_n(SymtableStack *symlist, int n);

/**
 * @brief Id was found in the statement - crossroads
 * 
 * <id_n> -> := <id_def>
 * <id_n> -> = <id_assign>
 * <id_n> -> , <id_list_assign>
 * <id_n> -> ( <f_call> )
 * 
 * @return int 
 */
int s_id_n(SymtableStack *symlist, char *id);

/**
 * @brief Variable definition
 * 
 * <id_def> -> <expr>
 * 
 * @return int 
 */
int s_id_def(SymtableStack *symlist, char *id);

/**
 * @brief Voluntary variable definition
 * 
 * <id_def_v> -> <id_def>
 * <id_def_v> -> e
 * 
 * @return int 
 */
int s_id_def_v(SymtableStack *symlist);

/**
 * @brief List of IDs, must be assignment
 * 
 * <id_list> -> id <id_list_n>
 * 
 * @return int 
 */
int s_id_list(SymtableStack *symlist);

/**
 * @brief Continuation of ID list
 * 
 * <id_list_n> -> ,id <id_list_n>
 * <id_list_n> -> e
 * 
 * @return int 
 */
int s_id_list_n(SymtableStack *symlist);

/**
 * @brief Assignment of the ID/s
 * 
 * <id_assign> -> <expr>
 * 
 * @return int 
 */
int s_id_assign(SymtableStack *symlist);

/**
 * @brief Voluntary assignment of the ID
 * 
 * <id_assign_v> -> <assign>
 * <id_assign_v> -> e
 * 
 * @return int 
 */
int s_id_assign_v(SymtableStack *symlist);

/**
 * @brief Assignment of list to list
 *
 * <id_list_assign> -> <id_list> = <expr_list>
 * 
 * @return int
 */
int s_id_list_assign(SymtableStack *symlist);

/**
 * @brief Assigning values to id/s from function.
 * 
 * @param func_def Function, from which we are assigning
 * @return int Return code (see syntax_common.h error_codes)
 */
int s_func_assign(Symtable_item *func_def);

/**
 * @brief List of parameters of function call
 * 
 * <param_list> -> <expr> <param_list_n>
 * 
 * @return int 
 */
int s_param_list(SymtableStack *symlist, Symtable_item *func_def);

/**
 * @brief Continuation of function call parameters
 * 
 * <param_list_n> -> , <expr> <param_list_n>
 * <param_list_n> -> e
 * 
 * @return int 
 */
int s_param_list_n(SymtableStack *symlist, Symtable_item *func_def, int n);

/**
 * @brief Variable type
 * 
 * <type> -> int
 * <type> -> float64
 * <type> -> string
 * 
 * @return int 
 */
int s_type(DataType *type);

/**
 * @brief Right side of the value assignment if it is not a function
 * 
 * @param symlist List of symtables
 * @return int Error code, 0 if ok
 */
int s_expr_list_assign(SymtableStack *symlist);

/**
 * @brief Right side of the value assignment if it is not a function
 * 
 * @param symlist List of symtables
 * @param n Position of id in list
 * @return int Error code, 0 if ok
 */
int s_expr_list_assign_n(SymtableStack *symlist, int n);

/**
 * @brief Evaluates expression and returns type in parameter type
 * 
 * @param TableList List of symtables
 * @param type Pointer to variable in which type of expression will be stored
 * @return int Error code, 0 if ok
 */
int s_expr(SymtableStack *TableList, TermType *type); // compatibility
// ############################# STATES END ###############################

#endif /* SYNTAX_H */
