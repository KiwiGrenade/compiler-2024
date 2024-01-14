#include <iostream>
#include <algorithm>
#include "AST.hpp"

// std::vector<AsmInstruction> _asm_instructions;
std::vector<ptr(AsmInstruction)>  AST::_asm_instructions;
int                          AST::instruction_pointer = 0;


std::string AST_log_head = "A";
#define logme_AST(str) { logme(str, AST_log_head) }

CodeBlock& AST::get_vertex(int _id) {
    for(CodeBlock& vert : AST::vertices){
        if(vert.id == _id) {
            return vert;
        }
    }
    throw "Error: node " + std::to_string(_id) + "not found!"; 
}

void AST::add_vertex(size_t id) {
    vertices.push_back(CodeBlock(id));
};

void AST::add_edge(int v_id, int u_id, bool flag) {
    try {
        std::string str_flag;
        if (flag) {
            AST::get_vertex(v_id).next_true_id = u_id;
        } else {
            AST::get_vertex(v_id).next_false_id = u_id;
        }
        logme_AST("dodano krawedz: " + std::to_string(flag) + "(" + std::to_string(v_id) + ", " + std::to_string(u_id) + ")");
        // logger.LOG("dodano krawedz: " + std::to_string(flag) + "(" + std::to_string(v_id) + ", " + std::to_string(u_id) + ")\n");
    } catch (const char* msg) {
        std::cerr << msg << std::endl;
    }
}

void add_asm_instruction(ptr(AsmInstruction) i) {
    AST::_asm_instructions.push_back(i);
    AST::instruction_pointer++;
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

        // translate_snippet(head);
        logme_AST("Translation of head: " << it << " END");
    }

    // _asm_halt();
}

void AST::_asm_halt(ptr(CodeBlock) cb) {
    add_asm_instruction(new_ptr(AsmInstruction, "HALT", instruction_pointer));
}