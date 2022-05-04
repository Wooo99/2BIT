/**
 * @file syntax_common.c
 * 
 * @brief common functions for syntax analysis and for parsing
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
#include "syntax_common.h"

TOKEN *token_buffer = NULL;
TOKEN *curr_token;
unsigned long int line = 1;
TokenList tokens;
bool first_pass = true;
unsigned int postfix = 0;
char result_here[200];
FILE *out_file;

void get_token(TOKEN **token)
{
    if (token_buffer == NULL)
    {
        if (!get_next_token_list(token, &tokens))
        {
            fputs("Something went extremely wrong. Token list corrupted?\n", stderr);
            exit(1);
        }
    }
    else
    {
        *token = token_buffer;
        token_buffer = NULL;
    }
}

void return_token(TOKEN *token)
{
    token_buffer = token;
}

int s_eols()
{
    do
    {
        get_token(&curr_token);
        line++;
    } while (TOKEN_IS(TOKEN_TYPE_EOL));
    line--;
    return_token(curr_token);
    return NO_ERR;
}

void describe_error(ERR_CODE_SYN err)
{
    switch (err)
    {
    case NO_ERR:
        fprintf(stderr, "Your prayers were answered.Probably.\n");
        break;

    case ERR_PROLOG:
        fprintf(stderr, "Line %li: Prolog invalid or missing.\n", line);
        break;

    case ERR_EOL_EXPECTED:
        fprintf(stderr, "Line %li: Unexpected token, EOL expected.\n", line);
        break;

    case ERR_EOF_EXPECTED:
        fprintf(stderr, "Line %li: Unexpected token, EOF expected.\n", line);
        break;

    case ERR_FUNC_EXPECTED:
        fprintf(stderr, "Line %li: Unexpected token, \"func\" expected.\n", line);
        break;

    case ERR_FUNC_ID_EXPECTED:
        fprintf(stderr, "Line %li: Unexpected token, function name expected.\n", line);
        break;

    case ERR_MULTIPLE_MAIN_FUNC:
        fprintf(stderr, "Line %li: Second definition of func main().\n", line);
        break;

    case ERR_F_PAR_L_BRACKET:
        fprintf(stderr, "Line %li: Function parameter declaration. \"(\" expected.\n", line);
        break;

    case ERR_F_PAR_R_BRACKET:
        fprintf(stderr, "Line %li: Function parameter declaration. \")\" expected.\n", line);
        break;

    case ERR_BODY_START:
        fprintf(stderr, "Line %li: {} block. \"{\" expected.\n", line);
        break;

    case ERR_BODY_END:
        fprintf(stderr, "Line %li: {} block. \"}\" expected.\n", line);
        break;

    case ERR_FUNC_DEF_UNEXPECTED:
        fprintf(stderr, "Line %li: Unexpected token in function parameters definition.\n", line);
        break;

    case ERR_FUNC_DEF_RET_UNEXPECTED:
        fprintf(stderr, "Line %li: Unexpected token in function parameters definition.\n", line);
        break;

    case ERR_STAT_UNEXPECTED:
        fprintf(stderr, "Line %li: Statement expected.\n", line);
        break;

    case ERR_IF_EXPECTED:
        fprintf(stderr, "Line %li: \"if\" expected.\n", line);
        break;

    case ERR_FOR_EXPECTED:
        fprintf(stderr, "Line %li: \"for\" expected.\n", line);
        break;

    case ERR_SEMICOLON_EXPECTED:
        fprintf(stderr, "Line %li: \"for\" expected.\n", line);
        break;

    case ERR_RETURN_EXPECTED:
        fprintf(stderr, "Line %li: \"for\" expected.\n", line);
        break;

    case ERR_COMMA_EXPECTED:
        fprintf(stderr, "Line %li: \",\" or EOL expected.\n", line);
        break;

    case ERR_UNKNOWN_ID_OPERATION:
        fprintf(stderr, "Line %li: Unknown operation with id.\n", line);
        break;

    case ERR_ID_DEF_EXPECTED:
        fprintf(stderr, "Line %li: Variable definition expected.\n", line);
        break;

    case ERR_ID_EXPECTED:
        fprintf(stderr, "Line %li: Identifier expected.\n", line);
        break;

    case ERR_UNEXPECTED_TOKEN:
        fprintf(stderr, "Line %li: Unexpected token.\n", line);
        break;

    case ERR_ID_ASSIGN_EXPECTED:
        fprintf(stderr, "Line %li: \" = \" expected.\n", line);
        break;

    case ERR_TYPE_EXPECTED:
        fprintf(stderr, "Line %li: Data type expected.\n", line);
        break;

    case ERR_ID_REDEFINITION:
        fprintf(stderr, "Line %li: Identifier was already defined.\n", line);
        break;

    case ERR_ID_UNDEFINED:
        fprintf(stderr, "Line %li: Undefined reference.\n", line);
        break;

    case ERR_WRONG_FUNC_PARAM:
        fprintf(stderr, "Line %li: Wrong function parameters.\n", line);
        break;

    case ERR_MAIN_MISSING:
        fprintf(stderr, "No main function defined.\n");
        break;

    case ERR_INVALID_MAIN_DEFINITION:
        fprintf(stderr, "Invalid definition of main function.\n");
        break;

    case ERR_INVALID_EXPRESSION:
        fprintf(stderr, "Line %li: Invalid expression.\n", line);
        break;

    case ERR_WRONG_NUMBER_LVALUES:
        fprintf(stderr, "Line %li: Wrong number of lvalues.\n", line);
        break;

    case ERR_TYPE_MISMATCH:
        fprintf(stderr, "Line %li: ID-term type mismatch.\n", line);
        break;

    case ERR_SYMTABLE:
        fprintf(stderr, "Line %li: FUNC_INIT failure. Symtable error has occured.\n", line);
        break;

    case ERR_MISSING_ELSE:
        fprintf(stderr, "Line %li: Else is missing.\n", line);
        break;

    case ERR_TOO_MANY_RVALUES:
        fprintf(stderr, "Line %li: Too many Rvalues.\n", line);
        break;

    case ERR_NOT_ENOUGH_RVALUES:
        fprintf(stderr, "Line %li: Not enough Rvalues.\n", line);
        break;

    case ERR_ID_IS_NOT_FUNC:
        fprintf(stderr, "Line %li: Trying to call a function, but defined id is not a function.\n", line);
        break;

    case ERR_INVALID_EXPRESSION_IF:
        fprintf(stderr, "Line %li: Condition is not valid.\n", line);
        break;

    case ERR_NO_EXPR:
        fprintf(stderr, "Line %li: Expression is empty.\n", line);
        break;

    default:
        fputs("Something is wrong, I can feel it.\n", stderr);
        break;
    }
}

int map_err_code(int code)
{
    // I know that it is probably not the best solution, but I really like it
    if (code < LEXICAL_ERRORS)
        return 0;
    if (code < SYNTAX_ERRORS)
        return LEXICAL_ERROR;
    if (code < SEM_ERR_UNDEFS)
        return SYNTAX_ERROR;
    if (code < SEM_ERR_NEW_VARS)
        return SEM_ERR_UNDEF;
    if (code < SEM_ERR_TYPE_COMPATS)
        return SEM_ERR_NEW_VAR;
    if (code < SEM_ERR_PAR_NUMS)
        return SEM_ERR_TYPE_COMPAT;
    if (code < SEM_ERR_OTHERS)
        return SEM_ERR_PAR_NUM;
    if (code < SEM_ERR_DIV_ZEROS)
        return SEM_ERR_OTHER;
    if (code < INTERN_ERRORS)
        return SEM_ERR_DIV_ZERO;
    else
        return INTERN_ERROR;
}

Symtable_item *symstack_lookup(SymtableStack *stack, char *key)
{
    Symtable_item *symbol = NULL;

    for (int i = 0; i <= stack->top; i++)
    {
        symbol = symtable_search(stack->table[stack->top - i], key);
        if (symbol)
            return symbol;
    }

    return NULL;
}

Symtable_item *symstack_lookup_top(SymtableStack *stack, char *key)
{
    return symtable_search(stack->table[stack->top], key);
}

void postfix_next()
{
    static unsigned int UID = 0;
    postfix = ++UID;
}

// TODO probably not used
void postfix_prev()
{
    postfix--;
}

char *make_codename(char *id, unsigned int postfix)
{
    char *n_name = calloc(sizeof(char), strlen(id) + 10); // 10 because 9 digits are for postfix and 1 for \0
    if (!n_name)
        return NULL;

    sprintf(n_name, "%s_%09i", id, postfix); // null terminator is implicit because of calloc

    return n_name;
}

bool add_codename(SymtableStack *stack, char *key, char *codename)
{
    Symtable_item *item = symstack_lookup(stack, key);
    if (!item){
        return false;
    }

    item->codename = codename;
    return true;
}

void print_builtins(){
    fputs(BUILT_IN_FUNC_CHR, out_file);
    fputs(BUILT_IN_FUNC_FLOAT_2_INT, out_file);
    fputs(BUILT_IN_FUNC_INT_2_FLOAT, out_file);
    fputs(BUILT_IN_FUNC_INPUTF, out_file);
    fputs(BUILT_IN_FUNC_INPUTS, out_file);
    fputs(BUILT_IN_FUNC_INPUTI, out_file);
    fputs(BUILT_IN_FUNC_PRINT, out_file);
    fputs(BUILT_IN_FUNC_LEN, out_file);
    fputs(BUILT_IN_FUNC_SUBSTR, out_file);
    fputs(BUILT_IN_FUNC_ORD, out_file);
}
