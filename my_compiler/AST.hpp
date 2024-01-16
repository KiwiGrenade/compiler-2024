#ifndef AST_HPP
#define AST_HPP

#include <map>
#include <memory>
#include <vector>
#include <fstream>
#include "CodeBlock.hpp"
#include "definitions.hpp"
#define logme_archt(str) { logme(str, "MEM")}

enum Register {A, B, C, D, E, F, G, H, NONE};

inline std::string to_string(Register reg){
    switch (reg) {
    case Register::A:
        return "a";
    case Register::B:
        return "b";
    case Register::C:
        return "c";
    case Register::D:
        return "d";
    case Register::E:
        return "e";
    case Register::F:
        return "f";
    case Register::G:
        return "g";
    case Register::H:
        return "h";
    case Register::NONE:
        return "";
    }
}
struct AsmInstruction {
    int instr_ptr;
    int jump_address = -1;
    std::string code;
    Register _register;
    std::shared_ptr<CodeBlock> cb;

    AsmInstruction(std::string _code, int ip) : 
        code(_code),
        _register(Register::NONE), 
        instr_ptr(ip){};
    
    AsmInstruction(std::string _code, ptr(CodeBlock) _cb, int ip) :
        code(_code),
        cb(_cb),
        instr_ptr(ip){};
    
    AsmInstruction(std::string _code, Register reg) : 
        code(_code),
        cb(nullptr),
        _register(reg){};
    
    AsmInstruction(std::string _code, int _jump_address, int ip) :
        code(_code),
        _register(Register::NONE), 
        jump_address(_jump_address),
        instr_ptr(ip){};
};

struct AST {
    // k in virtual machine
    static int                                  instruction_pointer;
    static std::vector<int>                     head_ids;
    static std::map<int, ident>                 head_map;
    static std::vector<ptr(CodeBlock)>          vertices;
    // static Architecture                         architecture;
    static std::vector<ptr(AsmInstruction)>     _asm_instructions;

    static void add_vertex(size_t _id, Instruction _ins);
    static void add_empty_vertex(size_t id);
    static void add_edge(int v_id, int u_id);
    static void add_edge(int v_id, int u_id, bool flag);
    static ptr(CodeBlock) get_vertex(int id);
    virtual ~AST() = default;
    
    // ASSEMBLER INSTRUCTIONS
    static void add_instruction(ptr(AsmInstruction) instr);
    
    static void _asm_load(Value val1, Register reg);

    // EXPRESSIONS
    static void _asm_add(Value val1, Value val2, ptr(CodeBlock) cb);
    static void _asm_sub(Value val1, Value val2, ptr(CodeBlock) cb);
    static void _asm_mul(Value val1, Value val2, ptr(CodeBlock) cb);
    static void _asm_div(Value val1, Value val2, ptr(CodeBlock) cb);
    static void _asm_mod(Value val1, Value val2, ptr(CodeBlock) cb);
    // CONDITIONS
    static void _asm_cmp_less(Value left, Value right, ptr(CodeBlock) cb);
    static void _asm_cmp_eq(Value left, Value right, ptr(CodeBlock) cb);
    static void _asm_cmp_less_or_equal(Value left, Value right, ptr(CodeBlock) cb);
    static void _asm_cmp_neq(Value left, Value right, ptr(CodeBlock) cb);
    // NONE OPEARND 
    static void _asm_halt();
    static void _asm_write();
    static void _asm_read();
    // JUMPS
    static void _asm_jump(ptr(CodeBlock) cb);
    static void translate_read();
    static void translate_write(Value val);
    static void translate_assignment(Instruction ins, ptr(CodeBlock) cd);
    static void translate_condition(Instruction ins, ptr(CodeBlock) cd);
    static void translate_ins(Instruction ins, ptr(CodeBlock) cb);
    static void translate_snippet(ptr(CodeBlock) cb);
    static void translate_main();

    static void link_vertices();
    static void preorder_traversal_proc_id(ptr(CodeBlock) cb, std::string proc_id);
    static void spread_proc_name();
    static void save_code(std::string file_name);
};

#endif