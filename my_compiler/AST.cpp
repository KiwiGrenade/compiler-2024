#include <iostream>
#include <algorithm>
#include <fstream>
#include "AST.hpp"

// std::vector<AsmInstruction> _asm_instructions;
int                                 AST::instruction_pointer = 0;
std::vector<ptr(AsmInstruction)>    AST::_asm_instructions;


std::string AST_log_head = "A";
#define logme_AST(str) { logme(str, AST_log_head) }

ptr(CodeBlock) AST::get_vertex(int _id) {
    for(ptr(CodeBlock) vert : AST::vertices){
        if(vert->id == _id) {
            return vert;
        }
    }
    throw "Error: node " + std::to_string(_id) + "not found!"; 
}

void AST::add_vertex(size_t id) {
    vertices.push_back(new_ptr(CodeBlock, id));
};

void AST::add_edge(int v_id, int u_id, bool flag) {
    try {
        std::string str_flag;
        if (flag) {
            AST::get_vertex(v_id)->next_true_id = u_id;
        } else {
            AST::get_vertex(v_id)->next_false_id = u_id;
        }
        logme_AST("dodano krawedz: " << flag << "(" << v_id << ", " << u_id << ")");
    } catch (const char* msg) {
        std::cerr << msg << std::endl;
    }
}

void add_asm_instruction(ptr(AsmInstruction) i) {
    AST::_asm_instructions.push_back(i);
    AST::instruction_pointer++;
}

void AST::_asm_read() {
    add_asm_instruction(new_ptr(AsmInstruction, "READ", instruction_pointer));
}

void AST::_asm_write() {
    add_asm_instruction(new_ptr(AsmInstruction, "WRITE", instruction_pointer));
}

void AST::_asm_halt() {
    add_asm_instruction(new_ptr(AsmInstruction, "HALT", instruction_pointer));
}

void AST::_asm_cmp_less(Value left, Value right, ptr(CodeBlock) cb) {
    warning("AST::_asm_cmp_less() not implemented!");
}
void AST::_asm_cmp_eq(Value left, Value right, ptr(CodeBlock) cb) {
    warning("AST::_asm_cmp_eq() not implemented!");
}
void AST::_asm_cmp_less_or_equal(Value left, Value right, ptr(CodeBlock) cb) {
    warning("AST::_asm_cmp_less_or_equal() not implemented!");
}
void AST::_asm_cmp_neq(Value left, Value right, ptr(CodeBlock) cb) {
    warning("AST::_asm_cmp_neq() not implemented!");
}

void AST::_asm_add(Value val1, ptr(CodeBlock) cb) {
    warning("AST::_asm_add() not implemented!");
}
void AST::_asm_sub(Value val1, ptr(CodeBlock) cb) {
    warning("AST::_asm_sub() not implemented!");
}
void AST::_asm_mul(Value val1, Value val2, ptr(CodeBlock) cb) {
    warning("AST::_asm_mul() not implemented!");
}
void AST::_asm_div(Value val1, Value val2, ptr(CodeBlock) cb) {
    warning("AST::_asm_div() not implemented!");
}
void AST::_asm_mod(Value val1, Value val2, ptr(CodeBlock) cb) {
    warning("AST::_asm_mod() not implemented!");
}
    

void AST::translate_condition(Instruction ins, ptr(CodeBlock) cb) {
    logme_AST("Translate condition: ")
    switch(ins.type_of_instruction) {
        case _LESS:
            _asm_cmp_less(ins.left, ins.right, cb);
            break;
        case _MORE:
            _asm_cmp_less(ins.right, ins.left, cb);
            break;
        case _EQ:
            _asm_cmp_eq(ins.left, ins.right, cb);
            break;
        case _LESSOREQUAL:
            _asm_cmp_less_or_equal(ins.left, ins.right, cb);
            break;
        case _MOREOREQUAL:
            _asm_cmp_less_or_equal(ins.right, ins.left, cb);
            break;
        case _NEQ:
            _asm_cmp_neq(ins.left, ins.right, cb);
            break;
    }
}

void AST::translate_assignment(Instruction ins, ptr(CodeBlock) cb) {
    logme_AST("Translate assignment: ")
        switch(ins.type_of_instruction) {
        case _ADD:
            _asm_add(ins.right, cb);
            break;
        case _SUB:
            _asm_sub(ins.right, cb);
            break;
        case _MUL:
            _asm_mul(ins.left, ins.right, cb);
            break;
        case _DIV:
            _asm_div(ins.left, ins.right, cb);
            break;
        case _MOD:
            _asm_mod(ins.left, ins.right, cb);
            break;
    }
}

void AST::translate_ins(Instruction ins, ptr(CodeBlock) cb){
    logme_AST("Translating instructions in procedure: " << cb->proc_id);
    switch(ins.type_of_instruction) {
        case _COND:
            translate_condition(ins, cb);
            break;
        case _READ:
            logme_AST("Translate READ");
            _asm_read();
            if(cb->next_true != nullptr && cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
                add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            }
            break;
        case _WRITE:
            logme_AST("Translate WRITE");
            _asm_write();
            if(cb->next_true != nullptr && cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
                add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            }
            break;
        case _ASS:
            translate_assignment(ins, cb);
            if(cb->next_true != nullptr && cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
                add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            }
            break;
        case _CALL:
            // translate_call(ins, cb);
            if(cb->next_true != nullptr && cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
                add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            }
            break;
        case _ENDWHILE:
            break;
    }
}


void AST::translate_snippet(ptr(CodeBlock) cb){
    logme_AST("Snippet translation: START");
    if(cb == nullptr || cb->translated) {
        return;
    }
    else {
        cb->ip = instruction_pointer;
        for(auto instruction : cb->instructions) {
            translate_ins(instruction, cb);
        }
        cb->translated = true;
        
        logme_AST("before if");
        if(cb->next_true == nullptr) {
            logme_AST("End of procedure: " + cb->proc_id) 
            if (cb->proc_id == "main") {
                _asm_halt();
            }
            else {
                // _asm_jump_i(cb);
            }
        }
        else {
            if (cb->empty) {
                cb->translated = false;
                // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer))
                logme_AST("My next is: " << cb->next_true->instructions[0].type_of_instruction);
            }
            logme_AST("after if")
            translate_snippet(cb->next_true);
            translate_snippet(cb->next_false);
        }
    }
    logme_AST("Snippet translation: END");
}

void AST::translate_main() {
    logme_AST("Translate_main: START");

    for(auto it : head_map) {
        if(it.second == "main") {
            auto head = get_vertex(it.first);
            logme_AST("Main head number: " << it.first);
            instruction_pointer++;
        }
    }
    for(auto it : head_ids) {
        logme_AST("Translating head: " << it);
        auto head = get_vertex(it);

        translate_snippet(head);
        logme_AST("Translation of head: " << it << " END");
    }

    // _asm_halt();
}


void AST::save_code(std::string file_name) {
    std::ofstream output (file_name);
    for (auto asmins : _asm_instructions) {
        output << asmins->code << std::endl;
        // if (asmins.jump_address == -1) {
        //     if (asmins._register != nullptr) {
        //         //log.log(std::to_string(asmins.ip) + "   " + asmins.code + "        " + std::to_string(asmins._register->id) + asmins.constant + asmins.label);
        //         output <<  "   " + asmins.code + "        " + std::to_string(asmins.reg_id) + asmins.constant + asmins.label << std::endl;
        //     } else {
        //         //log.log(std::to_string(asmins.ip) + "   " + asmins.code + "        " +asmins.constant + asmins.label);
        //                    output << "   " + asmins.code + "        " +asmins.constant + asmins.label << std::endl;;

        //     }
        // } else {
        //         //log.log(std::to_string(asmins.ip) + "   " + asmins.code + "        " + std::to_string(asmins.jump_address) + asmins.constant + asmins.label);
        //            output << "   " + asmins.code + "        " + std::to_string(asmins.jump_address) + asmins.constant + asmins.label << std::endl;

        // }
    }
    output.close();
}