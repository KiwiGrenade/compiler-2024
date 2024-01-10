#include <iostream>
#include <algorithm>
#include "AST.hpp"

// std::vector<CodeBlock> AST::vertices;

CodeBlock* AST::get_vertex(int _id) {
    CodeBlock* result = nullptr;
    for(auto vert:AST::vertices){
        if(vert.id == _id) {
            result = &vert;
        }
    }
    return result;
}

void AST::add_vertex(size_t id) {
    vertices.push_back(CodeBlock(id));
};

void AST::add_edge(int v_id, int u_id) {
    try
    {
        get_vertex(v_id)->next_true_id = u_id;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void AST::add_edge(int v_id, int u_id, bool flag) {
    try {
        if (flag) {
            AST::get_vertex(v_id)->next_true_id = u_id;
        } else {
            AST::get_vertex(v_id)->next_false_id = u_id;
            std::cout << "dodalem" << u_id << std::endl;
        }
    } catch (const char* msg) {
        std::cerr << msg << std::endl;
    }
}