%{
// Global and header definitions required 
void yyerror (char *s);
int yylex();
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>

%}

%token VAR CONST
%token OP COMP ASSIGNMENT
%token IF
%token END
%token GOTO
%start line

%%

line:   assignment "\n" line
        {
            printf("Assignment detected");
        }
        |
        unconditional_goto "\n" line
        {

        }
        |
        conditional_goto "\n" line
        {

        }
        |
        "\n"
        {

        }
        |
        END
        {

        }
        ;


assignment:     VAR ASSIGNMENT simple
                {

                }   
                ;     

unconditional_goto:     GOTO CONST
                        {
                            
                        }
                        ;

conditional_goto:   IF simple COMP simple GOTO CONST
                    {

                    }
                    ;

simple:     vars_only
            {

            }
            |
            const_only
            {

            }
            |
            mix
            {

            }
            ;

vars_only:  VAR OP VAR
            {   
                //Add expression to table
                //Assign order to both vars in case of addition or multiplication
                //a+b==b+a stored as a+b
            }
            ;

const_only: CONST OP CONST
            {
                //Evaluate expression here itself
            }
            ;

mix:    VAR OP CONST
        {
            //Save as VAR OP CONST
        }
        |
        CONST OP VAR
        {
            //Save as VAR OP CONST
        }
        ;
%%

int main(void)
{
    extern FILE *yyin;
    yyin=fopen("file.txt","r");
    return yyparse();
}


void yyerror (char *s) {fprintf (stderr, "%s\n", s);}