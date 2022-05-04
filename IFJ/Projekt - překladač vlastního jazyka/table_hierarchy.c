/**
 * @file table_hierarchy.c
 * 
 * @brief implementation of hierarchical system of tables.
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
#include <stdlib.h>
#include "table_hierarchy.h"
#include <stdio.h> //debug only

bool stackInit(SymtableStack *symtablestack)
{
    //alloc the array proper
    if (symtablestack == NULL)
    {
        return false;
    }
    symtablestack->table = malloc(sizeof(Symtable *) * DEFAULT_STACK_SIZE);
    if (symtablestack->table == NULL)
    {
        return false;
    }
    symtablestack->size = DEFAULT_STACK_SIZE;
    symtablestack->top = -1; //-1 == empty
    for (int i = 0; i < symtablestack->size; i++)
    {
        symtablestack->table[i] = NULL;
    }
    return true;
}

bool stackDoubleSize(SymtableStack *symtablestack)
{
    //basic check
    if (symtablestack == NULL)
    {
        return false;
    }
    //try to realloc, do not overwrite the previous thing if it fails
    void *tmp = realloc(symtablestack->table, sizeof(Symtable) * symtablestack->size * 2);
    if (tmp == NULL)
    {
        return false;
    } //save if successful, change size of it
    symtablestack->table = tmp;
    symtablestack->size = symtablestack->size * 2;
    return true;
}

bool stackEmpty(SymtableStack *symtablestack)
{
    if (symtablestack->top == -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool stackFull(SymtableStack *symtablestack)
{
    if (symtablestack->top == symtablestack->size - 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool stackPush(SymtableStack *symtablestack, Symtable *table)
{
    if (symtablestack == NULL || table == NULL)
    {
        return false;
    }
    if (stackFull(symtablestack))
    {
        bool tmpbool = stackDoubleSize(symtablestack);
        if (tmpbool == false)
        {
            return false;
        }
    }
    symtablestack->table[symtablestack->top + 1] = table;
    symtablestack->top++;
    return true;
}

Symtable *stackPop(SymtableStack *symtablestack)
{
    if (stackEmpty(symtablestack))
    {
        return NULL;
    }
    else
    {
        symtablestack->top--;
        return symtablestack->table[symtablestack->top + 1];
    }
}
void stackFree(SymtableStack *symtablestack)
{
    for (int i = 0; i < symtablestack->top + 1; i++)
    {
        symtable_free(symtablestack->table[i]);
    }
    free(symtablestack->table);
    symtablestack->size = 0;
    symtablestack->top = -1;
}

Symtable *stackTop(SymtableStack *symlist)
{
    if (stackEmpty(symlist))
    {
        return NULL;
    }
    return symlist->table[symlist->top];
}

Symtable *stackBottom(SymtableStack *symlist)
{
    if (stackEmpty(symlist))
    {
        return NULL;
    }
    return symlist->table[0];
}



Symtable_item *symstack_add_to_last(SymtableStack *symlist, char *id, TermType type)
{
    Symtable *table = stackTop(symlist);
    if (table == NULL){
        return NULL;
    }
    bool error;
    switch (type)
    {
    case T_INT:
        return symtable_add_int(table, id, 1); // TODO value does not have to be saved
        break;

    case T_FLOAT:
        return symtable_add_float(table, id, 1.0, &error);
        break;

    case T_STRING:
        return symtable_add_string(table, id, "\0", &error);
        break;

    default:
        return NULL;
        break;
    }
}
