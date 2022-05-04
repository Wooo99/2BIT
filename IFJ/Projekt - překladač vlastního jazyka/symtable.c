/**
 * @file symtable.c
 * 
 * @brief implementation of symbol table
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
#include <string.h>
#include <stdlib.h>
#include "symtable.h"
#include <stdio.h>
#include "dynamic_string.h"
unsigned long hash(char *str)
{
    unsigned long hash = 0;
    //unsigned long x = 0;
    for (char letter = *str; letter != '\0'; letter = *(++str))
    {
        hash = (hash << 4) + letter;
    }
    return hash % MAX_SIZE_OF_SYM_TABLE;
}

void symtable_init(Symtable *symtable)
{
    if (symtable == NULL)
    {
        return;
    }
    for (int i = 0; i < MAX_SIZE_OF_SYM_TABLE; i++)
    { //NULL ALL
        (*symtable)[i] = NULL;
    }
}

Symtable_item *symtable_add(Symtable *table, char *key, bool *noerror)
{
    if (table == NULL || key == NULL)
    { //initial check
        *noerror = false;
        return NULL;
    }
    //make sure symbol is not in the table already
    unsigned long hash_index = hash(key);
    Symtable_item *tmp = (*table)[hash_index];
    while (tmp != NULL)
    {
        if (strcmp(tmp->key, key) == 0)
        {
            *noerror = true;
            return NULL;
        }
        tmp = tmp->next;
    }

    //if not found, make new one
    Symtable_item *new_item = malloc(sizeof(Symtable_item)); //MALLOC
    if (new_item == NULL)
    {
        *noerror = false;
        return NULL;
    }

    new_item->key = (char *)malloc(sizeof(char) * (strlen(key) + 1)); //MALLOC
    if (new_item->key == NULL)
    {
        free(new_item);
        *noerror = false;
        return NULL;
    }

    strcpy(new_item->key, key);
    //put in table
    tmp = (*table)[hash_index];
    (*table)[hash_index] = new_item;
    new_item->next = tmp;
    new_item->dataType = T_UNKNOWN;
    new_item->codename = NULL;
    *noerror = true;
    return new_item;
}

Symtable_item *symtable_add_int(Symtable *table, char *key, int value)
{
    bool internalnoerror = false;
    Symtable_item *item = symtable_add(table, key, &internalnoerror);
    if (!item && internalnoerror)
    {
        item = symtable_search(table, key);
    }
    if (!internalnoerror)
    {
        return NULL;
    }
    item->itemData.intnumber = value;
    item->dataType = T_INT;
    return item;
}

Symtable_item *symtable_add_float(Symtable *table, char *key, float value, bool *noerror)
{
    bool internalnoerror = false;
    Symtable_item *item = symtable_add(table, key, &internalnoerror);
    if (!item && internalnoerror)
    {
        item = symtable_search(table, key);
    }
    if (!internalnoerror)
    {
        *noerror = false;
        return NULL;
    }

    item->itemData.floatnumber = value;
    item->dataType = T_FLOAT;
    *noerror = true;
    return item;
}

Symtable_item *symtable_add_string(Symtable *table, char *key, char *value, bool *noerror)
{
    bool internalnoerror = false;
    Symtable_item *item = symtable_add(table, key, &internalnoerror);
    if (!item && internalnoerror)
    {
        item = symtable_search(table, key);
    }
    if (!internalnoerror)
    {
        *noerror = false;
        return NULL;
    }

    item->dataType = T_STRING;

    Dynamic_string *strptr = malloc(sizeof(Dynamic_string));
    if (!strptr)
    {
        *noerror = false;
        return NULL;
    }

    internalnoerror = dynamic_string_init(strptr);
    if (!internalnoerror)
    {
        free(strptr);
        *noerror = false;
        return NULL;
    }

    internalnoerror = dynamic_string_add_string(strptr, value);
    if (!internalnoerror)
    {
        dynamic_string_free(strptr);
        free(strptr);
        *noerror = false;
        return NULL;
    }

    item->itemData.dynamicstring = strptr;
    *noerror = true;
    return item;
}

Symtable_item *symtable_add_function_init(Symtable *table, char *key)
{
    bool internalnoerror = false;
    Symtable_item *item = symtable_add(table, key, &internalnoerror);
    if (!item && internalnoerror)
    {
        item = symtable_search(table, key);
    }
    if (!internalnoerror)
    {
        return NULL;
    }
    item->dataType = T_FUNC;

    /*ItemData * itemdata = malloc(sizeof(ItemData));
    if (!itemdata){
        if(item){symtable_remove(table,key);}
        noerror = false; 
        return NULL;}*/

    FuncItemData *funcitemdataptr = malloc(sizeof(FuncItemData));
    if (!funcitemdataptr)
    {
        if (item)
        {
            symtable_remove(table, key);
        }
        return NULL;
    }

    funcitemdataptr->var_param = false;

    funcitemdataptr->return_types = malloc(sizeof(DataType) * DEFAULT_PARAM_COUNT);
    if (!funcitemdataptr->return_types)
    {
        if (item)
        {
            symtable_remove(table, key);
        }
        free(funcitemdataptr);
        //free(itemdata);
        return NULL;
    }
    funcitemdataptr->param_names = malloc(sizeof(char *) * DEFAULT_PARAM_COUNT);
    if (!funcitemdataptr->param_names)
    {
        if (item)
        {
            symtable_remove(table, key);
        }
        free(funcitemdataptr->return_types);
        free(funcitemdataptr);
        //free(itemdata);
        return NULL;
    }
    funcitemdataptr->param_types = malloc(sizeof(DataType) * DEFAULT_PARAM_COUNT);
    if (!funcitemdataptr->param_types)
    {
        if (item)
        {
            symtable_remove(table, key);
        }
        free(funcitemdataptr->return_types);
        free(funcitemdataptr->param_types);
        free(funcitemdataptr);
        //free(itemdata);
        return NULL;
    }
    funcitemdataptr->alloc_param = DEFAULT_PARAM_COUNT;
    funcitemdataptr->alloc_return = DEFAULT_PARAM_COUNT;
    funcitemdataptr->used_param = 0;
    funcitemdataptr->used_return = 0;
    item->itemData.funcitemptr = funcitemdataptr;
    return item;
}

/**
 * @note volaj len na spravne inicializovane
 * */
Symtable_item *Symtable_add_function_inparam(Symtable *table, char *key, char *paramname, DataType paramtype)
{
    //get what to change
    bool inbool = false;
    Symtable_item *item = symtable_add(table, key, &inbool);
    if (!item && inbool)
    {
        item = symtable_search(table, key);
    }
    if (!inbool)
    {
        return NULL;
    }
    if (item->dataType != T_FUNC)
    {
        return NULL;
    }
    FuncItemData *lePtr = item->itemData.funcitemptr;
    //realloc array of pointers to longer, if needed
    if (lePtr->alloc_param == lePtr->used_param)
    {
        char **tmp = realloc(lePtr->param_names, sizeof(char *) * lePtr->alloc_param * 2);
        if (tmp == NULL)
        {
            return NULL;
        }
        else
        {
            lePtr->param_names = tmp;
        }

        DataType *tmp2 = realloc(lePtr->param_types, sizeof(DataType) * lePtr->alloc_param * 2);
        if (tmp2 == NULL)
        {
            return NULL;
        }
        else
        {
            lePtr->param_types = tmp2;
        }

        lePtr->alloc_param = lePtr->alloc_param * 2;
    }
    //alloc new strings and copy in
    lePtr->param_names[lePtr->used_param] = malloc(sizeof(char) * (strlen(paramname) + 1));
    if (!lePtr->param_names[lePtr->used_param])
    {
        return NULL;
    }
    strcpy(lePtr->param_names[lePtr->used_param], paramname);

    lePtr->param_types[lePtr->used_param] = paramtype;

    lePtr->used_param++;
    //return
    return item;
}

Symtable_item *Symtable_add_function_outparam(Symtable *table, char *key, DataType returntype)
{
    bool inbool = false;
    Symtable_item *item = symtable_add(table, key, &inbool);
    if (!item && inbool)
    {
        item = symtable_search(table, key);
    }
    if (!inbool)
    {
        return NULL;
    }
    if (item->dataType != T_FUNC)
    {
        return NULL;
    }
    FuncItemData *lePtr = item->itemData.funcitemptr;

    if (lePtr->alloc_return == lePtr->used_return)
    {
        DataType *tmp = realloc(lePtr->return_types, sizeof(DataType) * lePtr->alloc_return * 2);
        if (tmp == NULL)
        {
            return NULL;
        }
        else
        {
            lePtr->return_types = tmp;
        }
        lePtr->alloc_return = lePtr->alloc_return * 2;
    }

    lePtr->return_types[lePtr->used_return] = returntype;

    lePtr->used_return++;
    return item;
}

void Symtable_set_var_param(Symtable *table, char *key){
    if (table == NULL){
        return;
    }

    Symtable_item *sym_item = symtable_search(table, key);

    if(sym_item == NULL){
        return;
    }

    sym_item->itemData.funcitemptr->var_param = true;
}

Symtable_item *symtable_search(Symtable *table, char *key)
{
    unsigned long hash_index = hash(key);
    Symtable_item *tmp = (*table)[hash_index];
    while (tmp != NULL)
    {
        if (strcmp(tmp->key, key) == 0)
        {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}
void symtable_remove(Symtable *table, char *key)
{
    unsigned long hash_index = hash(key);
    Symtable_item *current = (*table)[hash_index];
    Symtable_item *previous = NULL;
    bool found = false;
    while (current != NULL)
    {
        if (strcmp(key, current->key) == 0)
        {
            found = true;
            break;
        }
        previous = current;
        current = current->next;
    }
    if (found == false)
    {
        return;
    }

    //link the other items in table
    if (previous == NULL)
    {                                         //if item was first on hash
        (*table)[hash_index] = current->next; //works even if only one in list on the hash
    }
    else
    { //item was not first in list
        previous->next = current->next;
    }
    //free item
    if (current->dataType == T_FUNC)
    {
        FuncItemData_free(current->itemData.funcitemptr);
    }
    else if (current->dataType == T_STRING)
    {
        dynamic_string_free(current->itemData.dynamicstring);
        free(current->itemData.dynamicstring);
    }
    free(current->key); //FREE
    if (current->codename != NULL)
        free(current->codename);
    free(current);      //FREE
    return;
}

void FuncItemData_free(FuncItemData *data)
{
    if (data == NULL)
    {
        return;
    }
    for (int i = 0; i < data->used_param; i++)
    {
        free(data->param_names[i]);
    }

    free(data->param_names);
    free(data->param_types);
    free(data->return_types);
    free(data);
    return;
}

void symtable_free(Symtable *table)
{
    Symtable_item *current;
    Symtable_item *next;
    for (int i = 0; i < MAX_SIZE_OF_SYM_TABLE; i++)
    {
        current = (*table)[i];
        while (current != NULL)
        {
            next = current->next;
            if (current->dataType == T_FUNC)
            {
                FuncItemData_free(current->itemData.funcitemptr);
            }
            else if (current->dataType == T_STRING)
            {
                dynamic_string_free(current->itemData.dynamicstring);
                free(current->itemData.dynamicstring);
            }
            
            free(current->key);
            free(current);
            
            current = next;
        }
        (*table)[i] = NULL; //is it really needed?
    }
}
void printouttable(Symtable *table)
{
    Symtable_item *item;
    for (int i = 0; i < MAX_SIZE_OF_SYM_TABLE; i++)
    {
        int count = 0;
        item = (*table)[i];
        while (item != NULL)
        {
            printf("->found on hash %d, index %d : %s,", i, count, item->key);
            if (item->dataType == T_STRING)
            {
                printf("%s, ", item->itemData.dynamicstring->string);
            }
            else if (item->dataType == T_FLOAT)
            {
                printf("%f, ", item->itemData.floatnumber);
            }
            else if (item->dataType == T_INT)
            {
                printf("%d, ", item->itemData.intnumber);
            }
            else if (item->dataType == T_FUNC)
            {
                printf("\n \treturn types:");
                for (int i = 0; i < item->itemData.funcitemptr->used_return; i++)
                {
                    printf("\t\t%d, \n", item->itemData.funcitemptr->return_types[i]);
                }
                printf("\n \tparam names:");
                for (int i = 0; i < item->itemData.funcitemptr->used_param; i++)
                {
                    printf("\t\t%s, \n", item->itemData.funcitemptr->param_names[i]);
                }
                printf("\n \tparam types:");
                for (int i = 0; i < item->itemData.funcitemptr->used_param; i++)
                {
                    printf("\t\t%d, \n", item->itemData.funcitemptr->param_types[i]);
                }
            }
            else
            {
                printf("failed to load data of this item.");
            }
            item = item->next;
            count++;
        }
        if (count == 0)
        {
            printf("NOTHING at hash %d", i);
        }
        //item = NULL;
        printf("\n");
    }
}
