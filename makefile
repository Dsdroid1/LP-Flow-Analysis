all: Calculator

Calculator: y.tab.c lex.yy.c
	gcc lex.yy.c y.tab.c -o Calculator -lm	

y.tab.c: yaccGrammer.y
	yacc -d yaccGrammer.y

lex.yy.c: y.tab.c lexTokens.l
	flex lexTokens.l

clean:
	rm -rf lex.yy.c y.tab.c y.tab.h Calculator