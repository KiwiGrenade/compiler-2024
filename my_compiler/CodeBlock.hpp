#ifndef CODE_BLOCK_HPP
#define CODE_BLOCK_HPP
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include "definitions.hpp"

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
    bool argument;
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

struct Table: public Variable {
    //TODO: change to array
    std::vector<ptr(Variable)> cells;
    
    Table(ident _name, long long size): Variable(_name){
        cells.reserve(size);
        ident cell_name;
        for(long long i = 0; i < size-1; i++) {
            cell_name = _name + "_" + std::to_string(i);
            cells.push_back(new_ptr(Variable, cell_name));
        }
        address = cells.at(0)->address;
    }
};

class Procedure {
    ident name;
    std::shared_ptr<std::unordered_map<ident, ptr(Variable)>> variables;
    std::shared_ptr<std::unordered_map<ident, ptr(Table)>> tables;
    std::shared_ptr<std::map<ident, ptr(Argument)>> args;
    int ret_reg = -1;

public:
    Procedure() {
        variables = std::make_shared<std::unordered_map<ident, ptr(Variable)>>();
        tables = std::make_shared<std::unordered_map<ident, ptr(Table)>>();
        args = std::make_shared<std::map<ident, ptr(Argument)>>();
    };

    Procedure(ident _name) : name(_name){
        variables = std::make_shared<std::unordered_map<ident, ptr(Variable)>>();
        tables = std::make_shared<std::unordered_map<ident, ptr(Table)>>();
        args = std::make_shared<std::map<ident, ptr(Argument)>>();
    };

    bool isVar(ident name) {

        return (*variables).count(name);
    }

    bool isArg(ident name) {
        return (*args).count(name);
    }
    
    bool isTab(ident name) {
        return (*tables).count(name);
    }

    ident get_name() {
        return name;
    }

    void set_name(ident _name) {
        name = _name;
    }


    ptr(Variable) get_var(ident name) {
        if(isVar(name)) {
            return (*variables)[name];
        }
        return nullptr;
    }

    void add_var(ptr(Variable) _var) {
        (*variables)[_var->name] = _var;
    }

    ptr(Variable) get_tab(ident name) {
        if(isTab(name)) {
            return (*tables)[name];
        }
        return nullptr;
    }
    
    void add_tab(ptr(Table) _tab) {
        (*tables)[_tab->name] = _tab;
    }
    
    ptr(Argument) get_arg(ident name) {
        if(isArg(name)) {
            return (*args)[name];
        }
        return nullptr;
    }
    
    void add_arg(ptr(Argument) _arg) {
        (*args)[_arg->name] = _arg;
    }
    
    ptr(Argument) get_arg_at_idx(int _idx) {
        ptr(Argument) result = nullptr;
        for(auto arg : *args) {
            if(arg.second->position == _idx) {
                result = arg.second;
            }
        }
        return result;
    }
};


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
};

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
};

struct Expression {
    ptr(Value) left;
    ptr(Value) right;
    int op;
    Expression(ptr(Value) _left, int _op, ptr(Value) _right): 
    left(_left),
    op(_op),
    right(_right){};
};

struct Instruction {
    bool _while_cond = false;
    content_type type_of_instruction;
    ptr(Expression) expr;
    ptr(Value) lvalue;
    std::vector<ptr(Pointer)> args;
    std::string proc_id;

    Instruction() = default;
};


struct EdgeProvider {
        int _begin_id;
        int _end_id;
        EdgeProvider(int begin_id, int end_id) : _begin_id(begin_id), _end_id(end_id){};
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