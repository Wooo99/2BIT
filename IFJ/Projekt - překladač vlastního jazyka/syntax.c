/**
 * @file syntax.c
 * 
 * @brief implementation of syntax analysis
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
#include "syntax.h"

// ########################## global variables #########################
unsigned int assign_list_id_n = 1;
unsigned int assign_list_expr_n = 0;
Symtable_item id_list[MAX_N_RET_VAL];
int id_list_n = 0;
char *curr_func = NULL;
int labelUn = 1;

// ############################# helper functions ###########################

bool eol_required()
{
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_EOL)
    {
        return_token(curr_token);
        return false;
    }

    line++;
    return true;
}

int initialise_predefined(Symtable *table)
{
    // func inputs() (string,int)
    if (!symtable_add_function_init(table, "inputs"))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "inputs", T_STRING))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "inputs", T_INT))
        return INTERN_ERROR;

    // func inputi() (int,int)
    if (!symtable_add_function_init(table, "inputi"))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "inputi", T_INT))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "inputi", T_INT))
        return INTERN_ERROR;

    // func inputf() (float64,int)
    if (!symtable_add_function_init(table, "inputf"))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "inputf", T_FLOAT))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "inputf", T_INT))
        return INTERN_ERROR;

    // func int2float(i int) (float64)
    if (!symtable_add_function_init(table, "int2float"))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "int2float", "i", T_INT))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "int2float", T_FLOAT))
        return INTERN_ERROR;

    // func float2int(f float64) (int)
    if (!symtable_add_function_init(table, "float2int"))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "float2int", "f", T_FLOAT))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "float2int", T_INT))
        return INTERN_ERROR;

    // func len(𝑠 string) (int)
    if (!symtable_add_function_init(table, "len"))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "len", "s", T_STRING))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "len", T_INT))
        return INTERN_ERROR;

    // func substr(s string, i int, n int) (string, int)
    if (!symtable_add_function_init(table, "substr"))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "substr", "s", T_STRING))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "substr", "i", T_INT))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "substr", "n", T_INT))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "substr", T_STRING))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "substr", T_INT))
        return INTERN_ERROR;

    // func ord(s string, i int) (int, int)
    if (!symtable_add_function_init(table, "ord"))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "ord", "s", T_STRING))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "ord", "i", T_INT))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "ord", T_INT))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "ord", T_INT))
        return INTERN_ERROR;

    // func chr(i int) (string, int)
    if (!symtable_add_function_init(table, "chr"))
        return INTERN_ERROR;
    if (!Symtable_add_function_inparam(table, "chr", "i", T_INT))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "chr", T_STRING))
        return INTERN_ERROR;
    if (!Symtable_add_function_outparam(table, "chr", T_INT))
        return INTERN_ERROR;

    // print(...)
    if (!symtable_add_function_init(table, "print"))
        return INTERN_ERROR;
    Symtable_set_var_param(table, "print");

    // underscore
    if (!symtable_add_int(table, "_", 0)) // TODO Not sure about the type
        return INTERN_ERROR;

    return 0;
}

int copy_to_id(SymtableStack *symstack)
{
    Symtable_item *func = symstack_lookup(symstack, curr_func);
    if (func == NULL || func->dataType != T_FUNC)
    {
        return ERR_SYMTABLE;
    }
    for (int i = 0; i < func->itemData.funcitemptr->used_return; i++)
    {
        id_list[i].dataType = func->itemData.funcitemptr->return_types[i]; // not ideal but whatever
        char *new_ret = calloc(12, sizeof(char));
        sprintf(new_ret, "__ret_%03i__", i);
        id_list[i].codename = new_ret;
        id_list[i].key = new_ret;
        id_list_n++;
    }

    return 0;
}

void free_copied_id()
{
    for (int i = 0; i < id_list_n; i++)
        free(id_list[i].codename);
}

// ################### end of helper functions #################

// ############################# STATES ##################################

int s_prolog(SymtableStack *symstack)
{
    s_eols();
    get_token(&curr_token);

    // package
    if (curr_token->tokentype != TOKEN_TYPE_RESERVED_KEYWORD ||
        curr_token->keyword != KEYWORD_PACKAGE)
    {
        return ERR_PROLOG;
    }

    // main
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_MAIN)
    {
        return ERR_PROLOG;
    }

    // eol
    if (!eol_required())
    {
        return ERR_EOL_EXPECTED;
    }

    // <eols>
    s_eols();

    if (first_pass)
        return NO_ERR;

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, ".IFJcode20\n");
    fprintf(out_file, "CREATEFRAME\n");
    fprintf(out_file, "JUMP main\n");
    // ------------------------------------------

    // <f_list>
    int r_code = s_f_list(symstack);

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "EXIT int@0\n");
    // ------------------------------------------
    if (r_code)
    {
        return r_code;
    }

    // <eols>
    s_eols();

    // eof
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_EOF)
    {
        return ERR_EOF_EXPECTED;
    }

    return NO_ERR;
} // ---------- end of s_prolog() -------------

int s_f_list(SymtableStack *symstack)
{
    // eof
    get_token(&curr_token);
    if (curr_token->tokentype == TOKEN_TYPE_EOF)
    {
        return_token(curr_token);
        return NO_ERR;
    }

    // func
    if (curr_token->tokentype != TOKEN_TYPE_RESERVED_KEYWORD ||
        curr_token->keyword != KEYWORD_FUNC)
    {
        return ERR_FUNC_EXPECTED;
    }

    // <func>
    int r_code = s_func(symstack);
    if (r_code)
    {
        return r_code;
    }

    // <eols>
    s_eols();

    // f_list
    return s_f_list(symstack);
}

int s_func(SymtableStack *symstack)
{
    Symtable_item *new_func = NULL;

    //main || id
    get_token(&curr_token);
    if (curr_token->tokentype == TOKEN_TYPE_MAIN)
    {
        curr_func = "main";
        if (first_pass == true)
        {
            // check possible redefinition
            if (symstack_lookup(symstack, "main"))
            {
                return ERR_FUNC_REDEFINITION;
            }

            // add function to the symtable
            new_func = symtable_add_function_init(stackBottom(symstack), curr_func);
            if (!new_func)
            {
                return ERR_SYMTABLE;
            }
        }
    }
    else if (curr_token->tokentype == TOKEN_TYPE_IDENTIFIER)
    {
        curr_func = curr_token->string->string;
        if (first_pass == true)
        {
            // check possible redefinition
            if (symstack_lookup(symstack, curr_func))
            {
                return ERR_FUNC_REDEFINITION;
            }
            // add to symtable
            new_func = symtable_add_function_init(stackBottom(symstack), curr_func);
            if (!new_func)
            {
                return ERR_SYMTABLE;
            }
        }
    }
    else
    {
        return ERR_FUNC_ID_EXPECTED;
    }

    // <f_init>
    int r_code = s_f_init(symstack, curr_func);
    if (r_code)
    {
        return r_code;
    }

    // during first pass only function headers are interesting
    if (first_pass)
    {
        return NO_ERR;
    }

    // <s_body>
    new_func = symstack_lookup(symstack, curr_func);
    // if lookup fails, it is symtable failure because function should already be
    // in the symtable from the first pass
    if (new_func)
    {
        // ---------------- CODE-GEN ----------------
        fprintf(out_file, "# ---------- func %s ----------\n", curr_func);
        fprintf(out_file, "LABEL %s\n", curr_func);
        fprintf(out_file, "PUSHFRAME\n");
        for (int i = 0; i < new_func->itemData.funcitemptr->used_return; i++)
            fprintf(out_file, "DEFVAR LF@__ret_%03i__\n\n", i);
        // ------------------------------------------

        r_code = s_body(symstack, new_func->itemData.funcitemptr);

        if (strcmp("main", curr_func) != 0)
        {
            // ---------------- CODE-GEN ----------------
            fprintf(out_file, "POPFRAME\n");
            fprintf(out_file, "RETURN\n");
            // ------------------------------------------
        }
    }
    else
    {
        return ERR_SYMTABLE;
    }

    return r_code;
}

int s_f_init(SymtableStack *symstack, char *func_id)
{
    // (
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_OPENING_CLASSIC_BRACKET)
    {
        return ERR_F_PAR_L_BRACKET;
    }

    // <param_def_list>
    int r_code = s_param_def_list(symstack, func_id);
    if (r_code)
    {
        return r_code;
    }

    // )
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_CLOSING_CLASSIC_BRACKET)
    {
        return ERR_F_PAR_R_BRACKET;
    }

    // <ret_t_list>
    r_code = s_ret_t_list(symstack, func_id);

    return r_code;
}

int s_f_call(SymtableStack *symstack, Symtable_item *func_def)
{
    // param_def_list
    get_token(&curr_token);
    return_token(curr_token);

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "\n# ---------- function call ----------\n");
    fprintf(out_file, "CREATEFRAME\n");
    // ------------------------------------------

    if (TOKEN_IS_NOT(TOKEN_TYPE_OPENING_CLASSIC_BRACKET))
    {
        return ERR_UNEXPECTED_TOKEN;
    }
    int r_code = s_param_list(symstack, func_def);
    if (r_code)
        return r_code;
    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "CALL %s\n", func_def->key);
    fprintf(out_file, "# -------- end function call --------\n\n");
    // ------------------------------------------

    return NO_ERR;
}

int s_body(SymtableStack *symstack, FuncItemData *func_ptr)
{
    // new body, new symtable
    Symtable new_table;
    symtable_init(&new_table);
    stackPush(symstack, &new_table);

    // ---------------- CODE-GEN ----------------
    // save old and generate new postfix
    unsigned int old_postfix = postfix;
    postfix_next();
    // ------------------------------------------

    // in case the body is the body of the function, inject func params to the symtable
    if (func_ptr != NULL)
    {
        for (int i = 0; i < func_ptr->used_param; i++)
        {
            symstack_add_to_last(symstack, func_ptr->param_names[i], func_ptr->param_types[i]);
            add_codename(symstack, func_ptr->param_names[i], func_ptr->param_names[i]);
        }
    }

    // {
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_OPENING_CURVY_BRACKET)
    {
        symtable_free(stackPop(symstack));
        return ERR_BODY_START;
    }

    // eol
    if (!eol_required())
    {
        symtable_free(stackPop(symstack));
        return ERR_EOL_EXPECTED;
    }

    // <eols>
    s_eols();

    // <stat_list>
    int r_code = s_stat_list(symstack);
    if (r_code)
    {
        symtable_free(stackPop(symstack));
        return r_code;
    }

    // }
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_CLOSING_CURVY_BRACKET)
    {
        symtable_free(stackPop(symstack));
        return ERR_BODY_START;
    }

    // free last symtable
    symtable_free(stackPop(symstack));

    // ---------------- CODE-GEN ----------------
    // restore old postfix
    postfix = old_postfix;
    // ------------------------------------------

    return NO_ERR;
}

int s_param_def_list(SymtableStack *symstack, char *func_id)
{
    // e
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CLASSIC_BRACKET))
    {
        return_token(curr_token);
        return NO_ERR;
    }

    // id
    if (TOKEN_IS_NOT(TOKEN_TYPE_IDENTIFIER))
    {
        return ERR_FUNC_DEF_UNEXPECTED;
    }
    char *id = curr_token->string->string;

    // <type>
    DataType type;
    int r_code = s_type(&type);
    if (r_code)
    {
        return r_code;
    }

    if (first_pass == true)
    {
        Symtable_add_function_inparam(stackBottom(symstack), func_id, id, type);
    }

    // <param_def_list_n>
    r_code = s_param_def_list_n(symstack, func_id);

    return r_code;
}

int s_param_def_list_n(SymtableStack *symstack, char *func_id)
{
    // e
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CLASSIC_BRACKET))
    {
        return_token(curr_token);
        return NO_ERR;
    }

    // ,
    if (TOKEN_IS_NOT(TOKEN_TYPE_COMMA))
    {
        return ERR_FUNC_DEF_UNEXPECTED;
    }

    // eols
    s_eols();

    // id
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_IDENTIFIER))
    {
        return ERR_ID_EXPECTED;
    }
    char *id = curr_token->string->string;

    // <type>
    DataType type;
    int r_code = s_type(&type);
    if (r_code)
    {
        return r_code;
    }

    if (first_pass == true)
    {
        Symtable_add_function_inparam(stackBottom(symstack), func_id, id, type);
    }

    // <param_def_list_n>
    r_code = s_param_def_list_n(symstack, func_id);

    return r_code;
}

int s_ret_t_list(SymtableStack *symstack, char *func_id)
{
    // e
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_OPENING_CURVY_BRACKET))
    {
        return_token(curr_token);
        return NO_ERR;
    }

    // (
    if (TOKEN_IS_NOT(TOKEN_TYPE_OPENING_CLASSIC_BRACKET))
    {
        return ERR_FUNC_DEF_RET_UNEXPECTED;
    }

    // )
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CLASSIC_BRACKET))
    {
        return NO_ERR;
    }

    return_token(curr_token);

    // <type>
    DataType type;
    int r_code = s_type(&type);
    if (r_code)
    {
        return r_code;
    }

    if (first_pass == true)
    {
        Symtable_add_function_outparam(stackBottom(symstack), func_id, type);
    }

    // <ret_t_list_n>
    r_code = s_ret_t_list_n(symstack, func_id);
    if (r_code)
    {
        return r_code;
    }

    // )
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_CLOSING_CLASSIC_BRACKET))
    {
        return ERR_FUNC_DEF_RET_UNEXPECTED;
    }

    return NO_ERR;
}

int s_ret_t_list_n(SymtableStack *symstack, char *func_id)
{
    // e
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CLASSIC_BRACKET))
    {
        return_token(curr_token);
        return NO_ERR;
    }
    //,
    if (TOKEN_IS_NOT(TOKEN_TYPE_COMMA))
    {
        return ERR_FUNC_DEF_RET_UNEXPECTED;
    }

    // <type>
    DataType type;
    int r_code = s_type(&type);
    if (r_code)
    {
        return r_code;
    }

    if (first_pass == true)
    {
        Symtable_add_function_outparam(stackBottom(symstack), func_id, type);
    }

    // <ret_t_list_n>
    r_code = s_ret_t_list_n(symstack, func_id);
    if (r_code)
    {
        return r_code;
    }

    return NO_ERR;
}

int s_stat(SymtableStack *symstack)
{
    get_token(&curr_token);
    // <stat> -> e
    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CURVY_BRACKET))
    {
        return_token(curr_token);
        return NO_ERR;
    }
    // <stat> -> id <id_n>
    if (TOKEN_IS(TOKEN_TYPE_IDENTIFIER))
    {
        return s_id_n(symstack, curr_token->string->string);
    }
    if (TOKEN_IS(TOKEN_TYPE_UNDERSCORE))
    {
        return s_id_n(symstack, "_");
    }
    if (TOKEN_IS_NOT(TOKEN_TYPE_RESERVED_KEYWORD))
    {
        return ERR_STAT_UNEXPECTED;
    }

    switch (curr_token->keyword)
    {
    // <if>
    case KEYWORD_IF:
        return_token(curr_token);
        return s_if(symstack);
        break;

    // <stat> -> <for>
    case KEYWORD_FOR:
        return_token(curr_token);
        return s_for(symstack);
        break;

    // <stat> -> <return>
    case KEYWORD_RETURN:
        return_token(curr_token);
        int r_code = s_return(symstack);
        if (r_code == ERR_TYPE_MISMATCH)
        {
            return ERR_INVALID_RETURN;
        }
        return r_code;
        break;

    default:
        return ERR_STAT_UNEXPECTED;
        break;
    }
}

int s_stat_list(SymtableStack *symstack)
{
    // e
    get_token(&curr_token);
    return_token(curr_token);
    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CURVY_BRACKET))
    {
        return NO_ERR;
    }

    // <stat>
    int r_code = s_stat(symstack);
    if (r_code)
    {
        return r_code;
    }

    // eol
    if (!eol_required())
    {
        return ERR_EOL_EXPECTED;
    }
    // <eols>
    s_eols();

    // <stat-list>
    r_code = s_stat_list(symstack);

    return r_code;
}

int s_if(SymtableStack *symstack)
{
    //if
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_RESERVED_KEYWORD) || curr_token->keyword != KEYWORD_IF)
    {
        return ERR_IF_EXPECTED;
    }

    labelUn++;
    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "\n# -------------- if -------------\n");

    // <expr>
    TermType type;
    int r_code = s_expr(symstack, &type);

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "PUSHS bool@false\n");
    fprintf(out_file, "JUMPIFEQS else_%09i\n", postfix + labelUn);
    // ------------------------------------------

    if (r_code)
    {
        if (r_code == ERR_EMPTY_EXP)
            return ERR_NO_EXPR;
        return r_code;
    }

    if (type != T_BOOL)
    {
        return ERR_INVALID_EXPRESSION_IF;
    }

    // <body>
    r_code = s_body(symstack, NULL);
    if (r_code)
    {
        return r_code;
    }

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "JUMP endif_%09i\n", postfix + labelUn);
    fprintf(out_file, "LABEL else_%09i\n", postfix + labelUn);
    // ------------------------------------------

    // <else>
    r_code = s_else(symstack);

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "LABEL endif_%09i\n", postfix + labelUn);
    fprintf(out_file, "# -------------- end if -------------\n\n");

    if (r_code)
    {
        return r_code;
    }

    return NO_ERR;
}

int s_else(SymtableStack *symstack)
{
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_RESERVED_KEYWORD) && curr_token->keyword == KEYWORD_ELSE)
    {
        return s_body(symstack, NULL);
    }
    return ERR_MISSING_ELSE;
}

int s_for(SymtableStack *symstack)
{
    // for
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_RESERVED_KEYWORD) || curr_token->keyword != KEYWORD_FOR)
    {
        return ERR_FOR_EXPECTED;
    }

    // <id_def_v>
    int r_code = s_id_def_v(symstack);
    if (r_code)
    {
        return r_code;
    }

    // ;
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_SEMICOLON))
    {
        return ERR_SEMICOLON_EXPECTED;
    }

    labelUn++;
    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "\n# -------------- for loop -------------)\n");
    fprintf(out_file, "LABEL for_%09i\n", postfix + labelUn);
    // ------------------------------------------

    // <expr>
    TermType type;
    r_code = s_expr(symstack, &type);

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "PUSHS bool@false\n");
    fprintf(out_file, "JUMPIFEQS endfor_%09i\n", postfix + labelUn);
    fprintf(out_file, "JUMP for_body_%09i\n", postfix + labelUn);
    // ------------------------------------------

    if (type != T_BOOL)
    {
        return ERR_INVALID_EXPRESSION;
    }

    if (r_code)
    {
        if (r_code == ERR_EMPTY_EXP)
            return ERR_NO_EXPR;
        return r_code;
    }

    // ;
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_SEMICOLON))
    {
        return ERR_SEMICOLON_EXPECTED;
    }

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "LABEL for_assign_%09i\n", postfix + labelUn);
    // ------------------------------------------

    // <id_assign_v>
    r_code = s_id_assign_v(symstack);
    if (r_code)
    {
        return r_code;
    }

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "JUMP for_%09i\n", postfix + labelUn);
    fprintf(out_file, "LABEL for_body_%09i\n", postfix + labelUn);
    // ------------------------------------------

    // <body>
    r_code = s_body(symstack, NULL);

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "JUMP for_assign_%09i\n", postfix + labelUn);
    fprintf(out_file, "LABEL endfor_%09i\n", postfix + labelUn);
    fprintf(out_file, "\n# -------------- end for -------------)\n\n");
    // ------------------------------------------
    return r_code;
}

int s_return(SymtableStack *symstack)
{ //return
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_RESERVED_KEYWORD) || curr_token->keyword != KEYWORD_RETURN)
    {
        return ERR_RETURN_EXPECTED;
    }

    copy_to_id(symstack);
    //expr_list
    int r_code = s_expr_list(symstack);

    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "\nPOPFRAME\n");
    fprintf(out_file, "RETURN\n");
    fprintf(out_file, "# ---------- end func %s ----------\n", curr_func);
    // ------------------------------------------

    free_copied_id();
    return r_code;
}

int s_expr_list(SymtableStack *symstack)
{
    // e
    get_token(&curr_token);
    return_token(curr_token);

    if (TOKEN_IS(TOKEN_TYPE_EOL))
    {
        id_list_n = 0;
        return NO_ERR;
    }

    // empty return
    if (id_list_n == 0)
        return NO_ERR;

    // ---------------- CODE-GEN ----------------
    if (strcmp(id_list[0].key, "_") == 0)
        result_here[0] = '\0';
    else
        sprintf(result_here, "LF@%s", id_list[0].codename); // if _ do not assign result
    // ------------------------------------------

    // <expr>
    TermType type;
    int r_code = s_expr(symstack, &type);
    if (r_code)
    {
        return r_code;
    }

    if (type != id_list[0].dataType && strcmp(id_list[0].key, "_") != 0)
    {
        return ERR_TYPE_MISMATCH;
    }

    // <expr_list_n>
    return s_expr_list_n(symstack, 1);
}

int s_expr_list_n(SymtableStack *symstack, int n)
{
    get_token(&curr_token);

    // e
    if (TOKEN_IS(TOKEN_TYPE_EOL))
    {
        if (n != id_list_n)
        {
            return ERR_NOT_ENOUGH_RVALUES;
        }
        return_token(curr_token);
        id_list_n = 0;
        return NO_ERR;
    }

    // ,
    if (TOKEN_IS_NOT(TOKEN_TYPE_COMMA))
    {
        return ERR_COMMA_EXPECTED;
    }

    // ---------------- CODE-GEN ----------------
    if (strcmp(id_list[n].key, "_") == 0)
        result_here[0] = '\0';
    else
        sprintf(result_here, "LF@%s", id_list[n].codename); // if _ do not assign result
    // ------------------------------------------

    // <expr>
    TermType type;
    int r_code = s_expr(symstack, &type);
    if (r_code)
    {
        return r_code;
    }

    if (n == id_list_n)
    {
        return ERR_TOO_MANY_RVALUES;
    }

    if (type != id_list[n].dataType && strcmp(id_list[0].key, "_") != 0)
    {
        return ERR_TYPE_MISMATCH;
    }

    // <expr_list_n>
    return s_expr_list_n(symstack, n + 1);
}

int s_id_n(SymtableStack *symstack, char *id)
{
    get_token(&curr_token);
    return_token(curr_token);
    int r_code;

    Symtable_item *already_defined = symstack_lookup(symstack, id);

    switch (curr_token->tokentype)
    {
    // :=
    case TOKEN_TYPE_DEFINE:
        if (symstack_lookup_top(symstack, id))
        {
            return ERR_ID_REDEFINITION;
        }
        return s_id_def(symstack, id);
        break;

    // =
    case TOKEN_TYPE_ASSIGN:
        if (!already_defined)
        {
            return ERR_ID_UNDEFINED;
        }
        // assignment copy to first value
        memcpy(id_list, already_defined, sizeof(Symtable_item));
        id_list_n++;

        r_code = s_id_assign(symstack);
        id_list_n = 0;
        return r_code;
        break;

    // ,
    case TOKEN_TYPE_COMMA:
        if (!already_defined)
        {
            return ERR_ID_UNDEFINED;
        }
        // assignment copy to first value
        memcpy(id_list, already_defined, sizeof(Symtable_item));
        id_list_n++;

        r_code = s_id_list_assign(symstack);
        id_list_n = 0;
        return r_code;
        break;

    // (
    case TOKEN_TYPE_OPENING_CLASSIC_BRACKET:
        if (!already_defined)
        {
            return ERR_ID_UNDEFINED;
        }
        else if (already_defined->dataType != T_FUNC)
        {
            return ERR_ID_IS_NOT_FUNC;
        }
        return s_f_call(symstack, already_defined);
        break;

    default:
        return ERR_UNKNOWN_ID_OPERATION;
        break;
    }
}

int s_id_def(SymtableStack *symstack, char *id)
{
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_DEFINE))
    {
        return ERR_ID_DEF_EXPECTED;
    }

    char *new_var = make_codename(id, postfix);
    // ---------------- CODE-GEN ----------------
    fprintf(out_file, "DEFVAR LF@%s\n", new_var);
    // pass target variable
    sprintf(result_here, "LF@%s", new_var);
    // ------------------------------------------

    TermType type;
    int r_code = s_expr(symstack, &type);
    // check if expression evaluation succeeded
    if (r_code)
    {
        // determine correct error code
        if (r_code == ERR_EMPTY_EXP)
            return ERR_NO_EXPR;
        return r_code;
    }

    // add var to symtable
    if (type == T_STRING || type == T_INT || type == T_FLOAT)
    {
        symstack_add_to_last(symstack, id, type);
        add_codename(symstack, id, new_var);
        return r_code;
    }
    return ERR_TYPE_UNDETERMINED;
}

int s_id_def_v(SymtableStack *symstack)
{
    // <id_def>
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_IDENTIFIER))
    {
        char *id = curr_token->string->string;
        return s_id_def(symstack, id);
    }

    // e
    return_token(curr_token);
    return NO_ERR;
}

int s_id_list(SymtableStack *symstack)
{
    get_token(&curr_token);
    Symtable_item *sym_item;

    // id || _
    if (TOKEN_IS_NOT(TOKEN_TYPE_IDENTIFIER))
    {
        if (TOKEN_IS_NOT(TOKEN_TYPE_UNDERSCORE))
        {
            return ERR_ID_EXPECTED;
        }
        else
        // _
        {
            sym_item = symstack_lookup(symstack, "_");
            if (!sym_item)
            {
                return ERR_ID_UNDEFINED;
            }
        }
    }
    else
    // id
    {
        sym_item = symstack_lookup(symstack, curr_token->string->string);
        if (!sym_item)
        {
            return ERR_ID_UNDEFINED;
        }
    }

    // copy value to id_list
    memcpy(id_list + 1, sym_item, sizeof(Symtable_item));
    id_list_n++;

    return s_id_list_n(symstack);
}

int s_id_list_n(SymtableStack *symstack)
{
    // e
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_ASSIGN))
    {
        return_token(curr_token);
        return NO_ERR;
    }

    // ,
    if (TOKEN_IS(TOKEN_TYPE_COMMA))
    {
        Symtable_item *sym_item;

        get_token(&curr_token);
        if (TOKEN_IS_NOT(TOKEN_TYPE_IDENTIFIER))
        {
            if (TOKEN_IS_NOT(TOKEN_TYPE_UNDERSCORE))
            {
                return ERR_ID_EXPECTED;
            }
            else
            // _
            {
                sym_item = symstack_lookup(symstack, "_");
                if (!sym_item)
                {
                    return ERR_ID_UNDEFINED;
                }
            }
        }
        else
        // id
        {
            sym_item = symstack_lookup(symstack, curr_token->string->string);
            if (!sym_item)
            {
                return ERR_ID_UNDEFINED;
            }
        }

        // copy value to id_list
        memcpy(id_list + id_list_n, sym_item, sizeof(Symtable_item));
        id_list_n++;

        return NO_ERR;
    }

    return ERR_UNEXPECTED_TOKEN;
}

int s_id_assign(SymtableStack *symstack)
{
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_ASSIGN))
    {
        return ERR_ID_ASSIGN_EXPECTED;
    }
    TermType type;

    // check if next token is func id
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_IDENTIFIER)
    {
        return_token(curr_token);

        // ---------------- CODE-GEN ----------------
        if (strcmp(id_list[0].key, "_") == 0)
            result_here[0] = '\0';
        else
            sprintf(result_here, "LF@%s", id_list[0].codename);
        // ------------------------------------------

        int r_code = s_expr(symstack, &type);
        if (r_code == ERR_EMPTY_EXP)
            return ERR_NO_EXPR;
        return r_code;
    }

    Symtable_item *item = symstack_lookup(symstack, curr_token->string->string);

    if (item == NULL)
    {
        return ERR_ID_UNDEFINED;
    }
    else if (item->dataType == T_FUNC)
    {

        int r_code = s_func_assign(item);
        if (r_code)
        {
            return r_code;
        }
        r_code = s_f_call(symstack, item);

        // TODO get values from TF
        // ---------------- CODE-GEN ----------------
        for (int i = 0; i < item->itemData.funcitemptr->used_return; i++)
            fprintf(out_file, "MOVE LF@%s TF@__ret_%03i__\n", id_list[i].codename, i);
        // ------------------------------------------

        return r_code;
    }

    return_token(curr_token);
    // ---------------- CODE-GEN ----------------
    sprintf(result_here, "LF@%s", id_list[0].codename);
    // ------------------------------------------
    return s_expr(symstack, &type);
}

int s_id_assign_v(SymtableStack *symstack)
{
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_OPENING_CURVY_BRACKET))
    {
        return_token(curr_token);
        return NO_ERR;
    }

    if (TOKEN_IS_NOT(TOKEN_TYPE_IDENTIFIER))
    {
        return ERR_ID_ASSIGN_EXPECTED;
    }

    Symtable_item *item = symstack_lookup(symstack, curr_token->string->string);
    if (!item)
        return ERR_ID_UNDEFINED;

    memcpy(id_list, item, sizeof(Symtable_item));
    return s_id_assign(symstack);
}

int s_id_list_assign(SymtableStack *symstack)
{
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_COMMA))
    {
        return ERR_UNEXPECTED_TOKEN;
    }

    int r_code = s_id_list(symstack);
    if (r_code)
    {
        return r_code;
    }

    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_ASSIGN))
    {
        return ERR_UNEXPECTED_TOKEN;
    }

    // check if next token is func id
    get_token(&curr_token);
    if (curr_token->tokentype != TOKEN_TYPE_IDENTIFIER)
    {
        return_token(curr_token);
        // TODO check validity and number of the asignees
        return s_expr_list(symstack);
    }
    // it is identifier, but we don`t know if it is a function
    Symtable_item *item = symstack_lookup(symstack, curr_token->string->string);

    // it does not exist
    if (item == NULL)
    {
        return ERR_ID_UNDEFINED;
    }
    // it is a function
    else if (item->dataType == T_FUNC)
    {
        r_code = s_func_assign(item);
        if (r_code)
        {
            return r_code;
        }
        r_code = s_f_call(symstack, item);
        // ---------------- CODE-GEN ----------------
        for (int i = 0; i < item->itemData.funcitemptr->used_return; i++)
        {
            if (strcmp(id_list[i].key, "_") != 0)
                fprintf(out_file, "MOVE LF@%s TF@__ret_%03i__\n", id_list[i].codename, i);
        }
        // ------------------------------------------
        return r_code;
    }
    // it is not a function
    else
    {
        return_token(curr_token);
        return s_expr_list(symstack);
    }

    return_token(curr_token);
    return s_expr_list_assign(symstack);
}

int s_param_list(SymtableStack *symstack, Symtable_item *func_def)
{
    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_OPENING_CLASSIC_BRACKET))
    {
        return ERR_UNEXPECTED_TOKEN;
    }

    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CLASSIC_BRACKET))
    {
        fprintf(out_file, "DEFVAR TF@__param_n__\n"); // holds number of parameters
        fprintf(out_file, "MOVE TF@__param_n__ int@%i\n", 0);
        return NO_ERR;
    }
    return_token(curr_token);

    TermType type;
    // ---------------- CODE-GEN ----------------
    char *param_name;
    if (func_def->itemData.funcitemptr->var_param)
    {
        char tmp[11];
        sprintf(tmp, "__par%03d__", 0);
        param_name = tmp;
    }
    else
        param_name = func_def->itemData.funcitemptr->param_names[0];

    fprintf(out_file, "DEFVAR TF@%s\n", param_name);
    sprintf(result_here, "TF@%s", param_name);
    // ------------------------------------------
    int r_code = s_expr(symstack, &type);
    if (r_code)
    {
        return r_code;
    }

    // check if parameter is matching with the function fingerprint
    if (!func_def->itemData.funcitemptr->var_param)
    {
        if (func_def->itemData.funcitemptr->used_param == 0)
        {
            if (type != T_UNKNOWN)
            {
                return ERR_WRONG_FUNC_PARAM;
            }
        }
        else if (type != func_def->itemData.funcitemptr->param_types[0])
        {
            return ERR_WRONG_FUNC_PARAM;
        }
    }

    return s_param_list_n(symstack, func_def, 1);
}

int s_param_list_n(SymtableStack *symstack, Symtable_item *func_def, int n)
{
    get_token(&curr_token);
    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CLASSIC_BRACKET))
    {
        fprintf(out_file, "DEFVAR TF@__param_n__\n"); // holds number of parameters
        fprintf(out_file, "MOVE TF@__param_n__ int@%i\n", n);
        return NO_ERR;
    }

    if (TOKEN_IS_NOT(TOKEN_TYPE_COMMA))
    {
        return ERR_UNEXPECTED_TOKEN;
    }

    // ---------------- CODE-GEN ----------------
    char *param_name;
    if (func_def->itemData.funcitemptr->var_param)
    {
        char tmp[11];
        sprintf(tmp, "__par%03d__", n);
        param_name = tmp;
    }
    else
        param_name = func_def->itemData.funcitemptr->param_names[n];

    fprintf(out_file, "DEFVAR TF@%s\n", param_name);
    sprintf(result_here, "TF@%s", param_name);
    // ------------------------------------------

    TermType type;
    int r_code = s_expr(symstack, &type);
    if (r_code)
    {
        return r_code;
    }
    if (!func_def->itemData.funcitemptr->var_param)
    {
        if (func_def->itemData.funcitemptr->used_param <= n)
        {
            if (type != T_UNKNOWN)
            {
                return ERR_WRONG_FUNC_PARAM;
            }
        }
        else if (type != func_def->itemData.funcitemptr->param_types[n])
        {
            return ERR_WRONG_FUNC_PARAM;
        }
        // ---------------- CODE-GEN ----------------
        fprintf(out_file, "PUSHS TF@%s\n", func_def->itemData.funcitemptr->param_names[n]);
        // ------------------------------------------
    }

    return s_param_list_n(symstack, func_def, n + 1);
}

int s_type(DataType *type)
{
    get_token(&curr_token);

    if (TOKEN_IS_NOT(TOKEN_TYPE_RESERVED_KEYWORD))
    {
        return ERR_TYPE_EXPECTED;
    }
    switch (curr_token->keyword)
    {
    case KEYWORD_STRING:
        *type = T_STRING;
        return NO_ERR;
        break;

    case KEYWORD_INTEGER:
        *type = T_INT;
        return NO_ERR;
        break;

    case KEYWORD_FLOAT64:
        *type = T_FLOAT;
        return NO_ERR;
        break;

    default:
        *type = T_UNKNOWN;
        return ERR_TYPE_EXPECTED;
        break;
    }
}

int s_func_assign(Symtable_item *func_def)
{
    if (id_list_n != func_def->itemData.funcitemptr->used_return)
    {
        return ERR_WRONG_NUMBER_LVALUES;
    }

    for (int i = 0; i < id_list_n; i++)
    {
        //TODO check if underscore
        // if (id_list[i].dataType == TOKEN_TYPE_UNDERSCORE)
        // {
        //     continue;
        // }

        // check type
        if (strcmp(id_list[i].key, "_") == 0)
        {
            continue;
        }
        if (func_def->itemData.funcitemptr->return_types[i] != id_list[i].dataType)
        {
            return ERR_TYPE_MISMATCH;
        }
    }

    return NO_ERR;
}

int s_expr_list_assign(SymtableStack *symstack)
{

    get_token(&curr_token);

    TermType type;
    int r_code;

    // <expr>
    // ---------------- CODE-GEN ----------------
    sprintf(result_here, "LF@%s", id_list[0].codename);
    // ------------------------------------------

    r_code = s_expr(symstack, &type);
    if (r_code)
    {
        return r_code;
    }
    // check type
    if (id_list_n > 0)
    {
        if (id_list->dataType == type)
        {

            return s_expr_list_assign_n(symstack, 1);
        }
    }
    return ERR_TYPE_MISMATCH;
}
int s_expr_list_assign_n(SymtableStack *symstack, int n)
{
    if (n == id_list_n)
    {
        return NO_ERR;
    }

    get_token(&curr_token);
    if (TOKEN_IS_NOT(TOKEN_TYPE_COMMA))
    {
        return ERR_COMMA_EXPECTED;
    }

    // ---------------- CODE-GEN ----------------
    sprintf(result_here, "LF@%s", id_list[n].codename);
    // ------------------------------------------

    TermType type;
    int r_code;

    // <expr>
    r_code = s_expr(symstack, &type);
    if (r_code)
    {
        return r_code;
    }
    // check type
    if (id_list->dataType == type)
    {

        return s_expr_list_assign_n(symstack, n + 1);
    }

    return ERR_TYPE_MISMATCH;
}

// ############################# STATES END ###############################
