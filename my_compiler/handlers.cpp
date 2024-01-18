#include "handlers.hpp"
#include "CodeBlock.hpp"
#include "types.hpp"

bool                        head_sig = true;
size_t                      curr_vertex_id = 0;

std::map<int, std::string>  AST::head_map;
Architecture                AST::architecture;
std::vector<int>            AST::head_ids;
std::vector<ptr(CodeBlock)>      AST::vertices;
std::vector<EdgeProvider>   providers;
Address MemoryManager::var_p = 0;

// checking parameters
ptr(Procedure)                   curr_proc = new_ptr(Procedure, "main");

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
    if(curr_proc->variables.count(id) || curr_proc->args.count(id) || curr_proc->tables.count(id)) {
        error("Identifier " + id + " is already in use!", true);
        return true;
    }
    return false;
}


void checkIfInitialized(Value val) {
    if(val.identifier != nullptr) {
        ident pid = val.identifier->pid;
        ident ref_pid = val.identifier->ref_pid;
        switch (val.identifier->type)
        {
        case PID:
            if(curr_proc->variables.count(pid) && !curr_proc->variables[pid]->initialized) {
                error("Variable: " + pid + " not initialized!", true);
            }
            break;
        case TAB_PID:
            if(curr_proc->tables.count(pid) && !curr_proc->variables[ref_pid]->initialized) {
                error("Variable: " + ref_pid + " not initialized!", true);
            }
        }
    }
}

void handleProcedures2(ident PROCEDURES_ID, ident PROC_HEAD, ident DECLARATIONS_ID, ident COMMANDS_ID) {
    head_sig = true;
    logme_handle("to parse: " + PROC_HEAD);

    // AST::architecture.assert_ret_reg(proc_name);
    AST::architecture.add_procedure(curr_proc);

    // procedure_ids[proc_name] = curr_proc;

    auto lt = AST::head_map.end();
    lt--;
    int last = lt->first;
    AST::head_map[last] = curr_proc->name;

    //TODO: \/\/ necessary??
    // AST::architecture.assert_var(proc_name, proc_name);

    curr_proc = new_ptr(Procedure);


    logme_handle("PROCEDURE: definition of [" + curr_proc->name + "]");
    // curr_proc = new_ptr(Procedure);
}

ident handleProcedures1(ident PROCEDURES_ID, ident PROC_HEAD, ident COMMANDS_ID) {
    head_sig = true;
    logme_handle("to parse: " + PROC_HEAD);

    // AST::architecture.assert_ret_reg(proc_name);
    
    // procedure_ids[proc_name] = false;
    AST::architecture.add_procedure(curr_proc);
    
    auto lt = AST::head_map.end();
    lt--;
    int last = lt->first;
    AST::head_map[last] = curr_proc->name;
    
    //TODO: \/\/ necessary??
    // AST::architecture.assert_var(proc_name, proc_name);

    curr_proc = new_ptr(Procedure);

    logme_handle("PROCEDURE: definition of [" + curr_proc->name + "]");
    return curr_proc->name;
}

void handleMain2(ident DECLARATIONS_ID, ident COMMANDS_ID){
    
    handleMain1();
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


    AST::architecture.add_procedure(curr_proc);

    logme_handle("DEFINITION: main");
}

ident handleCommands(ident COMMANDS_ID, ident NEXT_COMMAND_ID) {
    logme_handle("# HANDLE COMMANDS #")
    int comms_id = std::stoi(COMMANDS_ID);
    int next_comm_id = std::stoi(NEXT_COMMAND_ID);

    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;
    
    int next_comm_begin_id = providers[next_comm_id]._begin_id;
    int next_comm_end_id = providers[next_comm_id]._end_id;
    
    AST::add_edge(comms_end_id, next_comm_begin_id, true);

    providers.push_back(EdgeProvider(comms_begin_id, next_comm_end_id));

    logme_handle("# HANDLE COMMANDS END #")

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);          
}

ident handleAssignment(ident IDENTIFIER_ID, ident EXPRESSION_ID) {
    int expr_id = stoi(EXPRESSION_ID);
    Value val = Value(IDENTIFIER_ID);

    switch (val.identifier->type)
    {
        case PID:
            curr_proc->variables[val.identifier->pid]->initialized = true;
            break;
        // case TAB_PID:
        //     if(variable_ids[val.identifier->ref_pid] == 1) {
        //         error("Variable: " + val.identifier->ref_pid + " not initialized!", true);
        //     }
        // break;
    }

    AST::get_vertex(providers[expr_id]._begin_id)->instructions[0].left = val;
    AST::get_vertex(providers[expr_id]._begin_id)->instructions[0].type_of_instruction = content_type::_ASS;

    curr_vertex_id--;
    logme_handle("ASSIGNMENT: " + IDENTIFIER_ID + ":=" + EXPRESSION_ID);
    curr_vertex_id++;
    
    return EXPRESSION_ID;
}

ident handleIfElse(ident CONDITION_ID, ident IF_COMMANDS_ID, ident ELSE_COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int if_comms_id = stoi(IF_COMMANDS_ID);
    int el_comms_id = stoi(ELSE_COMMANDS_ID);

    logme_handle("HANDLE_IF_ELSE");

    AST::add_empty_vertex(curr_vertex_id);

    int cond_begin_id = providers[cond_id]._begin_id;

    int if_comms_begin_id = providers[if_comms_id]._begin_id;
    int if_comms_end_id = providers[if_comms_id]._end_id;
    
    int el_comms_begin_id = providers[el_comms_id]._begin_id;
    int el_comms_end_id = providers[el_comms_id]._end_id;
    
    AST::add_edge(cond_begin_id, if_comms_begin_id, true);
    AST::add_edge(cond_begin_id, el_comms_begin_id, false);
    AST::add_edge(if_comms_end_id, curr_vertex_id, true);
    AST::add_edge(el_comms_end_id, curr_vertex_id, true);

    providers.push_back(EdgeProvider(cond_begin_id, curr_vertex_id));

    logme_handle("HANDLE_IF_ELSE_END");

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);   
}

ident handleIf(ident CONDITION_ID, ident COMMANDS_ID) {
    int cond_id = stoi(CONDITION_ID);
    int comms_id = stoi(COMMANDS_ID);
    
    logme_handle("# HANDLE_IF");
    
    AST::add_empty_vertex(curr_vertex_id);

    int cond_begin_id = providers[cond_id]._begin_id;
    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;
    
    AST::add_edge(cond_begin_id, comms_begin_id, true);
    AST::add_edge(cond_begin_id, curr_vertex_id, false);
    AST::add_edge(comms_end_id, curr_vertex_id, true);
    
    providers.push_back(EdgeProvider(cond_begin_id, curr_vertex_id));

    logme_handle("# HANDLE_IF_END");

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleWhile(ident CONDITION_ID, ident COMMANDS_ID) {
    logme_handle("################# WHILE #################");
    int cond_id = stoi(CONDITION_ID);
    int comms_id = stoi(COMMANDS_ID);
    
    AST::add_empty_vertex(curr_vertex_id);

    int cond_begin_id = providers[cond_id]._begin_id;
    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;

    AST::add_edge(cond_begin_id, comms_begin_id, true);
    AST::add_edge(comms_end_id, cond_begin_id, true);
    AST::add_edge(cond_begin_id, curr_vertex_id, false);

    AST::get_vertex(providers[cond_id]._begin_id)->instructions[0]._while_cond = true;
    
    providers.push_back(EdgeProvider(cond_begin_id, curr_vertex_id));
 
    logme_handle("################# END_WHILE #################");

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleProcHead(ident PROC_NAME, ident ARGS_DECL){
    if (AST::architecture.procedures.count(PROC_NAME)) {
        error("Procedure: " + PROC_NAME + " already exists!", true);
    }

    curr_proc = new_ptr(Procedure);
    curr_proc->name = PROC_NAME;

    ident arg_pid = "";
    bool is_table = false;
    int position = 0;

    for(auto c : ARGS_DECL) {
        if(c == ',') {
            isIdUsed(arg_pid);
            curr_proc->args[arg_pid] = new_ptr(Argument, arg_pid, position, is_table);
            if(is_table) {
                logme_handle("Declare arg_tab: " << arg_pid << " ---> " << PROC_NAME)
            }
            else {
                logme_handle("Declare arg: " << arg_pid << " ---> " << PROC_NAME)
            }
            position++;
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
    isIdUsed(arg_pid);
    curr_proc->args[arg_pid] = new_ptr(Argument, arg_pid, position, is_table);
    if(is_table) {
        logme_handle("Declare arg_tab: " << arg_pid << " ---> " << PROC_NAME)
    }
    else {
        logme_handle("Declare arg: " << arg_pid << " ---> " << PROC_NAME)
    }

    return PROC_NAME;
}

ident handleVDecl(ident PID) {
    isIdUsed(PID);
    logme_handle("Declare var: " + PID);
    curr_proc->variables[PID] = new_ptr(Variable, PID);
    return PID;
}

ident handleTDecl(ident PID, ident num) {
    isIdUsed(PID);
    long long val = std::stoll(num);
    logme_handle("Declare table: " + PID + "[" + num + "]");
    curr_proc->tables[PID] = new_ptr(Table, PID, val);
    return PID;
}

ident handleRepeat(ident COMMANDS_ID, ident CONDITION_ID) {
    logme_handle("################# REPEAT #################");
    
    int comms_id = stoi(COMMANDS_ID);
    int cond_id = stoi(CONDITION_ID);
    
    AST::add_empty_vertex(curr_vertex_id);


    int comms_begin_id = providers[comms_id]._begin_id;
    int comms_end_id = providers[comms_id]._end_id;
    int cond_begin_id = providers[cond_id]._begin_id;
    int cond_end_id = providers[cond_id]._end_id;

    AST::add_edge(comms_end_id, cond_begin_id, true);
    AST::add_edge(cond_end_id, comms_begin_id, false);
    AST::add_edge(cond_end_id, curr_vertex_id, true);
    
    providers.push_back(EdgeProvider(comms_begin_id, curr_vertex_id));

    logme_handle("################# END_REPEAT #################")
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleProcCall(ident PROC_CALL) {
    logme_handle("PROC_CALL: " << PROC_CALL);
    set_head();
    ident proc_id;
    ident tmp_arg;
    Instruction instruction;
    instruction.type_of_instruction = content_type::_CALL;

    //TODO: check if you can optimise this
    //TODO: include variable_tab_ids into argument_ids

    int i = 0;
    for(i = 0; i < PROC_CALL.size(); i++) {
        if(PROC_CALL[i] == '(') {
            if  (tmp_arg == curr_proc->name) {
                error("Inappropriate usage of procedure: " + tmp_arg, true);
            }
            if (!AST::architecture.procedures.count(tmp_arg)) {
                error("Procedure [" + tmp_arg + "] has not been declared", true);
            }
            proc_id += tmp_arg;
        }
        else {
            tmp_arg += PROC_CALL[i];
        }
    }
    
    if(!AST::architecture.procedures.count(proc_id)) {
        error("Undefined procedure: " + proc_id, true);
    }

    instruction.proc_id = proc_id;

    int arg_idx = 0;
    for(i; i < PROC_CALL.size(); i++) {
        if ((PROC_CALL[i] == ',' || PROC_CALL[i] == ')')) {
            if(AST::architecture.procedures[proc_id]->get_arg_at_idx(arg_idx) == nullptr)
            {
                error("Wrong number of arguments for procedure: " + proc_id, true);
            }
            // if(!AST::architecture.procedures[proc_id]->variables.count(tmp_arg) && 
            //     !AST::architecture.procedures[proc_id]->args.count(tmp_arg)) {
            //     error("Undeclared parameter: " + tmp_arg + " in procedure: " + proc_id, true);
            // }
            // if(AST::architecture.procedures[proc_id]->variables.count(tmp_arg) ||
                // AST::architecture.procedures[proc_id]->args.count(tmp_arg)                )
            if(AST::architecture.procedures[proc_id]->get_arg_at_idx(arg_idx)->table != arg_idx) {
                error("Wrong parameters of procedure: " + proc_id, true);
            }
            // if(AST::architecture.procedures[proc_id])
            // if("")
            instruction.args.push_back(Value(tmp_arg));
            arg_idx++;
            tmp_arg = "";
        }
        else {
            tmp_arg += PROC_CALL[i];
        }
    }


    
    AST::add_vertex(curr_vertex_id, instruction);

    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
 
}

ident handleCondition(ident VAL1, ident OP, int INS_TYPE, ident VAL2) {
    set_head();

    Instruction instruction;
    instruction.type_of_instruction = content_type::_COND;
    instruction.type_of_operator = INS_TYPE;
    
    // _WRITE -> no LHS
    instruction.left = Value(VAL1);
    // _NONE -> no RHS
    instruction.right = Value(VAL2);

    checkIfInitialized(instruction.left);
    checkIfInitialized(instruction.right);

    AST::add_vertex(curr_vertex_id, instruction);
    // add instructions to added vertex

    std::string log_msg_head = Instruction::get_ins_log_header(instruction.type_of_instruction);

    // logme_handle(log_msg_head + instruction.left.name + " " + OP + " "+ instruction.right.name);
    
    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleExpression(ident VAL1, ident OP, int INS_TYPE, ident VAL2) {
    set_head();

    Instruction instruction;
    instruction.type_of_instruction = INS_TYPE;
    instruction.type_of_operator = INS_TYPE;

    // _WRITE -> no LHS
    if((INS_TYPE != content_type::_WRITE) &&
        (INS_TYPE != content_type::_READ)) {
        instruction.left = Value(VAL1);
        checkIfInitialized(instruction.left);
    }
    // _NONE -> no RHS
    if((INS_TYPE != operator_type::_NONE)) {        
        instruction.right = Value(VAL2);
        if((INS_TYPE != content_type::_WRITE) &&
            (INS_TYPE != content_type::_READ)) {
            checkIfInitialized(instruction.right);
        }
    }

    if(INS_TYPE == content_type::_READ) {
        if(instruction.right.identifier->type == PID) {
            curr_proc->variables[instruction.right.identifier->pid]->initialized = true;
        }
    }
    AST::add_vertex(curr_vertex_id, instruction);
    // add instructions to added vertex

    std::string log_msg_head = Instruction::get_ins_log_header(instruction.type_of_instruction);

    logme_handle(log_msg_head + VAL1 + " " + OP + " "+ VAL2);
    
    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

void handleIdentifier1(ident PID) {
    if(curr_proc->tables.count(PID) || (curr_proc->args.count(PID) && curr_proc->args[PID]->table)) {
        error("Wrong use of table [" + PID + "]!", true);
    }
    if((!curr_proc->variables.count(PID)) && (!curr_proc->args.count(PID))) {
        error("Variable [" + PID + "] has not been declared!", true);
    }
}

void handleIdentifier2(ident TAB_PID) {
    // if((!variable_tab_))
    if((!curr_proc->tables.count(TAB_PID)) && (!curr_proc->args.count(TAB_PID))) {
        error("Table [" + TAB_PID + "] has not been declared!", true);
    }
}

void handleIdentifier3(ident TAB_PID, ident IDX_PID) {
    handleIdentifier2(TAB_PID);
    handleIdentifier1(IDX_PID);
}