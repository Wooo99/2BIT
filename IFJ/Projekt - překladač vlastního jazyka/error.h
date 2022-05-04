/**
 * @file error.h
 * 
 * @brief Compiler error codes. Header file
 * 
 * @date 27.11.2020
 * 
 * @copyright Brno University of Technology, Czech Republic
 * 
 * @author Michal Reznik,    xrezni28, +420 731 748 122
 * 
 * @author Samuel Repka,     xrepka07, +421 907 587 090
 * @author Jiri Hofirek,     xhofir06, +420 774 883 191
 * @author Timotej Kamensky  xkamen24, +421 944 687 328
 * 
 * */


/**
 * @brief 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému).
 * 
 */
#define LEXICAL_ERROR       		1

/**
 * @brief 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, neo- čekávané odřádkování).
 * 
 */
#define SYNTAX_ERROR 				2 

/**
 * @brief 3 - sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefi- nici funkce/proměnné, atp.
 * 
 */
#define SEM_ERR_UNDEF   			3 

/**
 * @brief 4 - sémantická chyba při odvozování datového typu nově definované proměnné.
 * 
 */
#define SEM_ERR_NEW_VAR				4 

/**
 * @brief 5 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech.
 * 
 */
#define SEM_ERR_TYPE_COMPAT	    	5 

/**
 * @brief 6 - sémantickáchybavprogramu–špatný počet/ty pparametrů či návratových hodnot u volání funkce či návratu z funkce.
 * 
 */
#define SEM_ERR_PAR_NUM	    	    6

/**
 * @brief 7 - ostatní sémantické chyby.
 * 
 */
#define SEM_ERR_OTHER		    	7 

/**
 * @brief 9 - sémantická chyba dělení nulovou konstantou.
 * 
 */
#define SEM_ERR_DIV_ZERO	        9

/**
 * @brief 99 - interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti, atd.).
 * 
 */
#define INTERN_ERROR        		99 
	   