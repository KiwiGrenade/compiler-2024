#include <iostream>
#include <algorithm>
#include <fstream>
#include "AST.hpp"

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
        // logme_AST("dodano krawedz: " << flag << "(" << v_id << ", " << u_id << ")");
    } catch (const char* msg) {
        std::cerr << msg << std::endl;
    }
}

void add_asm_instruction(ptr(AsmInstruction) i) {
    AST::_asm_instructions.push_back(i);
    AST::instruction_pointer++;
}

void AST::_asm_read() {
    add_asm_instruction(new_ptr(AsmInstruction, "READ", Register::NONE));
}

void AST::_asm_write() {
    add_asm_instruction(new_ptr(AsmInstruction, "WRITE", Register::NONE));
}

void AST::_asm_halt() {
    add_asm_instruction(new_ptr(AsmInstruction, "HALT", Register::NONE));
}

void AST::_asm_jump(ptr(CodeBlock) cb) {
    add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb));
}


// uses only reg
void AST::_asm_put_const(long long val, Register reg) {
    logme_AST(val);
    add_asm_instruction(new_ptr(AsmInstruction, "RST", reg));
    std::vector<bool> bits;
    while (val > 0) {
        bits.push_back(val % 2);
        val/=2;
    }
    for(auto i = bits.rbegin(); i != bits.rend(); i++) {
        if (*i) {
            add_asm_instruction(new_ptr(AsmInstruction, "SHL", reg));
            add_asm_instruction(new_ptr(AsmInstruction, "INC", reg));
        } else {
            add_asm_instruction(new_ptr(AsmInstruction, "SHL", reg));
        }
    }
}


// uses reg: A, h, REG -> register A is now loades with vlue of val
void AST::_asm_load(ptr(Value) val, Register reg, ptr(CodeBlock) cb) {
    long long address;
    long long n;
    if(val->identifier == nullptr) {
        _asm_put_const(val->val, reg);
    }
    else {
        // if(architecture.procedures[cb->proc_id]->variables.count(val->identifier.pid)) {
        switch(val->identifier->type) {
            case PID:
                // get var addres
                address = architecture.procedures[cb->proc_id]->variables[val->identifier->pid]->address;
                _asm_put_const(address, reg);
                add_asm_instruction(new_ptr(AsmInstruction, "LOAD", reg));
                break;
            case TAB_NUM:
                address = architecture.procedures[cb->proc_id]->tables[val->identifier->pid]->address;
                address += val->identifier->ref_num;
                // logme_AST("REFNUM: " + val->identifier->ref_num);
                _asm_put_const(address, reg);
                add_asm_instruction(new_ptr(AsmInstruction, "LOAD", reg));
                break;
            case TAB_PID:
                address = architecture.procedures[cb->proc_id]->variables[val->identifier->ref_pid]->address;
                logme_AST("ref_pid address: " + std::to_string(address));
                _asm_put_const(address, reg);
                add_asm_instruction(new_ptr(AsmInstruction, "LOAD", reg));
                
                // ptr(Value) under ref_pid is now in reg A
                
                // load address of pid into H
                address = architecture.procedures[cb->proc_id]->tables[val->identifier->pid]->address;
                logme_AST("pid address: " + std::to_string(address));
                _asm_put_const(address, Register::H);
                add_asm_instruction(new_ptr(AsmInstruction, "ADD", Register::H));
                // reg A = pid_addr + ref_pid_val                 
                add_asm_instruction(new_ptr(AsmInstruction, "LOAD", Register::A));
                // ptr(Value) under [pid_addr + ref_pid_val] is now in reg A
                break;
        }
    }
}

// void AST:::_asm_store_var(Identifier ident, long long val, )

// uses A, H and REG -> vals ptr(Value) is now equall to contents of reg A
void AST::_asm_store(ptr(Value) val, Register reg, ptr(CodeBlock) cb) {
    long long address;
    ptr(Procedure) curr_proc = architecture.procedures[cb->proc_id];
    // if(curr_proc.)

    switch(val->identifier->type)
    {
        case PID:
            address = curr_proc->variables[val->identifier->pid]->address;
            _asm_put_const(address, reg);
            add_asm_instruction(new_ptr(AsmInstruction, "STORE", reg));
            break;
        case TAB_NUM:
            address = curr_proc->tables[val->identifier->pid]->address;
            address += val->identifier->ref_num;
            _asm_put_const(address, reg);
            add_asm_instruction(new_ptr(AsmInstruction, "STORE", reg));
            break;
        case TAB_PID:
            // H = A
            add_asm_instruction(new_ptr(AsmInstruction, "PUT", Register::H));
            // load into A val of ref_pid
            address = architecture.procedures[cb->proc_id]->variables[val->identifier->ref_pid]->address;
            _asm_put_const(address, Register::A);
            add_asm_instruction(new_ptr(AsmInstruction, "LOAD", Register::A));

            // load address of pid into H
            address = architecture.procedures[cb->proc_id]->tables[val->identifier->pid]->address;
            _asm_put_const(address, reg);
            add_asm_instruction(new_ptr(AsmInstruction, "ADD", Register::H));
            // reg A = pid_addr + ref_pid_val
            add_asm_instruction(new_ptr(AsmInstruction, "STORE", reg));
        break;
    }
            // add_asm_instruction(new_ptr(AsmInstruction, "STORE", reg));

}

// void AST::_asm_store_const(Identifier ident, long long val, Register reg) {
//     // _asm_load_const(val, reg);
//     // add_asm_instruction()
// }

void AST::translate_read(ptr(Value) val, ptr(CodeBlock) cb) {
    _asm_read();
    // do not use register A here!
    _asm_store(val, Register::B, cb);
}

void AST::translate_write(ptr(Value) val, ptr(CodeBlock) cb) {
    _asm_load(val, Register::B, cb);
    // do not use register A here!
    _asm_write();
}


void AST::_asm_cmp_less(ptr(Value) left, ptr(Value) right, ptr(CodeBlock) cb) {
    warning("AST::_asm_cmp_less() not implemented!");
}
void AST::_asm_cmp_eq(ptr(Value) left, ptr(Value) right, ptr(CodeBlock) cb) {
    warning("AST::_asm_cmp_eq() not implemented!");
}
void AST::_asm_cmp_less_or_equal(ptr(Value) left, ptr(Value) right, ptr(CodeBlock) cb) {
    warning("AST::_asm_cmp_less_or_equal() not implemented!");
}
void AST::_asm_cmp_neq(ptr(Value) left, ptr(Value) right, ptr(CodeBlock) cb) {
    warning("AST::_asm_cmp_neq() not implemented!");
}

void AST::_asm_add(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb) {
    _asm_load(val1, Register::B, cb);
    add_asm_instruction(new_ptr(AsmInstruction, "PUT", Register::B));
    _asm_load(val2, Register::C, cb);
    add_asm_instruction(new_ptr(AsmInstruction, "PUT", Register::B));

    // sum is in reg A
    add_asm_instruction(new_ptr(AsmInstruction, "GET", Register::B));
    add_asm_instruction(new_ptr(AsmInstruction, "ADD", Register::C));
}
void AST::_asm_sub(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb) {
    _asm_load(val1, Register::B, cb);
    add_asm_instruction(new_ptr(AsmInstruction, "PUT", Register::B));
    _asm_load(val2, Register::C, cb);
    add_asm_instruction(new_ptr(AsmInstruction, "PUT", Register::C));

    add_asm_instruction(new_ptr(AsmInstruction, "GET", Register::B));
    add_asm_instruction(new_ptr(AsmInstruction, "SUB", Register::C));
}
void AST::_asm_mul(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb) {
    warning("AST::_asm_mul() not implemented!");
}
void AST::_asm_div(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb) {
    warning("AST::_asm_div() not implemented!");
}
void AST::_asm_mod(ptr(Value) val1, ptr(Value) val2, ptr(CodeBlock) cb) {
    warning("AST::_asm_mod() not implemented!");
}
    

void AST::translate_condition(Instruction ins, ptr(CodeBlock) cb) {
    logme_AST("Translate condition: ");
    ptr(Value) left = ins.expr->left;
    ptr(Value) right = ins.expr->right;
    switch(ins.expr->op) {
        case _LESS:
            _asm_cmp_less(left, right, cb);
            break;
        case _MORE:
            _asm_cmp_less(right, left, cb);
            break;
        case _EQ:
            _asm_cmp_eq(left, right, cb);
            break;
        case _LESSOREQUAL:
            _asm_cmp_less_or_equal(left, right, cb);
            break;
        case _MOREOREQUAL:
            _asm_cmp_less_or_equal(right, left, cb);
            break;
        case _NEQ:
            _asm_cmp_neq(left, right, cb);
            break;
    }
}

// uses register A, register B and register H
void AST::translate_assignment(Instruction ins, ptr(CodeBlock) cb) {
    logme_AST("Translate assignment: ");
    ptr(Value) left = ins.expr->left;
    ptr(Value) right = ins.expr->right;
        switch(ins.expr->op) {
        case _ADD:
            _asm_add(left, right, cb);
            break;
        case _SUB:
            _asm_sub(left, right, cb);
            break;
        case _MUL:
            _asm_mul(left, right, cb);
            break;
        case _DIV:
            _asm_div(left, right, cb);
            break;
        case _MOD:
            _asm_mod(left, right, cb);
            break;
        case _NONE:
            _asm_load(left, Register::A, cb);
            break;
    }
    _asm_store(ins.lvalue, Register::B, cb);
}

void AST::translate_ins(Instruction ins, ptr(CodeBlock) cb){
    logme_AST("Translating instruction " << ins.type_of_instruction << " in procedures: " << cb->proc_id);
    switch(ins.type_of_instruction) {
        case _COND:
            translate_condition(ins, cb);
            break;
        case _READ:
            logme_AST("Translate READ");
            translate_read(ins.expr->right, cb);
            // if(cb->next_true != nullptr && !cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
            //     _asm_jump(cb->next_true);
            //     // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            // }
            break;
        case _WRITE:
            logme_AST("Translate WRITE");
            translate_write(ins.expr->right, cb);
            // if(cb->next_true != nullptr && !cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
            //     _asm_jump(cb->next_true);
            //     // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            // }
            break;
        case _ASS:
            translate_assignment(ins, cb);
            // if(cb->next_true != nullptr && !cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
            //     _asm_jump(cb->next_true);
            // //    add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            // }
            break;
        case _CALL:
            // translate_call(ins, cb);
            // if(cb->next_true != nullptr && !cb->next_true->empty && cb->next_true->instructions[0]._while_cond) {
            //     _asm_jump(cb->next_true);
            //     // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            // }
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
    
    // logme_AST("before if");
    if(cb->next_true == nullptr) {
        logme_AST("End of procedures: " + cb->proc_id) 
        if (cb->proc_id == "main") {
            _asm_halt();
        }
        // else {
        //     _asm_jump_i(cb);
        // }
    }
    else {
        if (cb->empty) {
            // cb->translated = false;
            // _asm_jump(cb->next_true);
            // add_asm_instruction(new_ptr(AsmInstruction, "JUMP", cb->next_true, instruction_pointer));
            // logme_AST("My next is: " << std::to_string(cb->next_true->instructions[0].type_of_instruction));
        }
        // cb->translated = false;
        _asm_jump(cb->next_true);
        // logme_AST("after if")
        translate_snippet(cb->next_true);
        translate_snippet(cb->next_false);
    }
    logme_AST("Snippet translation: [" << cb->id  << "] END");
}

void AST::translate_main() {
    logme_AST("Translate_main: START");

    for(auto it : head_map) {
        if(it.second == "main") {
            auto head = get_vertex(it.first);
            logme_AST("Main head number: " << it.first);
            _asm_jump(head);
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
    logme_AST("############## Linking vertices: START ##############");
    for(auto ver : vertices) {
        logme_AST("Linking vertice: " << ver->id);
        logme_AST("next_true_id: " << ver->next_true_id);
        logme_AST("next_false_id: " << ver->next_false_id);
        ver->next_true = get_vertex(ver->next_true_id);
        ver->next_false = get_vertex(ver->next_false_id);
        if (ver->next_false_id == -1 && ver->next_true_id == -1) {
            ver->last = 1;
        }
    }
    logme_AST("############## Linking vertices: END ################");
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

void AST::resolve_jumps() {
    for(auto ins : _asm_instructions) {
        if(ins->code == "JUMP") {
            ins->jump_address = ins->cb->ip;
        }
    }
}

void AST::save_code(std::string file_name) {
    std::ofstream output (file_name);
    for (auto instr : _asm_instructions) {
        if(instr->jump_address == -1) {
            if(instr->_register != Register::NONE) {
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