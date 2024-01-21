#ifndef AST_HPP
#define AST_HPP

#include <unordered_map>
#include <memory>
#include <vector>
#include <fstream>
#include "CodeBlock.hpp"
#include "definitions.hpp"
#define logme_archt(str) { logme(str, "MEM")}

typedef long long Address;

// struct Pointer {
//     Address address;

// }


struct Architecture {
    std::unordered_map<ident, ptr(Procedure)> procedures;

    // ptr(Variable) get_var(ident proc_id, ident name) {
    //     if(!isVar(proc_id, name)) {
    //         error("Could not find var: " + name + " in proc: " + proc_id, true);
    //         return nullptr;
    //     }
    //     else {
    //         return (*procedures[proc_id]->variables)[name];
    //     }
    // }

    // ptr(Argument) get_arg(ident proc_id, ident name) {
    //     if(!isArg(proc_id, name)) {
    //         error("Could not find arg: " + name + " in proc: " + proc_id, true);
    //         return nullptr;
    //     }
    //     else {
    //         return (*procedures[proc_id]->args)[name];
    //     }
    // }

    // ptr(Table) get_tab(ident proc_id, ident name) {
    //     if(!isTable(proc_id, name)) {
    //         error("Could not find table: " + name + " in proc: " + proc_id, true);
    //         return nullptr;
    //     }
    //     else {
    //         return (*procedures[proc_id]->tables)[name];
    //     }
    // }

    // bool isArg(ident proc_id, ident name) {
    //     return (*procedures[proc_id]->args).count(name);
    // }

    // bool isVar(ident proc_id, ident name) {
    //     return (*procedures[proc_id]->variables).count(name);
    // }

    // bool isTable(ident proc_id, ident name) {
    //     return (*procedures[proc_id]->tables).count(name);
    // }
    void add_procedure(ptr(Procedure) proc_id) {
        procedures[proc_id->get_name()] = proc_id;
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
    default:
        return "";
    }
}
struct AsmInstruction {
    int instr_ptr;
    int jump_address = -1;
    bool jump = false;
    std::string code;
    Register _register;
    std::shared_ptr<CodeBlock> cb;
    
    AsmInstruction(std::string _code, Register reg) : 
        code(_code),
        cb(nullptr),
        _register(reg){};
    
    AsmInstruction(std::string _code, ptr(CodeBlock) _cb) :
        code(_code),
        _register(Register::NONE),
        jump(true),
        cb(_cb){};

    AsmInstruction(std::string _code, ptr(CodeBlock), Address _jump_address) :
        code(_code),
        _register(Register::NONE),
        jump(true),
        jump_address(_jump_address){};
};

class AST {
private:
    // ASSEMBLER INSTRUCTIONS
    static void add_instruction(ptr(AsmInstruction) instr);

    static void _asm_put_const(long long val, Register reg);

    static void _asm_load(ptr(Value) val1, Register reg, ptr(CodeBlock) cb);
    // static void _asm_load_var(std::string id, Register reg, ptr(CodeBlock) cb);

    // static void _asm_store_const(Identifier id, long long val, Register reg);
    static void _asm_store(ptr(Value) val, Register reg, ptr(CodeBlock) cb);

    // EXPRESSIONS
    static void _asm_add(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb);
    static void _asm_sub(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb);
    static void _asm_mul(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb);
    static void _asm_div(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb);
    static void _asm_mod(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb);
    // CONDITIONS
    static void _asm_cmp_more(ptr(Value) left, ptr(Value) right, ptr(CodeBlock) cb);
    static void _asm_cmp_eq(ptr(Value) left, ptr(Value) right, ptr(CodeBlock) cb);
    static void _asm_cmp_more_or_equal(ptr(Value) left, ptr(Value) right, ptr(CodeBlock) cb);
    static void _asm_cmp_neq(ptr(Value) left, ptr(Value) right, ptr(CodeBlock) cb);
    // NONE OPEARND 
    static void _asm_halt();
    static void _asm_write();
    static void _asm_read();
    // JUMPS
    static void _asm_jump(ptr(CodeBlock) cb);
    static void _asm_jump(ptr(CodeBlock) cb, Address _jump_address);
    static void _asm_jump_pos(ptr(CodeBlock) cb);
    static void _asm_jump_pos(ptr(CodeBlock) cb, Address _jump_address);
    static void _asm_jump_zero(ptr(CodeBlock) cb);
    static void _asm_jump_zero(ptr(CodeBlock) cb, Address _jump_address);

    static void translate_read(ptr(Value) val, ptr(CodeBlock) cb);
    static void translate_write(ptr(Value) val, ptr(CodeBlock) cb);
    static void translate_assignment(Instruction ins, ptr(CodeBlock) cd);
    static void translate_condition(Instruction ins, ptr(CodeBlock) cd);
    static void translate_ins(Instruction ins, ptr(CodeBlock) cb);
    static void translate_snippet(ptr(CodeBlock) cb);
    static void translate_call(Instruction ins, ptr(CodeBlock) cb);

    static void checkWhile(ptr(CodeBlock));
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
    static void resolve_jumps();
};

#endif