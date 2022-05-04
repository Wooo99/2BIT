CFLAGS=-g -Wall -Wextra 
FILES=main.c dynamic_string.c pars_string.c newscanner.c

main: main_syntax.c symtable.c syntax_common.c table_hierarchy.c syntax.c expression.c dynamic_string.c pars_string.c newscanner.c 
	gcc $^ -o ifj20 $(CFLAGS)
	
expr: main_expression.c symtable.c syntax_common.c table_hierarchy.c syntax.c expression.c dynamic_string.c pars_string.c newscanner.c 
	gcc $^ -o main_expr.out $(CFLAGS)

clean:
	rm -f *.o *.h.gch *.out main ifj20
