#ifndef AST_HPP
#define AST_HPP

#include <vector>
#include "CodeBlock.hpp"

struct AST {
    static std::vector<CodeBlock> vertices;
    virtual ~AST() = default;
    static void add_vertex(size_t id);
    static void add_edge(int v_id, int u_id);
    static void add_edge(int v_id, int u_id, bool flag);
    static CodeBlock& get_vertex(int id);
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