%{
#include <stdio.h>//Headers for the generated C code
#include <stdlib.h>
#include "y.tab.h"
extern YYSTYPE yylval;
void yyerror (char *s);
int yylex();
/*[0-9]+(.[0-9]+)?        yylval=strtof(yytext,NULL); return FLOAT;*/
%}

%%

[0-9]+  {yylval=atoi(yytext);return CONST;}
"+"     {return OP;}
"-"     {return OP;}
"*"     {return OP;}
"/"     {return OP;}
"if"    {return IF;}
\n      { return EOL;}
"goto"  {return GOTO;}
[>=<]   {return COMP;}
":="    {return ASSIGNMENT;}
[a-zA-Z][a-zA-Z0-9]*(\[[0-9]+\])? {return VAR;}
.* ;
%%
int yywrap (void) {return 1;}