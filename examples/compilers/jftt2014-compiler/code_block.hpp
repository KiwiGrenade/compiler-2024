#ifndef CODE_BLOCK_HPP
#define CODE_BLOCK_HPP


#include <map>
#include <set>
#include <deque>
#include <string>

// Ident
#include "typedefs.hpp"

#define CB_GENERIC 1

class code_block
{
	protected:
        code_block(char type) : type(type) {}
	public:
        bool can_be_flattened = false;
        bool is_loop = false;
        char type = CB_GENERIC;
		virtual std::deque<std::string> render(size_t start) = 0;
        virtual std::multiset<ident> used_idents() = 0;
        virtual std::multiset<ident> used_labels() = 0;
		virtual void optimize(const std::multiset<ident> & = std::multiset<ident>(), const std::multiset<ident> & = std::multiset<ident>()) = 0;
		virtual void rename_ident(ident to, ident from) = 0;
};

#endif

