/**
 * @file newscanner.c
 * 
 * @brief implementation of FSM machine scanner
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
#include "newscanner.h"
#include <stdio.h>
#include <ctype.h>

/***********************************************************************************************************
 * *********************************************************************************************************
 * *********    these variables are used globally inside the function get_next_token    ********************
************************************************************************************************************/
static int fsm_state;  //state of the finite state machine
static bool isbuff;    //is letter in the buff?
static int buffedchar; //what letter is in the buff?
FILE *fileptr;
Dynamic_string stringbuffer;
bool end = false;
//line,nextline, loadedchar, line_pos?
/****************************    variable end    ***********************************************************/
int get_next_token(TOKEN *tokenptr, TokenList *tokenlist)
{
    int c = 0;
    fsm_state = FSM_START;

    if (tokenptr == NULL)
    {
        return WTF;
    }
    dynamic_string_delete(&stringbuffer); //can be done on the start of the function, or at EVERY EXIT CONDITION
    //dokoncenie predosleho cyklu+podmienky
    //proper case
    while (!end)
    {
        if (isbuff)
        {
            isbuff = false;
            c = buffedchar;
        }
        else
        {
            c = getchar();
        }
        /*test*/
        switch (fsm_state)
        {
        case FSM_START:
            /*test*/
            if (isspace(c) && c != '\n')
            {
                continue;
            } /*isspace covers a lot more*/
            else if (c == '\n')
            {
                tokenptr->tokentype = TOKEN_TYPE_EOL;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            } /*else ignore*/
            else if (c == '/')
            {
                fsm_state = FSM_SLASH;
            }
            else if (c == '\"')
            {
                fsm_state = FSM_STRING;
            }
            else if (isalpha(c))
            {
                fsm_state = FSM_ID;
                dynamic_string_add_char(&stringbuffer, c);
            }
            else if (c == '_')
            {
                fsm_state = FSM_UNDERLINE;
            }
            else if (c == '=')
            {
                fsm_state = FSM_EQUALSIGN;
            }
            else if (c == '+')
            {
                tokenptr->tokentype = TOKEN_TYPE_ADD;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == ':')
            {
                fsm_state = FSM_COLON;
            }
            else if (c == ';')
            {
                tokenptr->tokentype = TOKEN_TYPE_SEMICOLON;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == '-')
            {
                tokenptr->tokentype = TOKEN_TYPE_SUBTRACT;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == '*')
            {
                tokenptr->tokentype = TOKEN_TYPE_MULTIPLY;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == '(')
            {
                tokenptr->tokentype = TOKEN_TYPE_OPENING_CLASSIC_BRACKET;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == ')')
            {
                tokenptr->tokentype = TOKEN_TYPE_CLOSING_CLASSIC_BRACKET;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == '{')
            {
                tokenptr->tokentype = TOKEN_TYPE_OPENING_CURVY_BRACKET;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == '}')
            {
                tokenptr->tokentype = TOKEN_TYPE_CLOSING_CURVY_BRACKET;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == '!')
            {
                fsm_state = FSM_EXCLAMATION;
            }
            else if (c == '0'){
                fsm_state = FSM_DECNUMBER_ZERO;
                dynamic_string_add_char(&stringbuffer, c);
            }
            else if (isdigit(c))
            { /*test*/
                fsm_state = FSM_DECNUMBER;
                dynamic_string_add_char(&stringbuffer, (char)c); /*test*/
            }
            else if (c == '<')
            {
                fsm_state = FSM_SMALLERTHAN;
            }
            else if (c == '>')
            {
                fsm_state = FSM_GREATERTHAN;
            }
            else if (c == ',')
            {
                tokenptr->tokentype = TOKEN_TYPE_COMMA;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else if (c == EOF)
            {
                tokenptr->tokentype = TOKEN_TYPE_EOF;
                end = true;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else
            {
                return WTF;
            }

            break;

        case FSM_SLASH:
            if (c == '/')
            {
                fsm_state = FSM_LINE_COMMENT_PROPER;
            }
            else if (c == '*')
            {
                fsm_state = FSM_BLOCK_COMMENT_PROPER;
            }
            else
            { //did not go to any of the
                buffedchar = c;
                isbuff = true;
                tokenptr->tokentype = TOKEN_TYPE_DIVIDE;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            break;

        case FSM_UNDERLINE:
            if (isalnum(c))
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_ID;
            }
            else
            {
                isbuff = true;
                buffedchar = c;
                tokenptr->tokentype = TOKEN_TYPE_UNDERSCORE;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            break;

        case FSM_LINE_COMMENT_PROPER:
            if (c == EOF || c == '\n')
            {
                buffedchar = c;
                isbuff = true;
                fsm_state = FSM_START;
            }
            break;

        case FSM_BLOCK_COMMENT_PROPER:
            if (c == '*')
            {
                fsm_state = FSM_BLOCK_COMMENT_END;
            }
            else if (c == EOF)
            {
                return WTF;
            }
            break;

        case FSM_BLOCK_COMMENT_END:
            if (c == '/')
            {
                fsm_state = FSM_START;
            }
            else if (c == '*')
            {
                fsm_state = FSM_BLOCK_COMMENT_END;
            }
            else if (c == EOF)
            {
                return WTF;
            }
            else
            {
                fsm_state = FSM_BLOCK_COMMENT_PROPER;
            }
            break;

        case FSM_ID:
            if (isalnum(c) || c == '_')
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
            }
            else
            {
                isbuff = true;
                buffedchar = c;
                if (!strcmp(stringbuffer.string, "else"))
                {
                    tokenptr->keyword = KEYWORD_ELSE;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }
                else if (!strcmp(stringbuffer.string, "for"))
                {
                    tokenptr->keyword = KEYWORD_FOR;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }
                else if (!strcmp(stringbuffer.string, "func"))
                {
                    tokenptr->keyword = KEYWORD_FUNC;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }
                else if (!strcmp(stringbuffer.string, "if"))
                {
                    tokenptr->keyword = KEYWORD_IF;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }
                else if (!strcmp(stringbuffer.string, "package"))
                {
                    tokenptr->keyword = KEYWORD_PACKAGE;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }               
                else if (!strcmp(stringbuffer.string, "return"))
                {
                    tokenptr->keyword = KEYWORD_RETURN;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }
                else if (!strcmp(stringbuffer.string, "float64"))
                {
                    tokenptr->keyword = KEYWORD_FLOAT64;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }
                else if (!strcmp(stringbuffer.string, "int"))
                {
                    tokenptr->keyword = KEYWORD_INTEGER;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }
                else if (!strcmp(stringbuffer.string, "string"))
                {
                    tokenptr->keyword = KEYWORD_STRING;
                    tokenptr->tokentype = TOKEN_TYPE_RESERVED_KEYWORD;
                }
                else if (!strcmp(stringbuffer.string, "main"))
                {
                    tokenptr->tokentype = TOKEN_TYPE_MAIN;
                }
                else
                {
                    tokenptr->tokentype = TOKEN_TYPE_IDENTIFIER;
                    dynamic_string_copy(tokenptr, &stringbuffer);
                }
                //dynamic_string_delete(&stringbuffer);
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                //printf("the tokentype: %d, and the keyword %d,\n",tokenptr->tokentype,tokenptr->keyword);
                return OK;
            }
            break;

        case FSM_STRING:
            if (c == '\\')
            {
                fsm_state = FSM_BACKSLASH;
            }
            else if (c == EOF){
                isbuff = true; buffedchar = c;
                return WTF;
            }
            else if (c != '\"')
            {
                dynamic_string_add_char(&stringbuffer, c);
            }
            
            else
            {
                tokenptr->tokentype = TOKEN_TYPE_STRING;
                dynamic_string_copy(tokenptr, &stringbuffer);
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            break;

        case FSM_COLON:
            if (c == '=')
            {
                tokenptr->tokentype = TOKEN_TYPE_DEFINE;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else
            {
                return WTF;
            }
            break;

        case FSM_EXCLAMATION:
            if (c == '=')
            {
                tokenptr->tokentype = TOKEN_TYPE_NOT_EQUAL;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else
            {
                return WTF;
            }
            break;

        case FSM_EQUALSIGN:
            if (c == '=')
            {
                tokenptr->tokentype = TOKEN_TYPE_EQUAL;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else
            {
                isbuff = true;
                buffedchar = c;
                tokenptr->tokentype = TOKEN_TYPE_ASSIGN;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            break;

        case FSM_GREATERTHAN:
            if (c == '=')
            {
                tokenptr->tokentype = TOKEN_TYPE_GREATER_EQUAL;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else
            {
                isbuff = true;
                buffedchar = c;
                tokenptr->tokentype = TOKEN_TYPE_GREATER_THAN;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            break;

        case FSM_SMALLERTHAN:
            if (c == '=')
            {
                tokenptr->tokentype = TOKEN_TYPE_LOWER_EQUAL;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            else
            {
                isbuff = true;
                buffedchar = c;
                tokenptr->tokentype = TOKEN_TYPE_LOWER_THAN;
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            break;

        case FSM_BACKSLASH: //note:sorted by ascii
            if (c == 'a')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\a'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == 'b')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\b'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == 't')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\t'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == 'n')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\n'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == 'v')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\v'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == 'f')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\f'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == 'r')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\r'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == '\\')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\\'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == '\'')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\''))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == '\"')
            {
                if (!dynamic_string_add_char(&stringbuffer, '\"'))
                {
                    return memoryerror;
                }
                fsm_state = FSM_STRING;
            }
            else if (c == 'x')
            {
                fsm_state = FSM_HEXNUMBER_1;
            }
            else
            {
                return WTF;
            }
            break;

        case FSM_DECNUMBER: //any other first number than zero
            ///*test*/
            if (isdigit(c))
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
            }
            else if (c == 'E' || c == 'e')
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXPONENT_OR_SIGN;
            }
            else if (c == '.')
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXP_DEC;
            }
            else
            {
                /*number processing*/
                isbuff = true;
                buffedchar = (char)c;
                tokenptr->tokentype = TOKEN_TYPE_INTEGER;
                tokenptr->integer = atol(stringbuffer.string);
                /*test*/
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
                //maketoken();
            }
            break;
        case FSM_DECNUMBER_IGNORE_REST:
            if (isdigit(c) || c == 'e' || c == 'E' || c == '.'){
                /*ignore*/
            }
            else{
                isbuff = true;buffedchar = c;
                fsm_state = FSM_START;
                return WTF;
            }
            break;
        case FSM_DECNUMBER_ZERO: //only point or exponent after first zero allowed
            if (isdigit(c))
            {
                fsm_state = FSM_DECNUMBER_IGNORE_REST;
            }
            else if (c == 'E' || c == 'e')
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXPONENT_OR_SIGN;
            }
            else if (c == '.')
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXP_DEC;
            }
            else
            {
                isbuff = true;
                buffedchar = c; /*maketoken();number processing*/
                tokenptr->tokentype = TOKEN_TYPE_INTEGER;
                tokenptr->integer = 0;
                if(!save_next_token(tokenlist,tokenptr)){return memoryerror;}
                return OK;
            }
            break;

        case FSM_DECNUMBER_EXPONENT_OR_SIGN: //got letter E, can get +,-,nums
            if (isdigit(c))
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXPONENT_NUMBER;
            }
            else if (c == '+')
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXPONENT_AND_SIGN;
            }
            else if (c == '-')
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXPONENT_AND_SIGN;
            }
            else
            {
                return WTF;
            }
            break;

        case FSM_DECNUMBER_EXPONENT_AND_SIGN: //already got e, passed  where +- was possible, so number is required
            if (isdigit(c))
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXPONENT_NUMBER;
            }
            else
            {
                return WTF;
            }
            break;

        case FSM_DECNUMBER_EXPONENT_NUMBER: //only number in exponent now
            if (isdigit(c))
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
            }
            else
            {
                isbuff = true;
                buffedchar = c;
                make_token_float(tokenptr, stringbuffer.string);
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            break;

        case FSM_DECNUMBER_FLOAT: //already well in decimal places, possible exponent or more numbers
            if (isdigit(c))
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
            }
            else if (c == 'e' || c == 'E')
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_EXPONENT_OR_SIGN;
            }
            else
            {
                isbuff = true;
                buffedchar = c;
                make_token_float(tokenptr, stringbuffer.string);
                if (!save_next_token(tokenlist, tokenptr))
                {
                    return memoryerror;
                }
                return OK;
            }
            break;

        case FSM_DECNUMBER_EXP_DEC: //got point, need at least one number
            if (isdigit(c))
            {
                if (!dynamic_string_add_char(&stringbuffer, c))
                {
                    return memoryerror;
                }
                fsm_state = FSM_DECNUMBER_FLOAT;
            }
            else
            {
                return WTF;
            }
            break;

        case FSM_HEXNUMBER_1: //got \x
            fsm_state = FSM_HEXNUMBER_2;
            buffedchar = c; //shouldnt bother the actual buffer cuz im not setting isbuff to true
            break;

        case FSM_HEXNUMBER_2:
            if (isxdigit(c) && isxdigit(buffedchar))
            {
                char hexnumber[4];
                hexnumber[0] = '0';
                hexnumber[1] = 'x';
                hexnumber[2] = buffedchar;
                hexnumber[3] = c;
                char *endptr = NULL;
                long symbol = strtol(hexnumber, &endptr, 16);
                if (!endptr)
                {
                    return WTF; /*fail*/
                }
                else
                { /*success*/
                    {
                        if (!dynamic_string_add_char(&stringbuffer, symbol))
                        {
                            return memoryerror;
                        }
                    } //note: can input long into int-processing function, since value will never go over 255
                    fsm_state = FSM_STRING;
                }
            }
            else
            {
                return WTF;
            }
            break;
        }
    }

    return 0;
}
/*
####################################################################################################################
###########################   Additional functions   ###############################################################
####################################################################################################################
*/
void set_fsm_state(FSM_STATES input)
{
    fsm_state = input;
}

void reset_list_position(TokenList *t_list)
{
    t_list->head = 0;
}

int load_tokens(TokenList *t_list)
{
    int r_code;
    TOKEN tmp;

    do
    {
        r_code = get_next_token(&tmp, t_list);
        if (r_code)
        {
            return r_code;
        }
    } while (tmp.tokentype != TOKEN_TYPE_EOF);

    return OK;
}

bool dynamic_string_copy(TOKEN *token, Dynamic_string *dynamicstring)
{
    token->string = dynamicstring;
    return true;
}

void make_token_float(TOKEN *t, char *num)
{
    t->tokentype = TOKEN_TYPE_FLOAT64;
    t->floater = atof(num);
}

bool get_next_token_list(TOKEN **token, TokenList *tokenlist)
{
    if (token == NULL || tokenlist == NULL)
    {
        return false;
    }
    //if (token->tokentype == TOKEN_TYPE_STRING){
    //    if (token->string != NULL){
    //        dynamic_string_free(token->string);
    //        free(token->string);
    //        token->string = NULL;
    //    }
    //}
    TokenListItem *foundtokenitem = tokenlist->first;

    for (int i = 0; i < tokenlist->head; i++)
    {
        if (foundtokenitem->next != NULL)
        {
            foundtokenitem = foundtokenitem->next;
        }
        else
        {
            return false;
        }
    }
    tokenlist->head++;
    *token = foundtokenitem->token;
    return true;
}

bool save_next_token(TokenList *tokenlist, TOKEN *token)
{

    TOKEN *newtoken = malloc(sizeof(TOKEN));
    if (!newtoken)
    {
        return false;
    }

    TokenListItem *listitem = malloc(sizeof(TokenListItem));
    if (!listitem)
    {
        free(newtoken);
        return false;
    }
    newtoken->tokentype = TOKEN_TYPE_EMPTY; //just so it is an initialised value. will get rewritten by copystring anyway
    if (!copy_token(newtoken, token))
    {
        free(newtoken);
        free(listitem);
        return false;
    }

    listitem->token = newtoken;
    listitem->next = NULL;

    if (tokenlist->first == NULL)
    {
        tokenlist->first = listitem;
        return true;
    }
    else
    {
        TokenListItem *current = tokenlist->first;
        TokenListItem *previous = NULL;
        while (current != NULL)
        {
            previous = current;
            current = current->next;
        }
        previous->next = listitem;
        return true;
    }
}

void init_token_list(TokenList *tokenlist)
{
    tokenlist->head = 0;
    tokenlist->first = NULL;
}

bool dynamic_string_copy_string(TOKEN *dest, TOKEN *src)
{
    if (((dest->tokentype != TOKEN_TYPE_STRING && dest->tokentype != TOKEN_TYPE_IDENTIFIER) &&
         ((src->tokentype != TOKEN_TYPE_STRING) && src->tokentype != TOKEN_TYPE_IDENTIFIER)))
    {
        //printf("AA\n");
        return false;
    }
    if (dest->string == NULL || src->string == NULL)
    {
        printf("uninitialised string");
        return false;
    }
    while (dest->string->allocated_size < src->string->actual_size)
    {
        if (!dynamic_string_double(dest->string))
        {
            return false;
        }
    }
    strcpy(dest->string->string, src->string->string);
    return true;
}

bool copy_token(TOKEN *dest, TOKEN *src)
{
    if (dest == NULL || src == NULL)
    {
        return false;
    }
    if (dest->tokentype == TOKEN_TYPE_STRING || dest->tokentype == TOKEN_TYPE_IDENTIFIER)
    {
        if (src->tokentype == TOKEN_TYPE_STRING || src->tokentype == TOKEN_TYPE_IDENTIFIER)
        {
            if (!dynamic_string_copy_string(dest, src))
            {
                return false;
            }
            return true;
        }
        else
        {
            dynamic_string_free(dest->string);
            free(dest->string);
        }
    }
    dest->tokentype = src->tokentype;

    if (src->tokentype == TOKEN_TYPE_STRING || src->tokentype == TOKEN_TYPE_IDENTIFIER)
    {
        Dynamic_string *newdynstring = malloc(sizeof(Dynamic_string));
        if (newdynstring == NULL)
        {
            return false;
        }
        if (!dynamic_string_init(newdynstring))
        {
            free(newdynstring);
            return false;
        }
        dest->string = newdynstring;
        if (!dynamic_string_copy_string(dest, src))
        {
            return false;
        }
    }
    else if (src->tokentype == TOKEN_TYPE_INTEGER)
    {
        dest->integer = src->integer;
    }
    else if (src->tokentype == TOKEN_TYPE_FLOAT64)
    {
        dest->floater = src->floater;
    }
    else if (src->tokentype == TOKEN_TYPE_RESERVED_KEYWORD)
    {
        dest->keyword = src->keyword;
    }
    return true;
}

void free_token_list(TokenList *tokenlist)
{
    if (tokenlist == NULL)
    {
        return;
    }
    TokenListItem *current = tokenlist->first;
    TokenListItem *previous = NULL;
    while (current != NULL)
    {
        previous = current;
        current = current->next;
        if (previous->token->tokentype == TOKEN_TYPE_STRING || previous->token->tokentype == TOKEN_TYPE_IDENTIFIER)
        {
            dynamic_string_free(previous->token->string);
            free(previous->token->string);
        }
        free(previous->token);
        free(previous);
    }
    return;
}
