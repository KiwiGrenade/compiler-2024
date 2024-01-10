#include "handlers.hpp"
#include "CodeBlock.hpp"
#include "types.hpp"

size_t curr_vertex_id = 0;
std::vector<CodeBlock> AST::vertices;
std::vector<EdgeProvider> providers;

// void chuj() {
//     std::cout << "chuj" << std::endl;
// }

ident handleCondition(ident VAL1, int INS_TYPE, ident VAL2) {
    AST::add_vertex(curr_vertex_id);

    Instruction instruction;
    instruction.type_of_instruction = INS_TYPE;
    // _WRITE -> no LHS
    if((instruction.type_of_instruction != content_type::_WRITE) &&
        (instruction.type_of_instruction != content_type::_READ)) {
        instruction.left = Value(VAL1);
    }
    // _NONE -> no RHS
    if(instruction.type_of_instruction != operator_type::_NONE) {
        instruction.right = Value(VAL2);
    }
    
    // add instructions to added vertex
    AST::vertices[AST::vertices.size() - 1].instructions.push_back(instruction);
    // logger.log("@@@condition: " + instruction.left.load + " <= " + instruction.right.load +"---->"
    //      + std::to_string(instruction.type_of_instruction));
    
    EdgeProvider provider;
    provider.set_begin_id(id);
    provider.set_end_id(id);
    providers.push_back(provider);
    curr_vertex_id++;
    
    return std::to_string(curr_vertex_id - 1);
}

ident handleRepeat(ident COMMANDS_ID, ident CONDITION_ID) {
    int comms_id = stoi(COMMANDS_ID);
    int cond_id = stoi(CONDITION_ID);
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    AST::add_edge(providers[comms_id]._end_id, providers[cond_id]._end_id);
    AST::add_edge(providers[cond_id]._end_id, providers[comms_id]._end_id, false);
    AST::add_edge(providers[comms_id]._end_id, curr_vertex_id, true);
    
    EdgeProvider provider;
    provider._begin_id = providers[cond_id]._begin_id;
    providers.push_back(provider);
    curr_vertex_id++;

    return std::to_string(curr_vertex_id - 1);
}

ident handleIf(ident CONDITION_ID, ident COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int comms_id = stoi(COMMANDS_ID);
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    AST::add_edge(providers[cond_id]._begin_id, providers[comms_id]._begin_id, true);
    AST::add_edge(providers[cond_id]._begin_id, curr_vertex_id, false);
    AST::add_edge(providers[comms_id]._end_id, curr_vertex_id, true);
    
    EdgeProvider provider;
    provider._begin_id = providers[cond_id]._begin_id;
    provider._end_id = curr_vertex_id;
    providers.push_back(provider);

    return std::to_string(curr_vertex_id - 1);
}