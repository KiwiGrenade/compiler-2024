#include "var_block.hpp"

var_block::var_block(const ident &id) : name(id), code_block(CB_VARBLOCK) { }
var_block::~var_block()  { }

std::deque<std::string>
    var_block::render(size_t start)
{
    return std::deque<std::string>();
}

std::multiset<ident>
    var_block::used_idents()
{
    std::multiset<ident> result;
    result.insert(name);
    return result;
}

std::multiset<ident>
    var_block::used_labels()
{
    return std::multiset<ident>();
}

void
    var_block::optimize(const std::multiset<ident> &_, const std::multiset<ident> &__)
{
    //
}

void
	var_block::rename_ident(ident to, ident from)
{
    if(name == from) name = to;
}
