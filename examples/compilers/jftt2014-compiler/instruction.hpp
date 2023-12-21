#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <map>
#include <set>
#include <deque>
#include <string>

#include "code_block.hpp"
#include "typedefs.hpp"

#define IA_REG 0
#define IA_LBL 1

#define CB_INSTRUCTION 4

class instruction : public code_block
{
	protected:
		std::string assembler;
        ident argument;
        char argtype;

	public:
		instruction(const std::string &_asm);
		instruction(const std::string &_asm, const ident &arg, const char type=IA_REG);
		~instruction();

        virtual std::deque<std::string> render(size_t start);
        std::string peek();

        virtual std::multiset<ident> used_idents();
        virtual std::multiset<ident> used_labels();

		virtual void optimize(
            const std::multiset<ident> & = std::multiset<ident>(), 
            const std::multiset<ident> & = std::multiset<ident>());

		virtual void rename_ident(ident to, ident from);
};

#define _PUSH_ASM(asm)       {_->push_block(ptr(code_block)(new instruction(asm)));}
#define _PUSH_ASMa(asm, arg) {_->push_block(ptr(code_block)(new instruction(asm, arg, IA_REG)));}
#define _PUSH_ASMe(asm, arg) {_->push_block(ptr(code_block)(new instruction(asm, arg, IA_LBL)));}

#define _PUSH_ASM_FRONT(asm)       {_->push_block_front(ptr(code_block)(new instruction(asm)));}
#define _PUSH_ASM_FRONTa(asm, arg) {_->push_block_front(ptr(code_block)(new instruction(asm, arg, IA_REG)));}
#define _PUSH_ASM_FRONTe(asm, arg) {_->push_block_front(ptr(code_block)(new instruction(asm, arg, IA_LBL)));}

#endif

