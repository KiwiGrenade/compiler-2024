#include <iostream>
#include "definitions.hpp"


bool is_verbose = false;
bool is_success = true;
bool is_parsing = true;

int yywrap() {
    return 1;
}

void error(std::string msg, bool is_fatal) {

    std::cerr << "[ERR]" << msg << '\n';
    if(is_parsing) {
        std::cerr << "Parse line: " << currLine << '\n';
    }
    if(is_fatal) {
        exit(-1);
    }
    is_success = false;
}

void warning(std::string msg) {
    std::cerr << "[WRN]" << msg << '\n';
    if(is_parsing) {
        std::cerr << "Parse line: " << currLine << '\n';
    }
}

void yyerror(const char* yerror_msg) {
    error(yerror_msg, true);
}


int main(int argc, char* argv[])
{
    FILE *pFILE_IN  = fopen(argv[1], "r");

    std::string output_file = argv[2];

    if(!pFILE_IN)
    {
        std::cerr << "ERROR: file " << argv[1] << " not found!" << std::endl;
        return 1;
    }

    yyin = pFILE_IN;

    yyparse();

    AST::translate_main();
    AST::save_code(output_file);

    return 0;
}
