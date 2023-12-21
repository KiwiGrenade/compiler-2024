#include "definitions.hpp"
#include "types.hpp"


uint64 nextLU = 0;

struct exprcond
{
    ident i1;
    int op;
    ident i2;
};

struct lutup
{
    ptr(snippet) b;
    exprcond ec;
};

std::map<ident, lutup> lookup;

ptr(snippet) program;

void startHandling()
{
    program = ptr(snippet)(new snippet());
    parsing = true;
}

ptr(snippet) getCode()
{
    return program;
}

ident genCommandIdent()
{
    nextLU += 0xff;
    return "ci" + std::to_string(nextLU);
}

ident handleCDecl(ident name, std::string val)
{
    ptr(snippet) _ = program;

    logme("Handle CONST " << name << " = " << val << " decl");

    if(constants.count(name) || variables.count(name))
        error("Identifier " + name + " already in use", false);
    constants[name] = std::stoll(val);
    logme("in map");
    // TODO: Push block with const assignment after const opt
    //_PUSH_BLOCK(snippet::build_assign_num(name, constants[name]));

    return "";
}

ident handleVDecl(ident name)
{
    logme("Handle VAR " << name << " decl");

    if(constants.count(name) || variables.count(name))
        error("Identifier " + name + " already in use", false);
    variables[name] = false;
    return "";
}

ident handleLastDecl()
{
    if(variables.size() < 3) use_temp_dest = false; // in such case, temp_dest may be just a pain in the ass
    return "";
}

#define _START \
ptr(snippet) _(new snippet());\
ident ___ = genCommandIdent();

#define ___RET \
lookup[___].b = _;\
return ___;

ident handleAssign(ident name, ident expr)
{
    _START;

    logme("Handle ASSIGN TO " << name);
    exprcond ec = lookup[expr].ec;
    lookup.erase(expr);

    ident i1 = ec.i1, i2 = ec.i2;

    switch(ec.op)
    {
        case NUM:
            if(!variables.count(name))
                error("Identifier " + name + " is not a variable", false);
            _PUSH_BLOCK(snippet::build_assign_num(name, std::stoll(i1)));
            break;
        case EQUAL:
            if(!variables.count(name))
                error("Identifier " + name + " is not a variable", false);
            if(i1 == name)
            {
                warning("Assignment " + name + " := " + i1);
                break;
            }
            _PUSH_BLOCK(snippet::build_assign(name, i1));
            break;
        case PLUS:
        case MINUS:
        case ASTERISK:
        case SLASH:
        case PERCENT:
            if(!variables.count(name))
                error("Identifier " + name + " is not a variable", false);
            switch(ec.op)
            {
                case PLUS:
                    _PUSH_BLOCK(snippet::build_add(name, i1, i2));
                    break;
                case MINUS:
                    _PUSH_BLOCK(snippet::build_sub(name, i1, i2));
                    break;
                case ASTERISK:
                    _PUSH_BLOCK(snippet::build_mul(name, i1, i2));
                    break;
                case SLASH:
                    _PUSH_BLOCK(snippet::build_div(name, i1, i2));
                    break;
                case PERCENT:
                    _PUSH_BLOCK(snippet::build_mod(name, i1, i2));
                    break;
            }
            break;
    }
    variables[name] = true;

    ___RET;
}

ident handleEmptyCommand()
{
    _START;
    ___RET;
}

ident handleCommand(ident block, ident name)
{
    logme("Handle COMMAND " << name);

    ptr(snippet) _ = lookup[block].b;
    _PUSH_BLOCK(lookup[name].b);
    lookup.erase(name);
    return block;
}

ident handleProgram(ident block)
{
    ptr(snippet) _ = program;
    _PUSH_BLOCK(lookup[block].b);
    lookup.erase(block);
    return "";
}

ident handleIf(ident cond, ident blockIf, ident blockElse)
{
    logme("Handle IF ");
    _START;
    ident i1 = lookup[cond].ec.i1, i2 = lookup[cond].ec.i2;
    _PUSH_BLOCK(snippet::build_if(
        i1, i2, lookup[cond].ec.op,
        lookup[blockIf].b, lookup[blockElse].b));
    lookup.erase(cond);
    lookup.erase(blockIf);
    lookup.erase(blockElse);
    ___RET;
}
ident handleWhile(ident cond, ident block)
{
    logme("Handle WHILE ");
    _START;
    ident i1 = lookup[cond].ec.i1, i2 = lookup[cond].ec.i2;
    _PUSH_BLOCK(snippet::build_while(
        lookup[cond].ec.i1, lookup[cond].ec.i2, lookup[cond].ec.op,
        lookup[block].b));
    lookup.erase(cond);
    lookup.erase(block);
    ___RET;
}
ident handleRead(ident var)
{
    _START;
    if(!variables.count(var))
        error("Identifier " + var + " is not a variable", false);
    _PUSH_ASMa("SCAN", var);
    variables[var] = true;
    ___RET;
}
ident handleWrite(ident var)
{
    _START;
    if(variables.count(var) && !variables[var])
        warning("Identifier " + var + " may be not initialized");
    _PUSH_ASMa("PRINT", var);
    ___RET;
}

ident handleExpression(ident expr_arg1, int op, ident expr_arg2)
{
    logme("Handle EXPR/COND " << expr_arg1 << " " << op << " " << expr_arg2);

    ident r = genCommandIdent();
    lookup[r].ec.i1 = expr_arg1;
    lookup[r].ec.i2 = expr_arg2;
    lookup[r].ec.op = op;
    return r;
}

void checkIdentAtTime(ident id)
{
    if(!variables.count(id) && !constants.count(id))
        error("Identifier " + id + " is not a variable nor a constant", false);
    if(variables.count(id) && !variables[id])
        warning("Identifier " + id + " may be not initialized");
}