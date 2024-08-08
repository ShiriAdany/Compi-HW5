%{
    /* Declarations section */
    #include <stdio.h>
    #include <stddef.h>
    #include <stdint.h>
    #include <iostream>
    #include <string.h>
    #include "types.hpp"
    #include "hw3_output.hpp"
    #include "parser.tab.hpp"

    void showToken(const char *);
    void append_char(char *dest, char c);

%}

%option yylineno
%option noyywrap

%%

int                 { yylval = new Node(yytext);  return INT; }
byte                { yylval = new Node(yytext);  return BYTE; }
b                   { yylval = new Node(yytext);  return B; }
bool                { yylval = new Node(yytext);  return BOOL; }
and                 { yylval = new Node(yytext);  return AND; }
or                  { yylval = new Node(yytext);  return OR; }
not                 { yylval = new Node(yytext);  return NOT; }
true                { yylval = new Node(yytext);  return TRUE; }
false               { yylval = new Node(yytext);  return FALSE; }
return              { yylval = new Node(yytext);  return RETURN; }
if                  { yylval = new Node(yytext);  return IF; }
else                { yylval = new Node(yytext);  return ELSE; }
while               { yylval = new Node(yytext);  return WHILE; }
break               { yylval = new Node(yytext);  return BREAK; }
continue            { yylval = new Node(yytext);  return CONTINUE; }
[\n\r\t ]           { /* ignore whitespace */ }
;                   {  return SC; }

\(                  {  return LPAREN; }
\)                  {  return RPAREN; }
\{                  {  return LBRACE; }
\}                  {  return RBRACE; }
=                   {  return ASSIGN; }
==|!=               {  yylval = new Operator(yytext); return RELEQ; }
\<=|\>=             {  yylval = new Operator(yytext); return RELOP; }
\<|\>               {  yylval = new Operator(yytext); return RELOP; }
[*|/]               {  yylval = new Operator(yytext); return MULDIV; }
[+|\-]              {  yylval = new Operator(yytext); return ADDSUB; }

\/\/[^\r\n]*[\r\n]?        { showToken("COMMENT"); }
[A-Za-z][A-Za-z0-9]*      {yylval = new Id(yytext);  return ID; }
[1-9][0-9]*|0              { yylval = new Num(yytext); return NUM; }
\"([^\n\r\"\\]|\\[rnt"\\])+\"   { yylval = new String(yytext);  return STRING; }

\"                  { showToken("UNCLOSED"); }
.                   { showToken("UNKNOWN"); }

%%

void append_char(char *dest, char c) {
    char temp[2]; // Temporary string to hold the character and null terminator
    temp[0] = c;  // First element is the character
    temp[1] = '\0'; // Second element is the null terminator
    strcat(dest, temp); // Concatenate temp to dest
}

void showToken(const char * name) {
    if(name=="UNKNOWN"){
        output::errorLex(yylineno);
        exit(0);
    }
    else if(name == "UNCLOSED"){
        output::errorLex(yylineno);
        exit(0);
    }
    else if(name=="STRING_ERROR")
    {
        output::errorLex(yylineno);
        exit(0);
    }
    else if(name=="STRING"){
        char to_print[1024];
        int index =0;
        bool prev_is_backslash = false;
        int length = strlen(yytext);

        for (int i=1; i < length - 1; i++) {
            if (prev_is_backslash) {
                prev_is_backslash = false;
                if (yytext[i] == 92 || yytext[i] == 34) {
                    to_print[index] = yytext[i];
                    index++;
                } else if (yytext[i] == 'n') {
                    to_print[index] = 10;
                    index++;

                } else if (yytext[i] == 'r') {
                    to_print[index] = 13;
                    index++;

                } else if (yytext[i] == 't') {
                    to_print[index] = 9;
                    index++;

                } else if (yytext[i] == '0') {
                    to_print[index] = 0;
                    index++;

                } else if (yytext[i] == 'x') {
                    // handle digits for the \x case
                    if(i+2>=length - 1){
                        printf("some1 %s", yytext);
                        output::errorLex(yylineno);
                        exit(0);
                    }
                    else{
                        char first_dig = yytext[i+1];
                        char sec_dig = yytext[i+2];
                        if(first_dig >= '0' &&first_dig <= '7' && ((sec_dig >= '0' && sec_dig <= '9') || (sec_dig >= 'a' && sec_dig <= 'f') || (sec_dig >= 'A' && sec_dig <= 'F')))
                        {
                            i+=2;
                            int first = (first_dig -'0')*16;
                            int second;
                            if(sec_dig >= '0' && sec_dig <= '9') second = sec_dig -'0';
                            else if (sec_dig >= 'a' && sec_dig <= 'f') second = sec_dig - 'a' +10;
                            else if (sec_dig >= 'A' && sec_dig <= 'F') second = sec_dig - 'A' +10;
                            to_print[index] = first+second;
                            index++;
                        }
                        else{
                            printf("some2 %s", yytext);
                            output::errorLex(yylineno);
                            exit(0);
                        }

                    }
                }

                 else {
                    printf("some3 %s", yytext);
                    output::errorLex(yylineno);
                    exit(0);

                }
            }
            else{
                if (yytext[i] == 92) {
                    prev_is_backslash = true;
                } else {
                    prev_is_backslash = false;
                    to_print[index] = yytext[i];
                    index++;
                }
            }
        }
        if(prev_is_backslash)
        {
            printf("some4 %s", yytext);
            output::errorLex(yylineno);
            exit(0);
        }

    }
    else{
        return;
    }
}
