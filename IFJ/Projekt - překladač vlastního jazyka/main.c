/**
 * @file main.c
 * 
 * @brief main function used for testing purposes
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

#include "newscanner.h"
#include "dynamic_string.h"

void print_token(TOKEN t, int code, FILE *fptr)
{
    const char *t_type_str[] = {
        "TOKEN_TYPE_ADD", 
        "TOKEN_TYPE_SUBTRACT", 
        "TOKEN_TYPE_MULTIPLY", 
        "TOKEN_TYPE_DIVIDE",

        "TOKEN_TYPE_EQUAL",
        "TOKEN_TYPE_NOT_EQUAL", 
        "TOKEN_TYPE_LOWER_THAN", 
        "TOKEN_TYPE_GREATER_THAN", 
        "TOKEN_TYPE_LOWER_EQUAL",
        "TOKEN_TYPE_GREATER_EQUAL", 
        "TOKEN_TYPE_SUB1", 
        "TOKEN_TYPE_ADD1", 

        "TOKEN_TYPE_OPENING_CLASSIC_BRACKET",
        "TOKEN_TYPE_CLOSING_CLASSIC_BRACKET", 
        "TOKEN_TYPE_OPENING_CURVY_BRACKET", 
        "TOKEN_TYPE_CLOSING_CURVY_BRACKET",
        "TOKEN_TYPE_COMMA", 
        "TOKEN_TYPE_SEMICOLON", 
        "TOKEN_TYPE_UNDERSCORE", 
        "TOKEN_TYPE_EOF", 
        "TOKEN_TYPE_EOL",
        "TOKEN_TYPE_IDENTIFIER", 

        "TOKEN_TYPE_RESERVED_KEYWORD",
        "TOKEN_TYPE_STRING", 
        "TOKEN_TYPE_INTEGER", 
        "TOKEN_TYPE_FLOAT64", 
        
        "TOKEN_TYPE_DEFINE", 
        "TOKEN_TYPE_ASSIGN", 
        "TOKEN_TYPE_MAIN",
        "TOKEN_TYPE_EMPTY"};

    const char *keyword_str[] = {
        "KEYWORD_ELSE", //0
        "KEYWORD_FOR", //1
        "KEYWORD_FUNC", //2
        "KEYWORD_IF", //3
        "KEYWORD_PACKAGE",//4
        "KEYWORD_RETURN", //5
        "KEYWORD_NONE"//6
        "KEYWORD_FLOAT64", //7
        "KEYWORD_INTEGER", //8
        "KEYWORD_STRING", //9
        };

    fprintf(fptr,"------New token------\n");
    fprintf(fptr,"Type: %s\n", t_type_str[t.tokentype]);

    if (t.tokentype == TOKEN_TYPE_STRING)
    {
        fprintf(fptr,"Payload: %s\n", t.string->string);
    }
    else if (t.tokentype == TOKEN_TYPE_INTEGER)
    {
        fprintf(fptr,"Payload: %li\n", t.integer);
    }
    else if (t.tokentype == TOKEN_TYPE_FLOAT64)
    {
        fprintf(fptr,"Payload: %f\n", t.floater);
    }
    else if (t.tokentype == TOKEN_TYPE_RESERVED_KEYWORD)
    {
        if (t.keyword == 0)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_ELSE");
        if (t.keyword == 1)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_FOR");
        if (t.keyword == 2)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_FUNC");
        if (t.keyword == 3)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_IF");
        if (t.keyword == 4)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_PACKAGE");
        if (t.keyword == 5)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_RETURN");
        if (t.keyword == 6)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_NONE");
        if (t.keyword == 7)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_FLOAT64");
        if (t.keyword == 8)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_INTEGER");
        if (t.keyword == 9)fprintf(fptr,"Payload %d: %s\n", t.keyword,"KEYWORD_STRING");
    }
    else if (t.tokentype == TOKEN_TYPE_IDENTIFIER)
    {
        fprintf(fptr,"Payload: %s\n", t.string->string);
    }
    fprintf(fptr,"Return code: %i\n",code);
    fprintf(fptr,"---------------------\n");
}

int main(/*int argc, char *argv[]*/)
{
    //if (argc == 2)
    // {
    //     fptr1 = fopen(argv[1], "r");
    //     if (fptr1 == NULL)
    //     {
    //         fputs("File not found.\n", stderr);
    //         return 1;
    //     }
    // }
    // else
    // {
    //     fptr1 = stdin;
    // }
    FILE * fptr1 = fopen("output1.out","w");

    FILE * fptr2 = fopen("output2.out","w");
    if (!fptr2 || !fptr1){
        fputs("File not found.\n", stderr);
        return 1;
    }
    dynamic_string_init(&stringbuffer);

    TOKEN * t = malloc(sizeof(TOKEN));
    t->tokentype = TOKEN_TYPE_EMPTY;
    t->string = NULL;
    TokenList  * list = malloc(sizeof(TokenList)); 
    init_token_list(list);
    int r_code = 0;
    printf("lets start:\n");
    fprintf(fptr1,"this is file containing tokens from original scanner.\n\n");
    fprintf(fptr2,"this is file containing tokens from saved token list.\n\n");
    while (!end )
    {
        //printf("\n\n->");
        r_code = get_next_token(t,list);
        if(r_code == 0)
            print_token(*t, r_code,fptr1);
        else
            fprintf(fptr1,"\n\nLEX_ERROR\n\n");
    }
    while(get_next_token_list(&t,list)){
        print_token(*t,r_code,fptr2);
    }
    free_token_list(list);
    //dynamic_string_free(t->string);
    //free(t->string);
    free(t);
    free(list);
    free(stringbuffer.string);
    fclose(fptr1);
    fclose(fptr2);
    return r_code;
}
