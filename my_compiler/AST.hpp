#ifndef AST_HPP
#define AST_HPP

#include <map>
#include <memory>
#include <vector>
#include <fstream>
#include "CodeBlock.hpp"
#include "definitions.hpp"
#define logme_archt(str) { logme(str, "MEM")}



struct Memory {
    std::map<ident, int> variables;
    std::map<ident, std::pair<int,int>> variables_tab;
    std::map<ident, int> args;
    std::map<ident, int> args_tab;
    int ret_reg = -1;
};

struct Architecture {
    int var_p = 0;
    std::map<ident, Memory> procedures_memory;

    void assert_var(ident var_id, ident proc_id){
        procedures_memory[proc_id].variables[var_id] = var_p;
        logme_archt("Add var " + var_id + " ----> " + proc_id);
        var_p++;
    }
    void assert_var_T(ident var_id, int size, ident proc_id){
        procedures_memory[proc_id].variables_tab[var_id] = std::pair<int,int>(var_p, size);
        var_p += size;
    }
    void assert_arg(ident arg_id, ident proc_id) {
        procedures_memory[proc_id].args[arg_id] = var_p;
        logme_archt("Add arg " + arg_id + " ----> " + proc_id);
        var_p++;
    }
    void assert_arg_T(ident arg_id, ident proc_id) {
        procedures_memory[proc_id].args_tab[arg_id] = var_p;
        var_p++;
    }
    void assert_ret_reg(ident proc_id) {
        procedures_memory[proc_id].ret_reg = var_p;
    }
    int get_var_addr(ident var_id, ident proc_id) {
        return procedures_memory[proc_id].variables[var_id];
    }
};

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

class AST {
private:
    // ASSEMBLER INSTRUCTIONS
    static void add_instruction(ptr(AsmInstruction) instr);

    static void _asm_put_const(long long val, Register reg);

    static void _asm_load(Value val1, Register reg, ptr(CodeBlock) cb);
    // static void _asm_load_var(std::string id, Register reg, ptr(CodeBlock) cb);

    static void _asm_store_const(Identifier id, long long val, Register reg);
    static void _asm_store(Value val, ptr(CodeBlock) cb);

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
    static void translate_read(Value val, ptr(CodeBlock) cb);
    static void translate_write(Value val, ptr(CodeBlock) cb);
    static void translate_assignment(Instruction ins, ptr(CodeBlock) cd);
    static void translate_condition(Instruction ins, ptr(CodeBlock) cd);
    static void translate_ins(Instruction ins, ptr(CodeBlock) cb);
    static void translate_snippet(ptr(CodeBlock) cb);

public:
    // k in virtual machine 
    static int                                  instruction_pointer;
    static std::vector<int>                     head_ids;
    static std::map<int, ident>                 head_map;
    static std::vector<ptr(CodeBlock)>          vertices;
    static Architecture                         architecture;
    static std::vector<ptr(AsmInstruction)>     _asm_instructions;

    static void add_vertex(size_t _id, Instruction _ins);
    static void add_empty_vertex(size_t id);
    static ptr(CodeBlock) get_vertex(int id);
    static void add_edge(int v_id, int u_id, bool flag);
    virtual ~AST() = default;
    static void link_vertices();
    static void preorder_traversal_proc_id(ptr(CodeBlock) cb, std::string proc_id);
    static void spread_proc_name();
    static void save_code(std::string file_name);
    static void translate_main();
};

#endif