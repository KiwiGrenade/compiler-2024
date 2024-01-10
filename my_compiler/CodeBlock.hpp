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

//TODO: update content enum
enum content_type {
    _COND = 1,
    _READ = 2,
    _WRITE = 3,
    _ASS = 4,
    _CALL = 16,
    _ENDWHILE = 17
} ;

//TODO: update operator enum
enum operator_type {
    _MUL = 5,
    _DIV = 6,
    _SUB = 7,
    _ADD = 8,
    _MOD = 9,
    _EQ = 10,
    _NEQ = 11,
    _LLEQ = 12,
    _LHEQ = 13,
    _LLESS = 14,
    _LMORE = 15,
    _NONE = 0
} ;

// struct Expression {
//     operator_type type;
//     Value left;
//     Value right;
//     // std::string exp_to_string() {
//     //     return "%%%exp---->" + left.val_to_string() + "____" + std::to_string(type_of_operator) + "____" + right.val_to_string();
//     // }
// };

struct Instruction {
    // bool _while_cond = 0;
    int type_of_instruction;
    // int type_of_operator;
    Value left;
    Value right;
    // Expression expr;
    std::vector<Value> args;
    std::string proc_id;

    Instruction() = default;

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
        int get_begin_id() {
                return _begin_id;
        }
        void set_begin_id(int to_set) {
                _begin_id = to_set;
        }

        int get_end_id() {
                return _end_id;
        }
        void set_end_id(int to_set) {
                _end_id = to_set;
        }
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
            next_true= nullptr;
            next_false= nullptr;
            next_true_id = -1;
            next_false_id = -1;
        }
};
#endif