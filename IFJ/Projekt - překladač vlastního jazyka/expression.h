/**
 * @file expression.h
 * 
 * @brief Header file for processing of expressions, using precedent analysis
 * 
 * @date 28.11.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */
#ifndef EXPRESSION_H
#define EXPRESSION_H
#include "table_hierarchy.h"
#include "symtable.h"
#include "error.h"
#include "newscanner.h"
#include "syntax_common.h"
#include <stdio.h>
//#include "types_definition.h" temporary disabled

#define START_STACK_SIZE 32


/*************TYPE OF RELATION************/
typedef enum{
    TR_MUL_DIV,
    TR_ADD_SUB,
    TR_NOT,
    TR_AND_OR,
    TR_GT_LT,     
    TR_EQUAL,      
    TR_VALUE,
    TR_LBRACKET,
    TR_RBRACKET,      
    TR_$
} RelType;
/*************RELATION************/
typedef enum{
    R_OPEN,       //<
    R_CLOSE,      //>
    R_EQUAL,      //=
    R_EMPTY       //doesn't exist
} Relation;
/*************TYPE OF ITEM ON STACK*************/
typedef enum{
    IT_TERM,        
    IT_NONTERM,
    IT_OPEN
}ItemType;

typedef union{
    TOKEN term;
    TermType type;
}ItemVal;
/*************ITEM IN STACK***********/

typedef struct item{
    ItemType type;
    ItemVal val;
}*Item;
/*************STACK*************/
typedef struct TheStack{
    int top;            //index of highest item
    int size;           //size of stack
    Item *p;           //array of items
}*MyStack;


void InitStack(MyStack *Stack);                           //stack initialization

void DisposeStack(MyStack *Stack);                        //free the stack and its memory

void PushStack(MyStack Stack, Item item);                 //put item to top of the stack

Item PopStack(MyStack Stack);                             //delete the item at the top of the stack and returns it

int FirstFoundTerm(MyStack Stack);                       //find first terminal from top

RelType TokenToTerm(TOKEN_TYPES token);

Relation PrecedenceTable(RelType First, RelType Second);  //with precedence table return relation

int CheckWhileR_Close(MyStack stack, SymtableStack *TableList);

int StartExpr(SymtableStack *TableList, TermType *type); // added temporary function in order for gcc to shut up

#endif /* EXPRESSION_H */
