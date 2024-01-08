#include "AST.hpp"

void AST::add_vertex(size_t id) {
    vertices.push_back(CodeBlock(id));
};