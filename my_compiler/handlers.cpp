#include "handlers.hpp"
#include "CodeBlock.hpp"
#include "types.hpp"
#include "Logger.hpp"

bool                        head_sig;
size_t                      curr_vertex_id = 0;
std::vector<CodeBlock>      AST::vertices;
std::vector<EdgeProvider>   providers;
std::vector<ident>          procedures;
std::map<int, std::string>  AST::head_map;
Architecture                AST::architecture;
std::vector<int>            AST::head_ids;
Logger::Logger logger =     Logger::Logger("logs.log");
// void chuj() {
//     std::cout << "chuj" << std::endl;
// }

void set_head() {
    if (head_sig) {
        AST::head_ids.push_back(curr_vertex_id);
        AST::head_map[curr_vertex_id] = "";
        std::string log_msg = "&&&codeblock glowa z id:" + std::to_string(curr_vertex_id); 
        logger.LOG(log_msg);
    }
    head_sig = 0;
}

ident handleCondition(ident VAL1, int INS_TYPE, ident VAL2) {
    set_head();

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
    logger.LOG("@@@condition: " + instruction.left.load + " <= " + instruction.right.load +"---->"
         + std::to_string(instruction.type_of_instruction));
    
    EdgeProvider provider;
    provider.set_begin_id(curr_vertex_id);
    provider.set_end_id(curr_vertex_id);
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

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleIfElse(ident CONDITION_ID, ident IF_COMMANDS_ID, ident ELSE_COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int if_comms_id = stoi(IF_COMMANDS_ID);
    int el_comms_id = stoi(ELSE_COMMANDS_ID);

    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    //TODO: this may be problematic
    AST::add_edge(providers[cond_id]._begin_id, providers[if_comms_id]._begin_id, true);
    AST::add_edge(providers[cond_id]._begin_id, providers[el_comms_id]._begin_id, false);
    AST::add_edge(providers[if_comms_id]._end_id, curr_vertex_id);
    AST::add_edge(providers[el_comms_id]._end_id, curr_vertex_id);

    EdgeProvider provider;
    provider._begin_id = providers[cond_id]._begin_id;
    provider._end_id = curr_vertex_id;
    providers.push_back(provider);

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);   
}


ident handleProcCall(ident PROC_CALL) {
    ident proc_name;
    ident tmp_arg_name;
    Instruction instruction;
    instruction.type_of_instruction = content_type::_CALL;
    
    //TODO: check if you can optimise this
    //TODO: include tables into arguments
    bool is_function = false;
    for(auto c : PROC_CALL) {
        if (c == '(') {
            is_function = true;
        }
        if (is_function) {
            if (c == ',' || c == ')') {
                instruction.args.push_back(Value(tmp_arg_name));
                tmp_arg_name = "";
            }
            else {
                tmp_arg_name += c;
            }
        }
        else {
            proc_name += c;
        }
    }

    proc_name += '_' + std::to_string(instruction.args.size());
    
    //TODO: you can optimize this
    for (auto p : procedures) {
        if(p == proc_name) { 
            instruction.proc_id = proc_name;
            AST::add_vertex(curr_vertex_id);
            AST::vertices[AST::vertices.size() - 1].instructions.push_back(instruction);
            
            EdgeProvider provider;
            provider.set_begin_id(curr_vertex_id);
            provider.set_end_id(curr_vertex_id);
            providers.push_back(provider);
            
            curr_vertex_id++;
            return std::to_string(curr_vertex_id - 1);
        }
    }
    return "no procedure found";
}

ident handleWhile(ident CONDITION_ID, ident COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int comms_id = stoi(COMMANDS_ID);
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    AST::add_edge(providers[cond_id]._begin_id, providers[comms_id]._begin_id, true);
    AST::add_edge(providers[comms_id]._end_id, providers[cond_id]._begin_id);
    AST::add_edge(providers[cond_id]._begin_id, curr_vertex_id, false);

    AST::get_vertex(providers[cond_id]._begin_id).instructions[0]._while_cond = true;
    
    EdgeProvider provider;
    provider._begin_id = providers[cond_id]._begin_id;
    provider._end_id = curr_vertex_id;
    providers.push_back(provider);

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

//TODO: check if this is right
ident handleAssignment(ident IDENTIFIER_ID, ident EXPRESSION_ID) {
    int expr_id = stoi(EXPRESSION_ID);
    AST::get_vertex(providers[expr_id]._begin_id).instructions[0].left = Value(IDENTIFIER_ID);
    AST::get_vertex(providers[expr_id]._begin_id).instructions[0].type_of_instruction = content_type::_ASS;
    return EXPRESSION_ID;
}

void handleMain1(){
    // lt -> iterator to past-the-end object
    if(AST::head_map.empty()) {
        std::cerr << "head_map is empty!";
        return;
    }
    auto lt = AST::head_map.end();
    lt--;
    lt->second = "main";
}

//TODO: add table handling
void handleMain2(ident DECLARATIONS_ID, ident COMMANDS_ID){
    printf("main detected\n");
    ident tmp_decl = "";
    
    handleMain1();
    
    for (auto c : DECLARATIONS_ID) {
        if(c == ',') {
            AST::architecture.assert_var(tmp_decl, "main");
            tmp_decl = "";
        }
        else {
            tmp_decl += c;
        }
    }
    // add var. declaration after last comma separation
    AST::architecture.assert_var(tmp_decl, "main");

}
void handleProcedures1(ident PROCEDURES_ID, ident PROC_HEAD, ident COMMANDS_ID);
void handleProcedures2(ident PROCEDURES_ID, ident PROC_HEAD, ident DECLARATIONS_ID, ident COMMANDS_ID);
