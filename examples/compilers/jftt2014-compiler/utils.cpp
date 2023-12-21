#include "definitions.hpp"
#include "instruction.hpp"
#include "snippet.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

bool verbose = 0;
bool silent = 0;

bool successful = true;
bool optimization= true;
bool no_sub = false;
bool no_jzero = true;
bool use_temp_dest = true;
bool parsing = false;

std::map<ident, int> labels;
std::map<ident, int> registers;
std::map<ident, uint64> constants;
std::map<ident, bool> variables;

int yywrap()
{
    return 1;
}

void yyerror(const char* emsg)
{
    error(emsg, true);
}

void error(std::string msg, bool fatal)
{
    std::cerr << " [ERR] " << msg << std::endl;
    if(parsing)
        std::cerr << " [Parser line] " << currentLine << std::endl;
    if(fatal) exit(-1);
    successful = false;
}

void warning(std::string msg)
{
    std::cerr << " [WARN] " << msg << std::endl;
    std::cerr << " [Parser line] " << currentLine << std::endl;
}

void naive_recalc_registers(ptr(snippet) source)
{
    auto vars = source->used_idents(); // recalc vars after pushing consts
    std::deque<ident> sorted_by_usage = multi_to_sorted(vars);
        
    int i = 0;
    for(auto it : sorted_by_usage)
    {
        // assign the most used var the lowest available register and so on
        logme("Assigning register " << i << " to var/const " << it);
        registers[it] = i++;
    }
}

std::string sub_labels(std::string str)
{
    for(auto &it : labels)
    {
        ident lbl = it.first;
        ident lnum = std::to_string(it.second);
        logme(lbl);
        
        size_t pos = 0;
        while((pos = str.find(lbl, pos)) != std::string::npos)
        {
            logme("Replacing " << lbl << " with " << lnum);
            str.replace(pos, lbl.length(), lnum);
        }
    }
    return str;
}

std::string render(const std::deque<std::string> &source)
{
    std::ostringstream os;
    for(auto &it : source)
    {
        os << it << std::endl;
    }
    std::string str = os.str();
    return str;
}


void synthesis()
{
    logme("Building test snippet");
    ptr(snippet) _(new snippet());
    _PUSH_BLOCK(snippet::build_assign_num("a", 1000));
    _PUSH_BLOCK(snippet::build_assign_num("b", 2));
    _PUSH_BLOCK(snippet::build_mul("c", "b", "a"));
    _PUSH_ASMa("PRINT", "a");
    
    ptr(snippet) program = _;
    logme("Test snippet built");
    naive_recalc_registers(program);
    logme("Registers precalculated");
    auto code = render(program->render(0));
    logme("Code rendered");
    std::string fin = sub_labels(code);
    logme("Labels substitued");
    std::cout << fin;
    std::cout << "HALT\n";
}

    
    // Prime factors 100:
    // no opt: 25001
    // opt:    24331
    // no_jzero:
    // opt:    23488
    // use_temp_dest:
    // opt:    20226

void print_help()
{
    std::cout << "\
  $ ./compiler [-o (output)] [-v] [-s] [-OX] [-h] [-a] [(input)]\n\
  \n\
  Gdzie:\n\
    -o determinuje plik wyjściowy. Domyślnie używane jest standardowe wyjście\n\
    -v to tryb VERBOSE - drukuje cały dziennik kompilacji\n\
    -s to tryb SILENT  - nie drukuje nic, poza błędami\n\
    -OX to wybór optymalizacji. Aktualnie -O0 wyłącza całą optymalizację, każda inna wartość pozostawia ją włączoną\n\
    -h drukuje tę pomoc\n\
    -a to tryb ASSEMBLE ONLY - nie produkuje kodu na maszynę\n\
               zwraca kod bez zmiany etykiet i rejestrów\n\
    (input) to plik wejściowy. Domyślnie czytane jest standardowe wejście\n\n\n";
    exit(1);
}

int main(int argc, const char* argv[])
{
    yyin = stdin;
    
    std::ofstream o;;
    
    // Filenames (just for the output file)
    std::string fnin = "-";
    
    // 1. Parse arguments
    for(int i = 1; i< argc; ++i)
    {
        if(argv[i][0] == '-')
        {
            if(argv[i][1] == 'o')
            {
                if(o.is_open())
                    error("Output file already specified", true);
                    
                o.open(argv[++i]);
            }
            else if(argv[i][1] == 'v')
            {
                verbose = true;
                logme("Verbose ON");
            }
            else if(argv[i][1] == 's')
            {
                silent = true;
            }
            else if(argv[i][1] == 'O' && argv[i][2] == '0')
            {
                optimization = false;
            }
            else if(argv[i][1] == 'O') // any other turns on (again?)
            {
                optimization = true;
            }
            else if(argv[i][1] == 'h')
            {
                print_help();
            }
            else if(argv[i][1] == 'a')
            {
                no_sub = true;
            }
            else error("Unknown parameter " + std::string(argv[i]), true);
        }
        else
        {
            if(yyin != stdin)
                error("Input file already specified", true);
            
            yyin = fopen(argv[i], "r");
            fnin = argv[i];
            if(yyin == NULL) error("Coulnd't open source file " + fnin, true);
        }
    }
    
    // 2. Set output stream
    std::ostream *os;
    if(o.is_open())
        os = &o;
    else
        os = &std::cout;
    
    // 3. Parse source file
    startHandling();
    yyparse();
    parsing = false;
    
    // 5. Check correctness:
    if(!successful) error("Compilation terminated.", true);
    
    // 6. Receive program code from parser
    ptr(snippet) program = getCode();
    logme("Program snippet built");
    
    // 7. Add necessary constants
    program->add_remaining_constants();
    logme("Constants built in");
    
    // 8. If optimization not turned off:
    if(optimization) 
    {
        program->optimize();
        logme("Program optimized");
    }
    
    // 9. Recalculate registers usage
    naive_recalc_registers(program);
    logme("Registers precalculated");
    
    // 10. Render code
    auto code = render(program->render(0));
    logme("Code rendered");
    
    // 11. Substitue labels
    std::string fin;
    if(!no_sub)
    {
        fin = sub_labels(code);
        logme("Labels substitued");
    }
    else fin = code;
    (*os) << fin;
    (*os) << "HALT\n";
    
    if(o.is_open()) o.close();
    info("Compilation successful");
}


