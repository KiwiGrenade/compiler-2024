#ifndef AST_HPP
#define AST_HPP

#include <map>
#include <memory>
#include <vector>
#include <fstream>
#include "CodeBlock.hpp"
#include "definitions.hpp"
#define logme_archt(str) { logme(str, "MEM")}

enum State {
    _LOCKED = 0,
    _UNLOCKED = 1
};

struct Register {
    int id;
    State state;
    bool is_argument = false;
    
    Register(int _id) : id(_id), state(_UNLOCKED) {};
    Register(int _id, bool STUD) : id(_id), is_argument(true), state(_UNLOCKED){};
};

struct Memory {
    std::map<ident, ptr(Register)> variables;
    std::vector<ident> arg_ids;
    ptr(Register) ret_reg = nullptr;
};
struct Architecture {
    int var_p;
    std::map<ident, Memory> procedures_memory;

    ptr(Register) reg_a;
    ptr(Register) reg_b;
    ptr(Register) reg_c;
    ptr(Register) reg_d;
    ptr(Register) reg_e;
    ptr(Register) reg_f;
    ptr(Register) reg_g;
    ptr(Register) reg_h;

    void assert_var(ident var_id, ident proc_id){
        procedures_memory[proc_id].variables[var_id] = new_ptr(Register, var_p);
        logme_archt("Add var " + var_id + " ----> " + proc_id);
        var_p++;
    }
    void assert_var_T(ident var_id, int size, ident proc_id){
        warning("AST::Architecture::assert_var_T() not implemented!");
    }
    void assert_arg(ident arg_id, ident proc_id) {
        procedures_memory[proc_id].variables[arg_id] = new_ptr(Register, var_p, "arg");
        procedures_memory[proc_id].arg_ids.push_back(arg_id);
        logme_archt("Add arg " + arg_id + " ----> " + proc_id);
        var_p++;
    }
    void assert_arg_T(ident tab_id, ident proc_id) {
        warning("AST::Architecture::assert_arg_T() not implemented!");
    }
    void assert_ret_reg(ident proc_id) {
        procedures_memory[proc_id].ret_reg = new_ptr(Register, var_p);
    }
};

struct AsmInstruction {
    std::string code;
    int instr_ptr;
    std::shared_ptr<CodeBlock> cb;

    AsmInstruction(std::string _code, int ip) : 
    code(_code), 
    instr_ptr(ip){};
    AsmInstruction(std::string _code, ptr(CodeBlock) _cb, int ip) :
    code(_code),
    instr_ptr(ip),
    cb(_cb){};
};

struct AST {
    // k in virtual machine
    static int                                  instruction_pointer;
    static std::vector<int>                     head_ids;
    static std::map<int, ident>                 head_map;
    static std::vector<ptr(CodeBlock)>          vertices;
    static Architecture                         architecture;
    static std::vector<ptr(AsmInstruction)>     _asm_instructions;

    static void add_vertex(size_t _id, Instruction _ins);
    static void add_empty_vertex(size_t id);
    static void add_edge(int v_id, int u_id);
    static void add_edge(int v_id, int u_id, bool flag);
    static ptr(CodeBlock) get_vertex(int id);
    virtual ~AST() = default;
    
    // ASSEMBLER INSTRUCTIONS
    static void add_instruction(ptr(AsmInstruction) instr);
    
    // EXPRESSIONS
    static void _asm_add(Value val1, ptr(CodeBlock) cb);
    static void _asm_sub(Value val1, ptr(CodeBlock) cb);
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