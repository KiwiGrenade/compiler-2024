%top{
    #include "definitions.hpp"
    #include "types.hpp"
}
comment         "#".*|\\n
whitespace      [[:blank:]]+ 
num             [[:digit:]]+
pidentifier     [_a-z]+
newline         \n|\r|\n\r

%%

";"     { return SEMICOLON; }
","     { yylval = std::string(yytext); return COMMA; }
"("     { yylval = std::string(yytext); return LPRNT; }
")"     { yylval = std::string(yytext); return RPRNT; }
"["     { yylval = std::string(yytext); return LBRCKT; }
"]"     { yylval = std::string(yytext); return RBRCKT; }

":="    { yylval = std::string(yytext); return ASSIGN; }

"+"     { yylval = std::string(yytext); return PLUS; }
"-"     { yylval = std::string(yytext); return MINUS; }
"*"     { yylval = std::string(yytext); return ASTERISK; }
"/"     { yylval = std::string(yytext); return FWSLASH; }
"%"     { yylval = std::string(yytext); return PERCENT; }

"="     { yylval = std::string(yytext); return EQUAL; }
"!="    { yylval = std::string(yytext); return NEQUAL; }
">"     { yylval = std::string(yytext); return MORE; }
"<"     { yylval = std::string(yytext); return LESS; }
">="    { yylval = std::string(yytext); return MOREOREQUAL; }
"<="    { yylval = std::string(yytext); return LESSOREQUAL; }

        
{comment}      { /*ignore comments*/ }
{whitespace}   { /*ignore whitespaces*/ }

{num}          { yylval = std::string(yytext); return num; }
{pidentifier}  { yylval = std::string(yytext); return pidentifier; }
{newline}      { currLine++; }

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
            } else if (KW == "WHILE") {
                return KW_WHILE;
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

.              {/* TODO: Add INVALID_CHARACTER handling */}
%%
size_t currLine = 1;
