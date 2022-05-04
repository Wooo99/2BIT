/**
 * @file expression.c
 * 
 * @brief Processing of expressions by precedent analysis
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
#include "expression.h"

TermType Result;

void InitStack(MyStack *Stack){
    if(Stack == NULL){
            return;
    }
    else
    {
        *Stack = malloc(sizeof(struct TheStack));
        if((*Stack) == NULL){
            fprintf(stderr,"Memory allocation failed\n");
            return;
        }
        (*Stack)->p = malloc((START_STACK_SIZE)*sizeof(Item));
        if(((*Stack)->p) == NULL){
            return;
        }
        (*Stack)->top = 0;
        (*Stack)->size = START_STACK_SIZE;
    }
    
}

void DisposeStack(MyStack *Stack){
    if((*Stack == NULL || Stack == NULL)){
        return;
    }
    for( int i = (*Stack)->top; i > 0; i--){
        free((*Stack)->p[i]);
    }

    free((*Stack)->p);
    free(*Stack);
    *Stack = NULL;
}

void PushStack(MyStack Stack, Item item){
    Stack->top++;
    if(Stack->size == Stack->top){
        Stack->size = Stack->top + START_STACK_SIZE;
        Stack->p = realloc(Stack->p, sizeof(Item) * Stack->size);
        if(Stack->p == NULL){
                return ;
        }
    }
    Stack->p[Stack->top] = item;
}

Item PopStack(MyStack Stack){
    if(Stack->top == 0){
        return NULL;
    }
    else{
        Stack->top--;
        return Stack->p[Stack->top + 1];
    }
}
//return first term from top
int FirstFoundTerm(MyStack Stack){
    int position ;
    position = Stack->top;
    while(position != 0){
        if(Stack->p[position]->type == IT_TERM){
            return position;
        }
        position--;
    }
    return position;
}
RelType TokenToTerm(TOKEN_TYPES token){
    switch(token){
        case TOKEN_TYPE_ADD:
            return TR_ADD_SUB;
        case TOKEN_TYPE_SUBTRACT:
            return TR_ADD_SUB;
        case TOKEN_TYPE_MULTIPLY:
            return TR_MUL_DIV;
        case TOKEN_TYPE_DIVIDE:
            return TR_MUL_DIV;
        case TOKEN_TYPE_EQUAL:
            return TR_EQUAL;   
        case TOKEN_TYPE_NOT_EQUAL:
            return TR_EQUAL;
        case TOKEN_TYPE_OPENING_CLASSIC_BRACKET:
            return TR_LBRACKET;
        case TOKEN_TYPE_CLOSING_CLASSIC_BRACKET:
            return TR_RBRACKET;
        case TOKEN_TYPE_INTEGER:
            return TR_VALUE;
        case TOKEN_TYPE_FLOAT64:
            return TR_VALUE;
        case TOKEN_TYPE_IDENTIFIER:
            return TR_VALUE;
        case TOKEN_TYPE_STRING:
            return TR_VALUE;
        case TOKEN_TYPE_GREATER_EQUAL:
            return TR_GT_LT;
        case TOKEN_TYPE_GREATER_THAN:
            return TR_GT_LT;
        case TOKEN_TYPE_LOWER_EQUAL:
            return TR_GT_LT;
        case TOKEN_TYPE_LOWER_THAN:
            return TR_GT_LT;
        default:
            return TR_$;

    }
}

Relation PrecedenceTable(RelType First, RelType Second){
    switch(First){
        case TR_ADD_SUB:
            if(Second == TR_LBRACKET || Second == TR_VALUE || Second == TR_MUL_DIV){
                return R_OPEN;
            }
            else if(Second == TR_NOT){
                return R_EMPTY;
            }
            return R_CLOSE;
        case TR_MUL_DIV:
            if(Second == TR_LBRACKET || Second == TR_VALUE){
                return R_OPEN;
            }
            else if(Second == TR_NOT){
                return R_EMPTY;
            }
            return R_CLOSE;
        case TR_EQUAL:
            if(Second == TR_RBRACKET || Second == TR_AND_OR || Second == TR_$ ){
                return R_CLOSE;
            }
            else if(Second == TR_EQUAL){
                return R_EMPTY;
            }
            else return R_OPEN;
        case TR_GT_LT:
            if( Second == TR_RBRACKET ||Second == TR_EQUAL || Second == TR_$ || Second == TR_AND_OR){
                return R_CLOSE;
            }
            else if(Second == TR_GT_LT){
                return R_EMPTY;
            }
            else return R_OPEN;
        case TR_LBRACKET:
            if(Second == TR_$){
                 return R_EMPTY;
            }
            else if(Second == TR_RBRACKET){
                return R_EQUAL;
            }
            else return R_OPEN;
        case TR_RBRACKET:
            if(Second == TR_LBRACKET || Second == TR_VALUE || Second == TR_NOT){
                return R_EMPTY;
            }
            else return R_CLOSE;
        case TR_NOT:
            if(Second == TR_RBRACKET || Second == TR_$ || Second == TR_AND_OR){
                return R_CLOSE;
            }
            else return R_OPEN;
        case TR_VALUE:
            if(Second == TR_LBRACKET || Second == TR_VALUE || Second == TR_NOT){
                return R_EMPTY;
            }
            else return R_CLOSE;
        case TR_AND_OR:
            if(Second == TR_RBRACKET || Second == TR_$){
                return R_CLOSE;
            }
            else return R_OPEN;
        case TR_$:
            if(Second == TR_RBRACKET || Second == TR_$){
                return R_EMPTY;
            }
            else return R_OPEN;
        
    }
    return R_EMPTY;
}
 int CheckWhileR_Close(MyStack stack, SymtableStack *TableList){
    Item item = PopStack(stack);            //vezmeme co máme na stacku
    if(item->type == IT_TERM){             
        if(item->val.term.tokentype == TOKEN_TYPE_CLOSING_CLASSIC_BRACKET){
            free(item);
            item = PopStack(stack);
            Item leftbracket = PopStack(stack);   
            free(leftbracket);
         }
        else{
            char *interpret;
            TermType type = T_UNKNOWN;
            switch(item->val.term.tokentype){               //rozhodneme co za term máme
                case TOKEN_TYPE_INTEGER:
                    type =  T_INT;
                    interpret = malloc(sizeof(char) * 60);
                    if(interpret == NULL){
                        return ERR_ALLOC_M;
                        }
                     int u = item->val.term.integer;
                    sprintf(interpret,"int@%d", u);
                    interpret = realloc(interpret, strlen(interpret));
                    if(interpret == NULL){
                        return ERR_ALLOC_M;
                    }
                    Result = T_INT;                         //uložíme dočasně co máme za term
                break;
                case TOKEN_TYPE_FLOAT64:
                    type = T_FLOAT;
                    interpret = malloc(sizeof(char) * 60);
                    if(interpret == NULL){
                        return ERR_ALLOC_M;
                        }
                    sprintf(interpret,"float@%a",item->val.term.floater);
                    interpret = realloc(interpret, strlen(interpret));
                    if(interpret == NULL){
                        return ERR_ALLOC_M;
                        
                    }
                    Result = T_FLOAT;
                break;
                case TOKEN_TYPE_STRING:
                    Result = T_STRING;
                    int size = strlen(item->val.term.string->string);
                    int pos = 7;
                    int multip = 10;
                    interpret = calloc((size+50),(sizeof(char)));
                    if(interpret == NULL){
                        return ERR_ALLOC_M;
                    }
                    strcpy(interpret,"string@");
                    for(int i =0; i< size; i++ ){
                        if(pos + 4 >= size){
                            multip += 100;
                            interpret = realloc(interpret, sizeof(char)*multip);
                            if(interpret == NULL){
                                return ERR_ALLOC_M;
                            }
                        }
                        if(item->val.term.string->string[i] == ' '){ //mezera ve stringu
                            interpret[pos] = 92;   
                            interpret[pos+1] = '0';
                            interpret[pos+2] = '3';
                            interpret[pos+3] = '2';
                            pos+=4;
                        }
                        else if(item->val.term.string->string[i] == '\\'){  //backslash
                            interpret[pos] = 92; 
                            interpret[pos+1] = '0';
                            interpret[pos+2] = '9';
                            interpret[pos+3] = '2';
                            pos+=4;
                        }
                        else if(item->val.term.string->string[i] == '\n'){  //eol ve stringu
                            interpret[pos] = 92; 
                            interpret[pos+1] = '0';
                            interpret[pos+2] = '1';
                            interpret[pos+3] = '0';
                            pos+=4;
                        }
                        else if(item->val.term.string->string[i] == '#' ){ //hashtag
                            interpret[pos] = 92; 
                            interpret[pos+1] = '0';
                            interpret[pos+2] = '3';
                            interpret[pos+3] = '5';
                            pos+=4;
                        }
                        else {
                            interpret[pos] = item->val.term.string->string[i];
                            pos++;
                        }

                    }
                    type= T_STRING;
                    break;
                case TOKEN_TYPE_IDENTIFIER:
                    if(symstack_lookup(TableList, item->val.term.string->string)== false){
                        free(item);
                        return ERR_ID_UNDEFINED;
                    }
                    else{
                        interpret = malloc(sizeof(char)*(strlen(item->val.term.string->string)+5));
                            if(interpret == NULL){
                                return ERR_ALLOC_M;
                                ;
                            }
                        Symtable_item *temp = symstack_lookup(TableList, item->val.term.string->string);
                        type = temp->dataType;
                        Result = temp->dataType;
                        strcpy(interpret, temp->codename);
                        break;
                    }  
                case TOKEN_TYPE_EMPTY:
                    interpret = malloc(sizeof(char)*7);
                        if(interpret == NULL){
                            return ERR_ALLOC_M;
                            ;
                        }
                    strcpy(interpret, "nil@nil");
                    type = T_EMPTY;
                    break;
                default:
                    free(item);
                    return ERR_INVALID_EXPRESSION;
                    ;
                
            }
            if(item->val.term.tokentype == TOKEN_TYPE_IDENTIFIER){
                fprintf(out_file,"PUSHS LF@%s\n",interpret);
            }
            else fprintf(out_file,"PUSHS %s\n",interpret);
            item->type = IT_NONTERM;
            free(interpret);
            item->val.type = type;
        }
        free(PopStack(stack));
        PushStack(stack,item);
        return 0; //item na zásobník

    }
    Item RightItem = item;          
    item  = PopStack(stack);
    Item  LeftItem = PopStack(stack);

    TermType RightType = RightItem->val.type;
    TermType LeftType = LeftItem->val.type;
    TermType ResultType = RightType;


    bool OnlyOne;
    if(LeftItem->type != IT_OPEN){
        OnlyOne = false;
    }
    else OnlyOne = true;
    bool Same = false;
    //rozhodování o druhu result typu
    TOKEN_TYPES Type = item->val.term.tokentype;  

    if(OnlyOne == false){
        if( (LeftType == T_UNKNOWN ) || (RightType == T_UNKNOWN) ){
            Result = LeftType;
        }
        if(LeftType == RightType){
            Same = true;
            Result = LeftType;
        }
        else if (LeftType == T_UNKNOWN)
        {
            Same = true;
            Result = RightType;
        }
        else if (RightType == T_UNKNOWN)
        {
            ResultType = LeftType;
            Same = true;
            Result = LeftType;
        }
        else if (LeftType == T_FLOAT && RightType == T_INT){
            Same = false;
            Result = LeftType;

        }
        else if (LeftType == T_INT && RightType == T_FLOAT){
            Same = false;
            Result = RightType;
        }
    }
    //kontrola správnosti typů
    switch(Type){
        case TOKEN_TYPE_ADD:
            if((ResultType != T_FLOAT && ResultType != T_INT && ResultType != T_STRING && ResultType != T_UNKNOWN)){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
                if(LeftType == RightType){

                }
            }
            if(Same == false && OnlyOne == false){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            if(LeftType == T_STRING && RightType == T_STRING){
                fprintf(out_file,"DEFVAR LF@$tmp1\n");
                fprintf(out_file,"DEFVAR LF@$tmp2\n");
                fprintf(out_file,"POPS LF@$tmp1\n");
                fprintf(out_file,"POPS LF@$tmp2\n");
                fprintf(out_file,"CONCAT LF@$tmp2 LF@$tmp2 LF@$tmp1\n");
                fprintf(out_file,"PUSHS LF@$tmp1\n");
            }
            else
            {
                fprintf(out_file,"ADDS\n");  
           }
            
        break;
        case TOKEN_TYPE_SUBTRACT:        
            if((ResultType != T_FLOAT && ResultType != T_INT && ResultType != T_UNKNOWN)){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            if(Same == false && OnlyOne == false){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            fprintf(out_file,"SUBS\n");
        break;
        case TOKEN_TYPE_MULTIPLY:
            if(Same == false){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            if((ResultType != T_FLOAT && ResultType != T_INT && ResultType != T_UNKNOWN)){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            fprintf(out_file, "MULS\n");
        break;
        case TOKEN_TYPE_DIVIDE:
            if(Same == false){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            if((ResultType != T_FLOAT && ResultType != T_INT && ResultType != T_UNKNOWN)){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            if(RightItem->val.term.integer == 0){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_ZERO_DIVISION;
            }
            if((LeftType == RightType) && (LeftType == T_FLOAT)){
                Result = T_FLOAT;
                fprintf(out_file,"DIVS\n");
            }
            else if((LeftType == RightType) && (LeftType == T_INT)){
                Result = T_INT;
                fprintf(out_file,"IDIVS\n");
            }
        break;
        case TOKEN_TYPE_EQUAL:
        case TOKEN_TYPE_NOT_EQUAL:
            if((ResultType != T_FLOAT && ResultType != T_INT && ResultType != T_STRING && ResultType != T_UNKNOWN)){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            else if(Same == false){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            else if(LeftType != RightType){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            Result = T_BOOL;
            fprintf(out_file,"EQS\n");
            if(Type != TOKEN_TYPE_EQUAL){
                fprintf(out_file,"NOTS\n");
            }
        break;
        case TOKEN_TYPE_LOWER_THAN:
        case TOKEN_TYPE_GREATER_THAN:
        case TOKEN_TYPE_LOWER_EQUAL:
        case TOKEN_TYPE_GREATER_EQUAL:
            Result = T_BOOL;
            if((ResultType != T_FLOAT && ResultType != T_INT && ResultType != T_STRING && ResultType != T_UNKNOWN)){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            if(Same == false){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
             else if(LeftType != RightType){
                free(RightItem);
                free(LeftItem);
                free(item);
                return ERR_TYPE_COMB;
            }
            if(Type == TOKEN_TYPE_LOWER_THAN){
                fprintf(out_file,"LTS\n");
            }
            else if(Type == TOKEN_TYPE_GREATER_THAN){
                fprintf(out_file,"GTS\n");
            }
            else if(Type == TOKEN_TYPE_GREATER_EQUAL){
                fprintf(out_file,"LTS\n");
                fprintf(out_file,"NOTS\n");
                
            }
            else if(Type == TOKEN_TYPE_LOWER_EQUAL){
                fprintf(out_file,"GTS\n");
                fprintf(out_file,"NOTS\n");
            }
        break;
        default:
            free(RightItem);
            free(LeftItem);
            free(item);
            return ERR_TYPE_COMB;
    }
    
    free(RightItem);
    free(LeftItem);
    item->val.type = Result;
    item->type = IT_NONTERM;
    if(OnlyOne == false){
        free(PopStack(stack));
    }
    PushStack(stack, item);

    return 0;
}

int StartExpr(SymtableStack *TableList, TermType *type){
    get_token(&curr_token);
    int reading = 1;
    MyStack stack;
    InitStack(&stack);
    while(reading > 0){
        RelType curr;
        RelType new;
        Item item;

        int FirstTermPosition = FirstFoundTerm(stack);
        if(FirstTermPosition == 0){ 
            curr=TR_$;
        }
        else{
            curr=TokenToTerm(stack->p[FirstTermPosition]->val.term.tokentype);
        }
        new = TokenToTerm(curr_token->tokentype);
        //Podle precedenční tabulky rozhhodneme o realci
        Relation ResultRelation = PrecedenceTable(curr,new);
        int temporary;
        switch (ResultRelation)
        {
            case R_CLOSE:
                //v close se děje výpočet a kontrola
                temporary = CheckWhileR_Close(stack,TableList);
                if(temporary == ERR_ALLOC_M){
                    return ERR_ALLOC_M;
                }
                else if(temporary == ERR_ID_UNDEFINED){
                    return ERR_ID_UNDEFINED;
                }
                else if(temporary == ERR_TYPE_COMB){
                    return ERR_TYPE_COMB;
                }
                else if(temporary == ERR_ZERO_DIVISION){
                    return ERR_ZERO_DIVISION;
                }
                else if(temporary == ERR_INVALID_EXPRESSION){
                    return ERR_INVALID_EXPRESSION;
                }
                break;
            case R_OPEN:
                //v open se pouze pushne na stack
                PushStack(stack,NULL);
                int position = FirstTermPosition + 1;
                for(int i = stack->top-1; i >= position; i-- ){     //udělat místo a posunout
                    stack->p[i+1]=stack->p[i];
                }
                Item New = malloc(sizeof(struct item));
                if(New == NULL) {
                    return ERR_ALLOC_M;
                }
                New->type = IT_OPEN;
                stack->p[position] = New;
                item = malloc(sizeof(struct item));
                if(item == NULL) {
                    return ERR_ALLOC_M;
                }
                item->type = IT_TERM;
                item->val.term = *curr_token;
                PushStack(stack, item);
                get_token(&curr_token);
                break;
            case R_EQUAL:
                item = malloc(sizeof(struct item));
                if(item == NULL){
                    return ERR_ALLOC_M;
                }
                item->type = IT_TERM;
                item->val.term = *curr_token;
                PushStack(stack,item);
                get_token(&curr_token);
                break;
            case R_EMPTY:
                if(curr == new && curr == TR_$){   
                    if(stack->top != 0){
                        reading = 0; //konec výrazu
                    }
                    else{
                        reading = -3; //výraz nemůže být prázdný
                    }
                }
                else{
                    if(stack->top == 1){
                        if(new == TR_RBRACKET){
                                reading = 0;
                                return_token(curr_token);
                        }
                    }
                    else {
                    reading = -4;
                    }
                }   
        }
    }
    if(reading == 0){
        *type = Result;
    }
    DisposeStack(&stack);
    return_token(curr_token);
    if (reading == -1){
        return ERR_INVALID_EXPRESSION;
    }

    if(reading == -3){
        return ERR_EMPTY_EXP;
    }
    if(reading == -4){
        return ERR_EXP_ORDER;
    } 
    if(Result != T_BOOL && (result_here[0] != '\0')){ 
    fprintf(out_file,"POPS %s\n", result_here);
    }
    return reading;         //stav v jakém skončilo převádění
}

int s_expr(SymtableStack *TableList, TermType *type){
    return StartExpr(TableList, type);  // TODO not good
}
