#include "handlers.hpp"
#include "CodeBlock.hpp"
#include "types.hpp"
#include "Logger.hpp"

bool                        head_sig = true;
size_t                      curr_vertex_id = 0;
std::vector<CodeBlock>      AST::vertices;
std::vector<EdgeProvider>   providers;
std::vector<ident>          procedures;
std::map<int, std::string>  AST::head_map;
Architecture                AST::architecture;
std::vector<int>            AST::head_ids;
Logger::Logger logger =     Logger::Logger("logs.log");

void set_head() {
    if (head_sig) {
        AST::head_ids.push_back(curr_vertex_id);
        AST::head_map[curr_vertex_id] = "";
        std::string log_msg = "Codeblock glowa z id:" + std::to_string(curr_vertex_id); 
        logger.LOG(log_msg);
    }
    head_sig = 0;
}

void handleProcedures2(ident PROCEDURES_ID, ident PROC_HEAD, ident DECLARATIONS_ID, ident COMMANDS_ID) {
    ident proc_id = handleProcedures1(PROCEDURES_ID, PROC_HEAD, DECLARATIONS_ID);
    std::string tmp_decl = "";
    for(auto c : DECLARATIONS_ID) {
        if(c == ',') {
            AST::architecture.assert_var(tmp_decl, proc_id);
            logger.LOG("Dodano zmienna: " + tmp_decl + " ---> " + proc_id);
            tmp_decl = "";
        }
        else {
            tmp_decl += c;
        }
    }

    AST::architecture.assert_var(tmp_decl, proc_id);
    logger.LOG("Dodano zmienna: " + tmp_decl + " ---> " + proc_id);
    logger.LOG("PROCEDURE: definition of [[[" + proc_id + "]]]");
}
ident handleProcedures1(ident PROCEDURES_ID, ident PROC_HEAD, ident COMMANDS_ID) {
    head_sig = true;
    logger.LOG("to parse: " + PROC_HEAD);
    int n_args = 0;
    ident tmp_id = "";
    ident proc_id;
    std::vector<ident> tmp_args;

    // extract arguments and procedure identifier
    bool is_procedure = false;
    for(auto c : PROC_HEAD) {
        if(is_procedure) {
            if(c == ',') {
                n_args++;
                tmp_args.push_back(tmp_id);
            }
        }
        else if (c == '(') {
            is_procedure = true;
            proc_id = tmp_id;
        }
        else if (c != ')'){
            tmp_id += c;
        }
    }
    tmp_args.push_back(tmp_id);
    n_args++;
    
    // allocate memory for procedure
    proc_id += "_" + std::to_string(n_args);
    for(auto it : tmp_args) {
        logger.LOG("argument procedury: " + proc_id + " o etykiecie: " + it);
        AST::architecture.assert_arg(it, proc_id);
    }
    AST::architecture.assert_ret_reg(proc_id);
    procedures.push_back(proc_id);
    //TODO: \/\/ necessary??
    AST::architecture.assert_var(proc_id, proc_id);

    auto lt = AST::head_map.end();
    lt--;
    int last = lt->first;
    AST::head_map[last] = proc_id;
    logger.LOG("PROCEDURE: definition of [" + proc_id + "]");
    return proc_id;
}

//TODO: add table handling
void handleMain2(ident DECLARATIONS_ID, ident COMMANDS_ID){
    printf("main detected\n");
    ident tmp_decl = "";
    
    handleMain1();
    
    for (auto c : DECLARATIONS_ID) {
        if(c == ',') {
            AST::architecture.assert_var(tmp_decl, "main");
            logger.LOG("Dodano zmienna: " + tmp_decl + "---> main");
            tmp_decl = "";
        }
        else {
            tmp_decl += c;
        }
    }
    // add var. declaration after last comma separation
    AST::architecture.assert_var(tmp_decl, "main");
    logger.LOG("Dodano zmienna: " + tmp_decl + "---> main");
    logger.LOG("DEFINITION: main");
}

void handleMain1(){
    // lt -> iterator to past-the-end object
    if(AST::head_map.empty()) {
        std::cerr << "head_map is empty!";
        return;
    }
    auto lt = AST::head_map.end();
    lt--;
    int index = lt->first;
    AST::head_map[index] = "main";
}

ident handleCommands(ident COMMANDS_ID, ident NEXT_COMMAND_ID) {
    int comms_id = std::stoi(COMMANDS_ID);
    int next_comm_id = std::stoi(NEXT_COMMAND_ID);

    int comms_end_id = providers[comms_id]._end_id;
    int next_comm_begin_id = providers[next_comm_id]._begin_id;
    
    AST::add_edge(comms_end_id, next_comm_begin_id);

    EdgeProvider provider;
    provider._begin_id = providers[comms_id]._begin_id;
    provider._end_id = providers[next_comm_id]._end_id;
    providers.push_back(provider);
 
    //LOGS
    std::string log_msg = "dodano krawedz: true(" + std::to_string(comms_end_id) + ", " + std::to_string(next_comm_begin_id) + ")";
    logger.LOG(log_msg);

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);          
}

//TODO: check if this is right
ident handleAssignment(ident IDENTIFIER_ID, ident EXPRESSION_ID) {
    // TODO: delete set_head?
    set_head();
    
    int expr_id = stoi(EXPRESSION_ID);
    AST::get_vertex(providers[expr_id]._begin_id).instructions[0].left = Value(IDENTIFIER_ID);
    AST::get_vertex(providers[expr_id]._begin_id).instructions[0].type_of_instruction = content_type::_ASS;
    logger.LOG("ASSIGNMENT: " + IDENTIFIER_ID + ":=" + EXPRESSION_ID);
    
    return EXPRESSION_ID;
}

ident handleIfElse(ident CONDITION_ID, ident IF_COMMANDS_ID, ident ELSE_COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int if_comms_id = stoi(IF_COMMANDS_ID);
    int el_comms_id = stoi(ELSE_COMMANDS_ID);

    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    int cond_begin_id = providers[cond_id]._begin_id;

    int if_comms_begin_id = providers[if_comms_id]._begin_id;
    int if_comms_end_id = providers[if_comms_id]._end_id;
    
    int el_comms_begin_id = providers[el_comms_id]._begin_id;
    int el_comms_end_id = providers[el_comms_id]._end_id;
    
    //TODO: this may be problematic
    AST::add_edge(cond_begin_id, if_comms_begin_id, true);
    AST::add_edge(cond_begin_id, el_comms_begin_id, false);
    AST::add_edge(if_comms_end_id, curr_vertex_id);
    AST::add_edge(el_comms_end_id, curr_vertex_id);

    EdgeProvider provider;
    provider._begin_id = providers[cond_id]._begin_id;
    provider._end_id = curr_vertex_id;
    providers.push_back(provider);

    // LOGS
    std::string log_msg = "################# IF_ELSE #################\n";
    log_msg += "dodano krawedz: true(" + std::to_string(cond_begin_id) + ", " + std::to_string(if_comms_begin_id) + ")\n";
    log_msg +=  "dodano krawedz: false(" + std::to_string(cond_begin_id) + ", " + std::to_string(el_comms_begin_id) + ")\n";
    log_msg +=  "dodano krawedz: true(" + std::to_string(if_comms_end_id) + ", " + std::to_string(curr_vertex_id) + ")\n";
    log_msg +=  "dodano krawedz: true(" + std::to_string(el_comms_end_id) + ", " + std::to_string(curr_vertex_id) + ")";
    logger.LOG(log_msg);

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);   
}

ident handleIf(ident CONDITION_ID, ident COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int comms_id = stoi(COMMANDS_ID);
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    int cond_begin_id = providers[cond_id]._begin_id;
    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;
    
    AST::add_edge(cond_begin_id, comms_begin_id, true);
    AST::add_edge(cond_begin_id, curr_vertex_id, false);
    AST::add_edge(comms_end_id, curr_vertex_id);
    
    EdgeProvider provider;
    provider._begin_id = providers[cond_id]._begin_id;
    provider._end_id = curr_vertex_id;
    providers.push_back(provider);

    // LOGS
    std::string log_msg = "################# IF #################\n";
    log_msg += "dodano krawedz: true(" + std::to_string(cond_begin_id) + ", " + std::to_string(comms_begin_id) + ")\n";
    log_msg +=  "dodano krawedz: false(" + std::to_string(cond_begin_id) + ", " + std::to_string(curr_vertex_id) + ")\n";
    log_msg +=  "dodano krawedz: true(" + std::to_string(comms_end_id) + ", " + std::to_string(curr_vertex_id) + ")\n";
    logger.LOG(log_msg);

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleWhile(ident CONDITION_ID, ident COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int comms_id = stoi(COMMANDS_ID);
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    int cond_begin_id = providers[cond_id]._begin_id;
    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;

    AST::add_edge(cond_begin_id, comms_begin_id, true);
    AST::add_edge(comms_end_id, cond_begin_id);
    AST::add_edge(cond_begin_id, curr_vertex_id, false);

    AST::get_vertex(providers[cond_id]._begin_id).instructions[0]._while_cond = true;
    
    EdgeProvider provider;
    provider._begin_id = providers[cond_id]._begin_id;
    provider._end_id = curr_vertex_id;
    providers.push_back(provider);
    
    // LOGS
    std::string log_msg = "################# WHILE #################\n";
    log_msg += "dodano krawedz: true(" + std::to_string(cond_begin_id) + ", " + std::to_string(comms_begin_id) + ")\n";
    log_msg +=  "dodano krawedz: true(" + std::to_string(comms_end_id) + ", " + std::to_string(cond_begin_id) + ")\n";
    log_msg +=  "dodano krawedz: false(" + std::to_string(cond_begin_id) + ", " + std::to_string(curr_vertex_id) + ")\n";
    logger.LOG(log_msg);
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleRepeat(ident COMMANDS_ID, ident CONDITION_ID) {
    int comms_id = stoi(COMMANDS_ID);
    int cond_id = stoi(CONDITION_ID);
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    int cond_begin_id = providers[cond_id]._begin_id;
    int cond_end_id = providers[cond_id]._end_id;
    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;

    AST::add_edge(comms_end_id, cond_begin_id);
    AST::add_edge(cond_end_id, comms_begin_id, false);
    AST::add_edge(comms_end_id, curr_vertex_id, true);
    
    std::string log_msg = "################# REPEAT #################\n";
    log_msg += "dodano krawedz: true(" + std::to_string(comms_end_id) + ", " + std::to_string(cond_begin_id) + ")\n";
    log_msg +=  "dodano krawedz: false(" + std::to_string(cond_end_id) + ", " + std::to_string(comms_begin_id) + ")\n";
    log_msg +=  "dodano krawedz: true(" + std::to_string(comms_end_id) + ", " + std::to_string(curr_vertex_id) + ")\n";

    EdgeProvider provider;
    provider._begin_id = providers[cond_id]._begin_id;
    providers.push_back(provider);
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleProcCall(ident PROC_CALL) {
    set_head();
    ident proc_id;
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
            proc_id += c;
        }
    }

    proc_id += '_' + std::to_string(instruction.args.size());
    
    //TODO: you can optimize this
    for (auto p : procedures) {
        if(p == proc_id) { 
            instruction.proc_id = proc_id;
            logger.LOG("Znaleziono procedure: " + proc_id);
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
    logger.LOG("Nie znaleziono procedury: " + proc_id);
    return "no procedure found";
}

ident handleCondition(ident VAL1, ident OP, int INS_TYPE, ident VAL2) {
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

    std::string log_msg_head = Instruction::get_ins_log_header(instruction.type_of_instruction);

    //TODO: expand logging here
    logger.LOG(log_msg_head + instruction.left.name + OP + instruction.right.name + " ----> "
         + std::to_string(curr_vertex_id));
    
    EdgeProvider provider;
    provider.set_begin_id(curr_vertex_id);
    provider.set_end_id(curr_vertex_id);
    providers.push_back(provider);
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}