/**
 * @file main_syntax.c
 * 
 * @brief main function for lexical analysis and syntax analysis
 * 
 * @date 27.11.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */
#include "syntax.h"
#include "symtable.h"
#include "table_hierarchy.h"

int main()
{
    //initialisation of the structures
    // out_file = fopen("output.IFJcode20", "w");
    // if (!out_file){
    //     fputs("Failed to open output file.\n", stderr);
    //     return INTERN_ERROR;
    // }
    out_file = stdout;
    dynamic_string_init(&stringbuffer);

    SymtableStack symlist;
    stackInit(&symlist);

    Symtable global;
    symtable_init(&global);
    initialise_predefined(&global);
    stackPush(&symlist, &global);

    init_token_list(&tokens);

    int r_code = load_tokens(&tokens);

    if (r_code)
    {
        // LEX error
        r_code = ERR_LEX;
        goto cleanup;
    }

    // begin check
    // first pass - register all function fingerprints

    r_code = s_prolog(&symlist);
    if (r_code)
        goto cleanup;

    do
    {
        get_token(&curr_token);
        if (curr_token->tokentype == TOKEN_TYPE_RESERVED_KEYWORD)
        {
            if (curr_token->keyword == KEYWORD_FUNC)
            {
                r_code = s_func(&symlist);
                if (r_code)
                {
                    // first pass failed
                    goto cleanup;
                }

                get_token(&curr_token);
                if (TOKEN_IS_NOT(TOKEN_TYPE_OPENING_CURVY_BRACKET))
                {
                    r_code = ERR_BODY_START;
                    goto cleanup;
                }
                int bracket = 1;
                do
                {
                    get_token(&curr_token);
                    if (TOKEN_IS(TOKEN_TYPE_OPENING_CURVY_BRACKET)){
                        bracket++;
                    }
                    if (TOKEN_IS(TOKEN_TYPE_CLOSING_CURVY_BRACKET)){
                        bracket--;
                    }

                } while (bracket != 0 && TOKEN_IS_NOT(TOKEN_TYPE_EOF));

                if (TOKEN_IS(TOKEN_TYPE_EOF))
                {
                    r_code = ERR_BODY_END;
                    goto cleanup;
                }
                s_eols();
            }
        }
        else
        {
            r_code = ERR_FUNC_EXPECTED;
            goto cleanup;
        }
    } while (curr_token->tokentype != TOKEN_TYPE_EOF);

    // prepare for second pass
    first_pass = false;
    line = 1;
    reset_list_position(&tokens);
    token_buffer = NULL;

    Symtable_item *main_f = symstack_lookup(&symlist, "main");
    // check if main() was defined
    if (!main_f)
    {
        r_code = ERR_MAIN_MISSING;
        goto cleanup;
    }
    else
    {
        // and if it has required parameters and return values (NONE)
        if (main_f->itemData.funcitemptr->used_param != 0 || main_f->itemData.funcitemptr->used_return != 0)
        {
            r_code = ERR_INVALID_MAIN_DEFINITION;
            goto cleanup;
        }
    }

    // second pass
    r_code = s_prolog(&symlist);

    if(!r_code)
        print_builtins();

// deallocate all structures in case of an error
cleanup:

    describe_error(r_code);
    fclose(out_file);
    // free all resources
    stackFree(&symlist);
    free(stringbuffer.string);
    free_token_list(&tokens);

    return map_err_code(r_code);
}
