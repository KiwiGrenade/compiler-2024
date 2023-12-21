#ifndef ETIQUETTE_HPP
#define ETIQUETTE_HPP


#include <map>
#include <set>
#include <deque>
#include <string>

#include "code_block.hpp"
#include "typedefs.hpp"

#define CB_LABEL 2

extern uint64 etiqId;

class label : public code_block
{
    protected:
        ident name;
    public:
        label(const ident&);
        ~label();

        virtual std::deque<std::string> render(size_t start);
        virtual std::multiset<ident> used_idents();
        virtual std::multiset<ident> used_labels();
        virtual void optimize(const std::multiset<ident> & = std::multiset<ident>(), const std::multiset<ident> & = std::multiset<ident>());
		virtual void rename_ident(ident to, ident from);
        
        std::string get_name() { return name; }
};

#define _DECL_LBL(id)       ident id = SX(id) + ("<" + std::to_string(etiqId++) + ">");
#define _PUSH_LBL(id)       {_->push_block(ptr(code_block)(new label(id)));}
#define _PUSH_LBL_FRONT(id) {_->push_block_front(ptr(code_block)(new label(id)));}
#define _POP_LBL(id)        {}

#endif

