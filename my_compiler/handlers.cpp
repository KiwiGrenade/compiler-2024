#include "handlers.hpp"
#include "CodeBlock.hpp"
#include "types.hpp"

bool                        head_sig = true;
size_t                      curr_vertex_id = 0;

std::map<int, std::string>  AST::head_map;
Architecture                AST::architecture;
std::vector<int>            AST::head_ids;
std::vector<CodeBlock>      AST::vertices;

std::vector<EdgeProvider>   providers;

// checking parameters
std::map<ident, bool>       procedures;
std::map<ident, bool>       variables;
std::map<ident, bool>       arguments;
std::map<ident, bool>       arguments_tab;
std::map<ident, int>        variables_tab;
ident                       proc_name;

std::string log_head = "H"; // for handlers

#define logme_handle(str) { logme("[" << curr_vertex_id << "] " << str, log_head)}

void set_head() {
    if (head_sig) {
        AST::head_ids.push_back(curr_vertex_id);
        AST::head_map[curr_vertex_id] = "";
        std::string log_msg = "Codeblock glowa z id:" + std::to_string(curr_vertex_id); 
        logme_handle(log_msg);
    }
    head_sig = 0;
}

bool isIdUsed(ident id) {
    if(variables.count(id) || arguments.count(id) || procedures.count(id) || arguments_tab.count(id) || variables_tab.count(id)) {
        error("Identifier " + id + " is already in use!", false);
        return true;
    }
    return false;
}

void handleProcedures2(ident PROCEDURES_ID, ident PROC_HEAD, ident DECLARATIONS_ID, ident COMMANDS_ID) {
    head_sig = true;
    logme_handle("to parse: " + PROC_HEAD);

    for(auto arg : arguments) {
        AST::architecture.assert_arg(arg.first, proc_name);
    }
    
    for(auto arg_tab : arguments_tab) {
        AST::architecture.assert_arg_T(arg_tab.first, proc_name);
    }

    for(auto var : variables)  {
        AST::architecture.assert_var(var.first, proc_name);
    }

    for(auto var_tab : variables_tab) {
        AST::architecture.assert_var_T(var_tab.first, var_tab.second, proc_name);
    }

    AST::architecture.assert_ret_reg(proc_name);

    procedures[proc_name] = false;

    auto lt = AST::head_map.end();
    lt--;
    int last = lt->first;
    AST::head_map[last] = proc_name;

    //TODO: \/\/ necessary??
    AST::architecture.assert_var(proc_name, proc_name);

    variables.clear();
    variables_tab.clear();
    arguments.clear();
    arguments_tab.clear();

    logme_handle("PROCEDURE: definition of [" + proc_name + "]");
}

ident handleProcedures1(ident PROCEDURES_ID, ident PROC_HEAD, ident COMMANDS_ID) {
    head_sig = true;
    logme_handle("to parse: " + PROC_HEAD);

    for(auto arg : arguments) {
        AST::architecture.assert_arg(arg.first, proc_name);
    }
    
    for(auto arg_tab : arguments_tab) {
        AST::architecture.assert_arg_T(arg_tab.first, proc_name);
    }

    AST::architecture.assert_ret_reg(proc_name);
    
    procedures[proc_name] = false;
    
    
    auto lt = AST::head_map.end();
    lt--;
    int last = lt->first;
    AST::head_map[last] = proc_name;
    
    //TODO: \/\/ necessary??
    AST::architecture.assert_var(proc_name, proc_name);

    
    logme_handle("PROCEDURE: definition of [" + proc_name + "]");
    return proc_name;
}

void handleMain2(ident DECLARATIONS_ID, ident COMMANDS_ID){
    printf("main detected\n");
    proc_name = "main";
    
    handleMain1();
    
    for(auto var : variables)  {
        AST::architecture.assert_var(var.first, proc_name);
    }

    for(auto var_tab : variables_tab) {
        AST::architecture.assert_var_T(var_tab.first, var_tab.second, proc_name);
    }
    logme_handle("DEFINITION: main");
}

void handleMain1(){
    // lt -> iterator to past-the-end object
    if(AST::head_map.empty()) {
        error("head_map is empty!", true);
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
    int comms_begin_id = providers[comms_id]._begin_id;
    
    int next_comm_begin_id = providers[next_comm_id]._begin_id;
    int next_comm_end_id = providers[next_comm_id]._end_id;
    
    AST::add_edge(comms_end_id, next_comm_begin_id, true);

    providers.push_back(EdgeProvider(comms_begin_id, next_comm_end_id));

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);          
}

//TODO: check if this is right
ident handleAssignment(ident IDENTIFIER_ID, ident EXPRESSION_ID) {
    
    int expr_id = stoi(EXPRESSION_ID);
    AST::get_vertex(providers[expr_id]._begin_id).instructions[0].left = Value(IDENTIFIER_ID);
    AST::get_vertex(providers[expr_id]._begin_id).instructions[0].type_of_instruction = content_type::_ASS;
    logme_handle("ASSIGNMENT: " + IDENTIFIER_ID + ":=" + EXPRESSION_ID);
    
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
    AST::add_edge(if_comms_end_id, curr_vertex_id, true);
    AST::add_edge(el_comms_end_id, curr_vertex_id, true);

    providers.push_back(EdgeProvider(cond_begin_id, curr_vertex_id));


    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);   
}

ident handleIf(ident CONDITION_ID, ident COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int comms_id = stoi(COMMANDS_ID);
    
    logme_handle("################# IF #################");
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    int cond_begin_id = providers[cond_id]._begin_id;
    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;
    
    AST::add_edge(cond_begin_id, comms_begin_id, true);
    AST::add_edge(cond_begin_id, curr_vertex_id, false);
    AST::add_edge(comms_end_id, curr_vertex_id, true);
    
    providers.push_back(EdgeProvider(cond_begin_id, curr_vertex_id));

    logme_handle("################# IF #################");

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleWhile(ident CONDITION_ID, ident COMMANDS_ID) {
    logme_handle("################# WHILE #################");
    int cond_id = stoi(CONDITION_ID);
    int comms_id = stoi(COMMANDS_ID);
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    int cond_begin_id = providers[cond_id]._begin_id;
    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;

    AST::add_edge(cond_begin_id, comms_begin_id, true);
    AST::add_edge(comms_end_id, cond_begin_id, true);
    AST::add_edge(cond_begin_id, curr_vertex_id, false);

    AST::get_vertex(providers[cond_id]._begin_id).instructions[0]._while_cond = true;
    
    providers.push_back(EdgeProvider(cond_begin_id, curr_vertex_id));
 
    logme_handle("################# END_WHILE #################");

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

// ident handleDeclarations()

ident handleProcHead(ident PROC_NAME, ident ARGS_DECL){
    isIdUsed(PROC_NAME);

    arguments.clear();

    proc_name = PROC_NAME;

    ident arg_pid = "";
    bool is_table = false;

    std::cout << ARGS_DECL << std::endl;

    for(auto c : ARGS_DECL) {
        if(c == ',') {
            if(!isIdUsed(arg_pid)) {
                if(is_table) {
                    arguments_tab[arg_pid] = false;
                    logme_handle("Declare arg_tab: " << arg_pid << " ---> " << PROC_NAME)
                }
                else {
                    arguments[arg_pid] = false;
                    logme_handle("Declare arg: " << arg_pid << " ---> " << PROC_NAME)
                }
            }
            arg_pid = "";
            is_table = false;
        }
        else if(c == 'T') {
            is_table = true;
        }
        else {
            arg_pid += c;
        }
    }
    if(!isIdUsed(arg_pid)) {
        if(is_table) {
            arguments_tab[arg_pid] = false;
            logme_handle("Declare arg_tab: " << arg_pid << " ---> " << PROC_NAME)
        }
        else {
            arguments[arg_pid] = false;
            logme_handle("Declare arg: " << arg_pid << " ---> " << PROC_NAME)
        }
    }

    return PROC_NAME;
}

ident handleVDecl(ident PID) {
    isIdUsed(PID);
    logme_handle("Declare var: " + PID);
    variables[PID] = false;
    return PID;
}

ident handleTDecl(ident PID, ident num) {
    isIdUsed(PID);
    logme_handle("Declare table: " + PID + "[" + num + "]")
    variables_tab[PID] = stoi(num);
    return PID;
}

ident handleRepeat(ident COMMANDS_ID, ident CONDITION_ID) {
    logme_handle("################# REPEAT #################");
    
    int comms_id = stoi(COMMANDS_ID);
    int cond_id = stoi(CONDITION_ID);
    
    AST::add_vertex(curr_vertex_id);
    AST::vertices[AST::vertices.size() - 1].empty = 1;

    int cond_begin_id = providers[cond_id]._begin_id;
    int cond_end_id = providers[cond_id]._end_id;
    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;

    AST::add_edge(comms_end_id, cond_begin_id, true);
    AST::add_edge(cond_end_id, comms_begin_id, false);
    AST::add_edge(comms_end_id, curr_vertex_id, true);
    
    providers.push_back(EdgeProvider(cond_begin_id, curr_vertex_id));

    logme_handle("################# END_REPEAT #################")
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
    //TODO: include variables_tab into arguments
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

    // proc_id += '_' + std::to_string(instruction.args.size());
    
    //TODO: you can optimize this
    if (procedures.count(proc_id)) {
        instruction.proc_id = proc_id;
        logme_handle("################# PROC_CALL: " << proc_id << " #################")
        AST::add_vertex(curr_vertex_id);
        AST::vertices[AST::vertices.size() - 1].instructions.push_back(instruction);
        
        providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));
        
        logme_handle("################# END_PROC_CALL: " << proc_id << " #################")
        curr_vertex_id++;
        return std::to_string(curr_vertex_id - 1);
    }
    logme_handle("Nie znaleziono procedury: " + proc_id);
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

    logme_handle(log_msg_head + instruction.left.name + " " + OP + " "+ instruction.right.name);
    
    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}
