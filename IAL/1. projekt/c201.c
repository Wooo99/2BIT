
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                              Úpravy: Kamil Jeřábek, září 2020
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      DisposeList ... zrušení všech prvků seznamu,
**      InitList ...... inicializace seznamu před prvním použitím,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam předá
** někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int errflg;
int solved;

void Error() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList (tList *L) {
	
	L->Act = NULL;
	L->First = NULL;
}

void DisposeList (tList *L) {

	tElemPtr start = L->First;

	while (start != NULL)
	{
		tElemPtr next = start->ptr;
		free(start);
		start = next;
	}
	L->Act = NULL;
	L->First = NULL;
}

void InsertFirst (tList *L, int val) {

	tElemPtr new = malloc(sizeof(struct tElem));
	
	if(new != NULL){
		new->ptr = L->First;
		new-> data = val;
		L->First = new;
	}
	else{
		Error();
		return;
	}
}

void First (tList *L) {

	L->Act = L->First;
}

void CopyFirst (tList *L, int *val) {

	if(L->First == NULL){
		Error();
	}
	else{
	*val = L->First->data;	
	}
}

void DeleteFirst (tList *L) {
	if(L->First == NULL){
		return;
	}
	else if(L->First == L->Act){
		L->Act = NULL;
	}
	tElemPtr new = L->First->ptr;
	free(L->First);
	L->First = new;
}	

void PostDelete (tList *L) {
	
	if(L->Act == NULL){
		return;
	}
	else if(L->Act->ptr == NULL){
		return;
	}
	
	tElemPtr ptr = L->Act->ptr;
	tElemPtr second = ptr->ptr;

	free(L->Act->ptr);
	L->Act->ptr = second;

	
}

void PostInsert (tList *L, int val) {

	if(L->Act != NULL){
		tElemPtr new = malloc(sizeof(struct tElem));
		if( new == NULL){
			Error();
			return;
		}
	new->data = val;	//nahrání obsahu do elementu
	new->ptr = L->Act->ptr;
	L->Act->ptr = new;
	}
	else return;
}

void Copy (tList *L, int *val) {

	if(L->Act == NULL){
		Error();
	}
	else{
	*val = L->Act->data;
	}
}

void Actualize (tList *L, int val) {

	if(L->Act !=NULL){
		L->Act->data = val;
	}
}

void Succ (tList *L) {

	if(L->Act != NULL){
		L->Act = L->Act->ptr;
	}
}

int Active (tList *L) {

	return L->Act == NULL ? 0 : 1;
}

/* Konec c201.c */
