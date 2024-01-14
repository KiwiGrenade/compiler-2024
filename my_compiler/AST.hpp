#ifndef AST_HPP
#define AST_HPP

#include <map>
#include <memory>
#include <vector>
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
    AsmInstruction(std::string _code, int ip) : code(_code), instr_ptr(ip){};
};

struct AST {
    static int                          instruction_pointer;
    static std::vector<int>             head_ids;
    static std::map<int, ident>         head_map;
    static std::vector<ptr(CodeBlock)>       vertices;
    static Architecture                 architecture;
    static std::vector<ptr(AsmInstruction)>  _asm_instructions;

    static void add_vertex(size_t id);
    static void add_edge(int v_id, int u_id);
    static void add_edge(int v_id, int u_id, bool flag);
    static ptr(CodeBlock) get_vertex(int id);
    virtual ~AST() = default;
    // ASSEMBLER INSTRUCTIONS
    static void add_instruction(ptr(AsmInstruction) instr);
    static void _asm_halt(ptr(CodeBlock));
    static void translate_main();
};

// enum table_ref_type {
//     NUM = 1,
//     PID = 2
// };

// struct AST_num : AST {
//     uint64_t val;
//     AST_num(uint64_t _val) : val(_val){};
// };

// struct AST_pidentifier : AST {
//     std::string pidentifier;
//     AST_pidentifier(std::string _pidentifier) : pidentifier(_pidentifier){};
// };

// struct Table_ref : AST {
//     table_ref_type type;
//     std::string var_name;
    
//     uint64_t idx_num;
//     std::string idx_pid;
    
//     Table_ref(std::string _var_name, table_ref_type _type, std::string _idx):
//     var_name(_var_name),
//     type(_type){
//         if(type == table_ref_type::NUM) {
//             idx_num = std::stoull(_idx);
//         }
//         else {
//             idx_pid = _idx;
//         }
//     }
// };

// enum identifier_type {
//     var = 1,
//     table = 2
// };

// struct Identifier : AST {
//     identifier_type type;
//     std::string name;
//     ptr(Table_ref) table_id;


//     Identifier(ptr(Table_ref) _table_id) : table_id(_table_id){};
//     Identifier(std::string _name) : name(_name) {};
// };

// enum value_type {
//     val_num = 1,
//     val_idt = 2
// };

// struct Value : AST{
//     value_type type;
//     std::string hel;
// };


#endif