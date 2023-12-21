#include "instruction.hpp"
#include "definitions.hpp"

instruction::instruction(const std::string &_asm) : assembler(_asm), argument(""), code_block(CB_INSTRUCTION) { }
instruction::instruction(const std::string &_asm, const ident &var, const char type)
    : assembler(_asm), argument(var), argtype(type), code_block(CB_INSTRUCTION) { }
instruction::~instruction()  { }

std::deque<std::string>
    instruction::render(size_t start)
{
    if(argument.size() > 0)
        if(argtype == IA_REG && !no_sub)
            return std::deque<std::string>(1, assembler + " " + std::to_string(registers[argument]));
        else
            return std::deque<std::string>(1, assembler + " " + argument);
    else
        return std::deque<std::string>(1, assembler);
}

std::string
    instruction::peek()
{
    if(argument.size() > 0)
        return assembler + " " + argument;
    else
        return assembler;
}

std::multiset<ident>
    instruction::used_idents()
{
    std::multiset<ident> result;
    if(argument != "" && argtype == IA_REG)
        result.insert(argument);
    return result;
}

std::multiset<ident>
    instruction::used_labels()
{
    std::multiset<ident> result;
    if(argument != "" && argtype == IA_LBL)
        result.insert(argument);
    return result;
}

void
	instruction::optimize(const std::multiset<ident> &_, const std::multiset<ident> &__)
{
	// can't optimize just one instruction
}

void
	instruction::rename_ident(ident to, ident from)
{
	if(argument == from)
    {
        // if(argtype == IA_LBL) error("Renaming etiquette " + from + " to " + to, true); // used by opt
        argument = to;
    }
}
