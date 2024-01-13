#ifndef CODE_BLOCK_HPP
#define CODE_BLOCK_HPP
#include <vector>
#include <string>

enum value_type {
    NUM = 1,
    PID = 2,
    TAB_NUM = 3,
    TAB_PID = 4
};

struct Value {
    value_type type;
    std::string name;
    bool is_arg = 0;
    Value() = default;
    Value(std::string _name) : name(_name){
        char last_char = _name[_name.size() - 1];
        char second_last_char = _name[_name.size() - 2];
        if(isdigit(last_char)) {
            type = value_type::NUM;
        }
        else if (last_char == ']') {
            if (isdigit(second_last_char)) {
                type = value_type::TAB_NUM;
            }
            else {
                type = value_type::TAB_PID;
            }
        }
        else {
            type = value_type::PID;
        }
    };
    // std::string val_to_string() {
    //     return "%val% = %type:" + std::to_string(type) + "%load:" + load;
    // }
};

//TODO: merge those two enums?
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
    // int type_of_operator;
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

    std::string to_asm() {
        //switch (type_of_instruction) {
          //  case _type_of_meat::_ASS:
                //aaaa
            return "";
        //}
    }
};


// TODO: get rid of this. It does not make sense.
struct EdgeProvider {
        int _begin_id;
        int _end_id;
        EdgeProvider(int begin_id, int end_id) : _begin_id(begin_id), _end_id(end_id){};
};

class CodeBlock {
    public:
        bool empty = 0;
        bool last = 0;
        bool visited = false;
        bool translated = false;
        int ip;
        int id;
        int next_true_id; // = -1;
        int next_false_id; // = -1;
        std::string proc_id;
        std::vector<Instruction> instructions;
        
        CodeBlock* next_true;
        CodeBlock* next_false;
        
        CodeBlock(){}
        CodeBlock(int _id) :id(_id) {
            next_true = nullptr;
            next_false = nullptr;
            next_true_id = -1;
            next_false_id = -1;
        }
};
#endif