#ifndef CODE_BLOCK_HPP
#define CODE_BLOCK_HPP
#include <vector>
#include <string>
#include <map>
#include <memory>
#include "definitions.hpp"

enum identifier_type {
    PID,
    TAB_PID,
    TAB_NUM    
};

struct Identifier {
    std::string pid;
    std::string ref_pid;
    identifier_type type;
    long long ref_num;

    Identifier(std::string _name) {
        char last_char = _name[_name.size() - 1];
        if(last_char == ']') {
            char second_last_char = _name[_name.size() - 2];
            if(isdigit(second_last_char)) {
                type = identifier_type::TAB_NUM;
            }
            else {
                type = identifier_type::TAB_PID;
            }
        }
        else {
            type = identifier_type::PID;
        }

        int lbrt_idx = _name.find("[");
        int rbrt_idx = _name.find("]");
        switch(type) {
            case PID:
                pid = _name;
                break;
            case TAB_PID:
                pid = _name.substr(0, lbrt_idx);
                ref_pid = _name.substr(lbrt_idx + 1, _name.size() - rbrt_idx);
                break;
            case TAB_NUM:
                pid = _name.substr(0, lbrt_idx);
                ref_num = std::stoi(_name.substr(lbrt_idx + 1, _name.size() - rbrt_idx));
                break;
        }
    }
};

struct Value {
    long long val = -1;
    std::shared_ptr<Identifier> identifier = nullptr;

    Value() = default;
    Value(std::string _name){
        bool is_tab = false;
        if(isdigit(_name[0])) {
            val = std::stoi(_name);
        }
        else {
            identifier = new_ptr(Identifier, _name);
        }
    };
};

enum content_type {
    _COND = 1,
    _READ = 2,
    _WRITE = 3,
    _ASS = 4,
    _CALL = 16,
    _ENDWHILE = 17
} ;

enum operator_type {
    _MUL = 5,
    _DIV = 6,
    _SUB = 7,
    _ADD = 8,
    _MOD = 9,
    _EQ = 10,
    _NEQ = 11,
    _LESSOREQUAL = 12,
    _MOREOREQUAL = 13,
    _LESS = 14,
    _MORE = 15,
    _NONE = 0
} ;

struct Instruction {
    bool _while_cond = false;
    int type_of_instruction;
    int type_of_operator;
    Value left;
    Value right;
    // Expression expr;
    std::vector<Value> args;
    std::string proc_id;

    Instruction() = default;

    static std::string get_ins_log_header(int ins_type)
    {   
        if(ins_type >= 5 && ins_type <= 9) 
            return "EXPRESSION: ";
        else if(ins_type >= 10 && ins_type <= 15)
            return "CONDITION: ";
        else 
            return "COMMAND: ";
    }
};


struct EdgeProvider {
        int _begin_id;
        int _end_id;
        EdgeProvider(int begin_id, int end_id) : _begin_id(begin_id), _end_id(end_id){};
};

struct Procedure {
    ident name;
    std::map<ident, Address> variables;
    std::map<ident, Address> variables_tab;
    std::map<ident, Address> arguments;
    std::map<ident, Address> arguments_tab;
    Address ret_address = -1;
};

struct CodeBlock {
        bool empty = false;
        bool last = false;
        bool visited = false;
        bool translated = false;
        int ip;
        int id;
        int next_true_id; // = -1;
        int next_false_id; // = -1;
        std::string proc_id;
        std::vector<Instruction> instructions;
        
        std::shared_ptr<CodeBlock> next_true;
        std::shared_ptr<CodeBlock> next_false;
        
        CodeBlock(){}
        CodeBlock(int _id, bool _empty) : id(_id), empty(_empty) {
            next_true = nullptr;
            next_false = nullptr;
            next_true_id = -1;
            next_false_id = -1;
        }
        CodeBlock(int _id, Instruction _ins) : id(_id) {
            instructions.push_back(_ins);
            next_true = nullptr;
            next_false = nullptr;
            next_true_id = -1;
            next_false_id = -1;
        }
};
#endif