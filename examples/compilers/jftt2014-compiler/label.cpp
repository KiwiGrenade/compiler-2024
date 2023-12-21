#include "label.hpp"
#include "definitions.hpp"

uint64 etiqId = 0;

label::label(const ident &id) : name(id), code_block(CB_LABEL) { }
label::~label()  { }

std::deque<std::string>
    label::render(size_t start)
{
    labels[name] = start;
    logme("Label " << name << " at " << start);
    if(no_sub)
        return std::deque<std::string>(1, name + ':');
    return std::deque<std::string>();
}

std::multiset<ident>
    label::used_idents()
{
    return std::multiset<ident>();
}

std::multiset<ident>
    label::used_labels() // only USED should be returned, not declared
{
    return std::multiset<ident>();
}

void
    label::optimize(const std::multiset<ident> &_, const std::multiset<ident> &__)
{
    // You're kidding me, aren't you?
}

void 
    label::rename_ident(ident to, ident from)
{ }