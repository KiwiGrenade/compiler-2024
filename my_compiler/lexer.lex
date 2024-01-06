%top{
    #include <string>
    #include "grammar.hpp"
    size_t currLine = 0;
}

comment         "#".*|\\n
whitespace      [[:blank:]]+ 
num             [[:digit:]]+
pidentifier     [_a-z]+
newline         \n|\r|\n\r

%%

";"     { return SEMICOLON; }
","     { return COMMA; }
"("     { return LPRNT; }
")"     { return RPRNT; }
"["     { return LBRCKT; }
"]"     { return RBRCKT; }

":="    { return ASSIGN; }

"+"     { return PLUS; }
"-"     { return MINUS; }
"*"     { return ASTERISK; }
"\""    { return FWSLASH; }
"%"     { return PERCENT; }

"="     { return EQUAL; }
"!="    { return NEQUAL; }
">"     { return MORE; }
"<"     { return LESS; }
">="    { return MOREOREQUAL; }
"<="    { return LESSOREQUAL; }

        
{comment}      { /*ignore comments*/ }
{whitespace}   { /*ignore whitespaces*/ }

{num}          { yylval = std::string(yytext); return num; }
{pidentifier}  { yylval = std::string(yytext); return pidentifier; }
{newline}      { currLine++; }
.              {/* TODO: Add INVALID_CHARACTER handling */}

[A-Z]+  {
            std::string KW = std::string(yytext);
            if (KW == "PROCEDURE") {
                return KW_PROCEDURE;
            } else if (KW == "IS") {
                return KW_IS;
            } else if (KW == "IN") {
                return KW_IN;
            } else if (KW == "END") {
                return KW_END;
            } else if (KW == "PROGRAM") {
                return KW_PROGRAM;
            } else if (KW == "IF") {
                return KW_IF;
            } else if (KW == "THEN") {
                return KW_THEN;
            } else if (KW == "ELSE") {
                return KW_ELSE;
            } else if (KW == "ENDIF") {
                return KW_ENDIF;
            } else if (KW == "DO") {
                return KW_DO;
            } else if (KW == "ENDWHILE") {
                return KW_ENDWHILE;
            } else if (KW == "REPEAT") {
                return KW_REPEAT;
            } else if (KW == "UNTIL") {
                return KW_UNTIL;
            } else if (KW == "READ") {
                return KW_READ;
            } else if (KW == "WRITE") {
                return KW_WRITE;
            } else if (KW == "T") {
                return KW_T;
            } else {
                /* TODO: Add INVALID_KEYWORD handling */
            }
        }
%%