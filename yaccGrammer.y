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
%token EOL
%start line

%%

line:   
        EOL
        {
            printf("Program end");
        }
        | 
        assignment EOL
        {
            printf("Assignment detected");
        }
        |
        line unconditional_goto 
        {

        }
        |
        line conditional_goto
        {

        }


assignment:     VAR ASSIGNMENT simple EOL
                {
                    printf("Assignment");
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
                printf("COnst only");
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
    yyin=fopen("file2.txt","r");
    return yyparse();
}


void yyerror (char *s) {fprintf (stderr, "%s\n", s);}