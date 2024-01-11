#ifndef AST_HPP
#define AST_HPP

#include <map>
#include <memory>
#include <vector>
#include "CodeBlock.hpp"
#include "definitions.hpp"

enum State {
    _LOCKED = 0,
    _UNLOCKED = 1
};

struct Register {
    int id;
    State state;
    
    Register(int _id) : id(_id), state(_UNLOCKED) {};
};

struct Memory {
    std::map<ident, ptr(Register)> variables;
    std::vector<ident> arg_ids;
    ptr(Register) ret_reg = nullptr;
};
struct Architecture {
    int var_p;
    std::map<ident, Memory> procedures_memory;
    
    void assert_var(ident var_id, ident proc_id){
        procedures_memory[proc_id].variables[var_id] = new_ptr(Register, var_p);
        var_p++;
    }
};
struct AST {
    static std::vector<CodeBlock> vertices;
    static void add_vertex(size_t id);
    static void add_edge(int v_id, int u_id);
    static void add_edge(int v_id, int u_id, bool flag);
    static CodeBlock& get_vertex(int id);
    static std::map<int, std::string> head_map;
    static Architecture architecture;
    
    virtual ~AST() = default;
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