#include <iostream>
#include "definitions.hpp"


bool verbose = false;
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
    FILE *pFILE_IN;

    std::string output_file;


    for(int i = 1; i< argc; ++i)
    {
        if(argv[i][0] == '-')
        {
            if(argv[i][1] == 'v')
            {
                verbose = true;
                logme("Verbose ON", "[M]");
            }
            // else if(argv[i][1] == 'O') // any other turns on (again?)
            // {
            //     optimization = true;
            // }
            // else if(argv[i][1] == 'h')
            // {
            //     print_help();
            // }
            else error("Unknown compiler parameter " + std::string(argv[i]), true);
        }
        else {
            pFILE_IN  = fopen(argv[i], "r");
            if(!pFILE_IN)
            {
                error("File " + std::string(argv[i]) + " not found!", true);
                return 1;
            }
            output_file = argv[i+1];
            break;
        }
    }

    yyin = pFILE_IN;

    yyparse();


    AST::link_vertices();
    AST::spread_proc_name();
    AST::translate_main();
    AST::resolve_jumps();
    AST::save_code(output_file);

    return 0;
}
