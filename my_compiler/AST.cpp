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
    if (_id == -1)
    {
        return nullptr;
    }
    
    for(ptr(CodeBlock) vert : AST::vertices){
        if(vert->id == _id) {
            return vert;
        }
    }
    throw "Error: node " + std::to_string(_id) + "not found!"; 
}

void AST::add_vertex(size_t _id, Instruction _ins) {
    vertices.push_back(new_ptr(CodeBlock, _id, _ins));
};

void AST::add_empty_vertex(size_t _id) {
    vertices.push_back(new_ptr(CodeBlock, _id, true));
}

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

void AST::_asm_load(Value val, ptr(CodeBlock) cb) {
    // variable
    if(val.type == 2) {
        // std::cout << 
    }
    // constant
    else {
        // add_asm_instruction(new_ptr(AsmInstruction, "RST"));
    }
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

void AST::_asm_jump(ptr(CodeBlock) cb) {
    add_asm_instruction(new_ptr(AsmInstruction, "JUMP", instruction_pointer, cb->ip));
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
    logme_AST("Translate condition: " << ins.type_of_operator);
    switch(ins.type_of_operator) {
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
    logme_AST("Translate assignment: ");
        switch(ins.type_of_operator) {
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
    logme_AST("Translating instruction " << ins.type_of_instruction << " in procedure: " << cb->proc_id);
    switch(ins.type_of_instruction) {
        case _COND:
            translate_condition(ins, cb);
            break;
        case _READ:
            logme_AST("Translate READ");
            _asm_read();
            if(cb->next_true != nullptr && !cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
                _asm_jump(cb->next_true);
                // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            }
            break;
        case _WRITE:
            logme_AST("Translate WRITE");
            _asm_write();
            if(cb->next_true != nullptr && !cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
                _asm_jump(cb->next_true);
                // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            }
            break;
        case _ASS:
            translate_assignment(ins, cb);
            if(cb->next_true != nullptr && !cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
                _asm_jump(cb->next_true);
            //    add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            }
            break;
        case _CALL:
            // translate_call(ins, cb);
            if(cb->next_true != nullptr && !cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
                _asm_jump(cb->next_true);
                // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            }
            break;
        case _ENDWHILE:
            break;
    }
}


void AST::translate_snippet(ptr(CodeBlock) cb){
    if(cb == nullptr || cb->translated) {
        return;
    }
    logme_AST("Snippet translation: [" << cb->id  << "] START");
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
            _asm_jump(cb->next_true);
            // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            // logme_AST("My next is: " << std::to_string(cb->next_true->instructions[0].type_of_instruction));
        }
        logme_AST("after if")
        translate_snippet(cb->next_true);
        translate_snippet(cb->next_false);
    }
    // logme_AST("Snippet translation: [" << cb->id  << "] END");
}

void AST::translate_main() {
    logme_AST("Translate_main: START");

    for(auto it : head_map) {
        if(it.second == "main") {
            auto head = get_vertex(it.first);
            logme_AST("Main head number: " << it.first);
            _asm_jump(head);
            // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", head, instruction_pointer));
        }
    }
    for(auto it : head_ids) {
        logme_AST("Translating head: " << it);
        auto head = get_vertex(it);

        translate_snippet(head);
        logme_AST("Translation of head: " << it << " END");
    }
    logme_AST("Translate_main: END");
}

void AST::link_vertices() {
    logme_AST("Linking vertices: START");
    for(auto ver : vertices) {
        logme_AST("Linking vertice: " << ver->id);
        ver->next_true = get_vertex(ver->next_true_id);
        ver->next_false = get_vertex(ver->next_false_id);
        if (ver->next_false_id == -1 && ver->next_true_id == -1) {
            ver->last = 1;
        }
    }
    logme_AST("Linking vertices: END");
}

void AST::preorder_traversal_proc_id(ptr(CodeBlock) cb, std::string proc_id) {
    if(cb != nullptr && cb-> visited == false) {
        cb->visited = true;
        cb->proc_id = proc_id;
        preorder_traversal_proc_id(cb->next_true, proc_id);
        preorder_traversal_proc_id(cb->next_false, proc_id);
    }
}

void AST::spread_proc_name() {
    for (auto head : head_map) {
        ptr(CodeBlock) ver = get_vertex(head.first);
        preorder_traversal_proc_id(ver, head.second);
    }
}

void AST::save_code(std::string file_name) {
    std::ofstream output (file_name);
    for (auto instr : _asm_instructions) {
        if(instr->jump_address == -1) {
            if(instr->_register == Register::NONE) {
                output << instr->code << "  " << to_string(instr->_register) << std::endl;          
            }
            else {
                output << instr->code << std::endl;                
            }
        }
        else {
            output << instr->code << "  " << instr->jump_address << std::endl;
        }
    }
    output.close();
}