#ifndef VAR_BLOCK_HPP
#define VAR_BLOCK_HPP


#include <map>
#include <set>
#include <deque>
#include <string>

#include "code_block.hpp"
#include "typedefs.hpp"

#define CB_VARBLOCK 16

class var_block : public code_block
{
    protected:
        ident name;
    public:
        var_block(const ident&);
        ~var_block();

        virtual std::deque<std::string> render(size_t start);
        virtual std::multiset<ident> used_idents();
        virtual std::multiset<ident> used_labels();
        virtual void optimize(const std::multiset<ident> & = std::multiset<ident>(), const std::multiset<ident> & = std::multiset<ident>());
		virtual void rename_ident(ident to, ident from);
};

#define _PUSH_VAR(id)       ident id = SX(id); {_->push_block(ptr(code_block)(new var_block(id)));}
#define _POP_VAR(id)        {}

#endif

