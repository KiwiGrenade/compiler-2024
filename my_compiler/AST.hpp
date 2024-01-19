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

struct MemoryManager {
    static Address var_p;
    static Address get_address() {
        var_p++;
        return var_p-1;
    }
    // static void zero_var_p() {
    //     var_p = 0;
    // }
};

struct Pointer {
    ident name;
    Address address;
    Pointer(ident _name) : name(_name), address(MemoryManager::get_address()) {};
};

struct Argument : public Pointer {
    int position;
    bool table;
    Argument(ident _name, int _pos, bool _table) : Pointer(_name), position(_pos), table(_table){};
};

struct Variable : public Pointer {
    bool initialized;

    Variable(ident _name) : Pointer(_name),  initialized(false){};
    void set_initialized() {
        initialized = true;
    }
};

struct Table {
    //TODO: change to array
    ident name;
    Address address;
    std::vector<ptr(Variable)> cells;
    
    Table(ident _name, long long size) {
        cells.reserve(size);
        ident cell_name;
        for(long long i = 0; i < size; i++) {
            cell_name = _name + "_" + std::to_string(i);
            cells.push_back(new_ptr(Variable, cell_name));
        }
        address = cells.at(0)->address;
    }
};

struct Procedure {
    ident name;
    std::unordered_map<ident, ptr(Variable)> variables;
    std::unordered_map<ident, ptr(Table)> tables;
    std::map<ident, ptr(Argument)> args;
    int ret_reg = -1;

    Procedure() = default;

    Procedure(ident _name) : name(_name){};
    ptr(Argument) get_arg_at_idx(int _idx) {
        ptr(Argument) result = nullptr;
        for(auto arg : args) {
            if(arg.second->position == _idx) {
                result = arg.second;
            }
        }
        return result;
    }
};

struct Architecture {
    std::unordered_map<ident, ptr(Procedure)> procedures;

    void add_procedure(ptr(Procedure) proc_id) {
        procedures[proc_id->name] = proc_id;
    }

    // void assert_var(ident var_id, ident proc_id){
    //     procedure[proc_id].variables[var_id] = var_p;
    //     logme_archt("Add var " + var_id + " ----> " + proc_id);
    //     var_p++;
    // }
    // void assert_var_T(ident var_id, int size, ident proc_id){
    //     procedure[proc_id].variables_tab[var_id] = std::pair<int,int>(var_p, size);
    //     var_p += size;
    // }
    // void assert_arg(ident arg_id, ident proc_id) {
    //     procedure[proc_id].args[arg_id] = var_p;
    //     logme_archt("Add arg " + arg_id + " ----> " + proc_id);
    //     var_p++;
    // }
    // void assert_arg_T(ident arg_id, ident proc_id) {
    //     procedure[proc_id].args_tab[arg_id] = var_p;
    //     var_p++;
    // }
    // void assert_ret_reg(ident proc_id) {
    //     procedure[proc_id].ret_reg = var_p;
    // }
    // int get_var_addr(ident var_id, ident proc_id) {
    //     return procedure[proc_id].variables[var_id];
    // }
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
    static void _asm_jump_pos(ptr(CodeBlock) cb);
    static void _asm_jump_zero(ptr(CodeBlock) cb);

    static void translate_read(ptr(Value) val, ptr(CodeBlock) cb);
    static void translate_write(ptr(Value) val, ptr(CodeBlock) cb);
    static void translate_assignment(Instruction ins, ptr(CodeBlock) cd);
    static void translate_condition(Instruction ins, ptr(CodeBlock) cd);
    static void translate_ins(Instruction ins, ptr(CodeBlock) cb);
    static void translate_snippet(ptr(CodeBlock) cb);

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