#ifndef SNIPPET_HPP
#define SNIPPET_HPP

#include <map>
#include <set>
#include <deque>
#include <string>

#include "code_block.hpp"
#include "typedefs.hpp"

#define CB_SNIPPET 8

class snippet : public code_block
{
	protected:
		std::deque<ptr(code_block)> blocks;

	public:
		snippet() ;
		snippet(ptr(code_block));
		~snippet();

		virtual std::deque<std::string> render(size_t start);

        virtual std::multiset<ident> used_idents();
        virtual std::multiset<ident> used_labels();

		virtual void optimize(const std::multiset<ident> & = std::multiset<ident>(), const std::multiset<ident> & = std::multiset<ident>());
		virtual void rename_ident(ident to, ident from);

		static ptr(snippet) build_assign_num(ident dest, uint64 num);
		static ptr(snippet) build_assign(ident dest, ident arg);

        static ptr(snippet) build_add(ident dest, ident arg1, ident arg2);
        static ptr(snippet) build_sub(ident dest, ident arg1, ident arg2);
        static ptr(snippet) build_mul(ident dest, ident arg1, ident arg2);
        static ptr(snippet) build_div(ident dest, ident arg1, ident arg2);
        static ptr(snippet) build_mod(ident dest, ident arg1, ident arg2);

		static ptr(code_block) build_if(ident cond_arg1, ident cond_arg2, int cond_type, ptr(code_block), ptr(code_block));
		static ptr(code_block) build_while(ident cond_arg1, ident cond_arg2, int cond_type, ptr(code_block));

		void push_block       (ptr(code_block));
		void push_block_front (ptr(code_block));

		void flatten();
        void add_remaining_constants(); // use only on top-level block for opt. reasons
        void remove_unused_labels(std::multiset<ident> used = std::multiset<ident>());
};

ptr(snippet) getCode();

#define _PUSH_BLOCK(blck)       {_->push_block(blck);}

#endif

