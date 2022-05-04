/**
 * @file main_expression.c
 * 
 * @brief function main for expression evaluation
 * 
 * @date 5.12.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */
#include "expression.h"
#include "symtable.h"
#include "syntax.h"
#include "newscanner.h"

int main()
{
    // initialisation of the structures
    dynamic_string_init(&stringbuffer);

    SymtableStack symlist;
    stackInit(&symlist);

    Symtable global;
    symtable_init(&global);
    initialise_predefined(&global);
    stackPush(&symlist, &global);

    init_token_list(&tokens);

    load_tokens(&tokens);   // can fail

    TermType type;
    int r_code = StartExpr(&symlist, &type);

    printf("Expression evaluation ended, return code: %i\n", r_code);

    // free all resources
    free(stringbuffer.string);
    stackFree(&symlist);
    free_token_list(&tokens);

    return 0;
}
