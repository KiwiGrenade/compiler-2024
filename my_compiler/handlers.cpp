#include "definitions.hpp"
#include "types.hpp"

size_t vertex_id = 0;
std::vector<CodeBlock> AST::vertices;

// void chuj() {
//     std::cout << "chuj" << std::endl;
// }

ident handleCondition(ident VAL1, int OP_TYPE, ident VAL2)
{
    size_t curr_vertex_id = vertex_id;
    AST::add_vertex(curr_vertex_id);
    Instruction instruction;
    instruction.type_of_instruction = content_type::_COND;
    instruction.type_of_operator = OP_TYPE;
    // _WRITE -> no LHS
    if(instruction.type_of_operator != content_type::_WRITE){
        instruction.left = Value(VAL1);
    }
    // _NONE -> no RHS
    if(instruction.type_of_operator != operator_type::_NONE){
        instruction.right = Value(VAL2);
    }
    AST::vertices[AST::vertices.size() - 1].instructions.push_back(instruction);
    // logger.log("@@@condition: " + instruction.left.load + " <= " + instruction.right.load +"---->"
    //      + std::to_string(instruction.type_of_instruction));
    // EdgeProvider provider;
    // provider.set_begin_id(id);
    // provider.set_end_id(id);
    // providers.push_back(provider);
    vertex_id++;
    return std::to_string(curr_vertex_id);
}
