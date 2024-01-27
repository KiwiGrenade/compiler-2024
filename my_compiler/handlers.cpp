#include "handlers.hpp"
#include "CodeBlock.hpp"
#include "types.hpp"

bool                        head_sig = true;
size_t                      curr_vertex_id = 0;

std::map<long long, std::string>  AST::head_map;
Architecture                AST::architecture;
std::vector<long long>            AST::head_ids;
std::vector<ptr(CodeBlock)>      AST::vertices;
std::vector<EdgeProvider>   providers;
Address MemoryManager::var_p = 0;

extern long long loop_depth;

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
    if(curr_proc->isVar(id) || curr_proc->isArg(id) || curr_proc->isTab(id)) {
        error("Identifier " + id + " is already in use!", true);
        return true;
    }
    return false;
}

void checkIfInitialized(ptr(Value) val) {
    if(val->identifier != nullptr) {
        ident pid = val->identifier->pid;
        ident ref_pid = val->identifier->ref_pid;
        switch (val->identifier->type)
        {
        case PID:
            if(curr_proc->isVar(pid)) {
                if(curr_proc->get_var(pid)->initialized == false) {
                    if(loop_depth == 0) {
                        error("Variable: " + pid + " not initialized!", true);
                    }
                    else {
                        warning("Variable: " + pid + " may be used before set!");
                    }
                }
                curr_proc->get_var(pid)->used = true;
            }
            break;
        case TAB_PID:
            if(curr_proc->isTab(pid) && curr_proc->isVar(ref_pid) && curr_proc->get_var(ref_pid)->initialized == false) {
                error("Variable: " + ref_pid + " not initialized!", true);
            }
        }
    }
}

void markInitialized(ptr(Value) val) {
    switch (val->identifier->type)
    {
        case PID:
            if(curr_proc->isVar(val->identifier->pid)) {
                curr_proc->get_var(val->identifier->pid)->initialized = true;
            }
            else {
                curr_proc->get_arg(val->identifier->pid)->initialized = true;
            }
            break;
        // case TAB_NUM:
        //     if(variable_ids[val.identifier->ref_pid] == 1) {
        //         error("Variable: " + val.identifier->ref_pid + " not initialized!", true);
        //     }
        // break;
    }
}


ident extract_proc_name(ident PROC_CALL) {
    return PROC_CALL.substr(0, PROC_CALL.find_first_of('('));
}

void checkProcName(ident name) {
    if(name == curr_proc->get_name()) {
        error("Inappropriate usage of procedure: " + name, true);
    }
    //TODO: CHANGE THIS
    if (!AST::architecture.procedures.count(name)) {
        error("Procedure [" + name + "] has not been declared", true);
    }
}


// TODO: add checking for params
void extract_proc_params(std::shared_ptr<std::vector<ptr(Value)>> args, ident params) {
    
    ident tmp_arg = "";
    for (auto c : params) {
        if(c == ',' || c == ')') {
            if(curr_proc->isArg(tmp_arg)) {
                (*args).push_back(new_ptr(Value, tmp_arg));
                // args.push_back(curr_proc->get_arg(tmp_arg));
            }
            else if(curr_proc->isVar(tmp_arg)) {
                (*args).push_back(new_ptr(Value, tmp_arg));
                // args.push_back(curr_proc->get_var(tmp_arg));
            }
            else if(curr_proc->isTab(tmp_arg)) {
                (*args).push_back(new_ptr(Value, tmp_arg));
                // args.push_back(curr_proc->get_tab(tmp_arg));
            }
            else {
                error("Can't find: " + tmp_arg, true);
            }
            tmp_arg = "";
        }
        else {
            tmp_arg += c;
        }
    }
    // return args;
}

void checkSequence(std::shared_ptr<std::vector<ptr(Value)>> args, ident proc_id) {
    for(long long i = 0; i < (*args).size(); i++) {
        ptr(Argument) call_arg = AST::architecture.procedures[proc_id]->get_arg_at_idx(i);
        ident param_pid = (*args)[i]->identifier->pid;

        if(call_arg->table) {
            if(curr_proc->isTab(param_pid) == false) {
                error("Wrong parameters in procedure: " + proc_id, true);
            }
            else if(curr_proc->isArg(param_pid) && (curr_proc->get_arg(param_pid)->table == false)) {
                error("Wrong parameters in procedure: " + proc_id, true);
            }
        }
        else {
            if(curr_proc->isTab(param_pid)) {
                error("Wrong parameters in procedure: " + proc_id, true);
            }
            if(curr_proc->isArg(param_pid) && (curr_proc->get_arg(param_pid)->table == true)) {
                error("Wrong parameters in procedure: " + proc_id, true);
            }
        }
    }
}

void markParamsInitialized(std::shared_ptr<std::vector<ptr(Value)>> args, ident proc_id) {
    for(long long i = 0; i < (*args).size(); i++) {
        ptr(Argument) call_arg = AST::architecture.procedures[proc_id]->get_arg_at_idx(i);
        ident param_pid = (*args)[i]->identifier->pid;
        if(curr_proc->isArg(param_pid) && !curr_proc->get_arg(param_pid)->table && !curr_proc->get_arg(param_pid)->initialized) {
            curr_proc->get_arg(param_pid)->initialized = true;        
        }
        else if(curr_proc->isVar(param_pid) && !curr_proc->get_var(param_pid)->initialized){
            curr_proc->get_var(param_pid)->initialized = true;
        }
    }
}

void checkSetAndUsed() {
    if(loop_depth == 0) {
        curr_proc->checkIfArgsSetAndUsed();
        curr_proc->checkIfVarsSetAndUsed();
    }
}

void handleProcedures() {
    head_sig = true;

    // AST::architecture.assert_ret_reg(proc_name);
    AST::architecture.add_procedure(curr_proc);

    auto lt = AST::head_map.end();
    lt--;
    long long last = lt->first;
    AST::head_map[last] = curr_proc->get_name();

    curr_proc = new_ptr(Procedure, "main");

    logme_handle("PROCEDURE: definition of [" + curr_proc->get_name() + "]");
}

void handleMain(){
    // lt -> iterator to past-the-end object
    auto lt = AST::head_map.end();
    lt--;
    long long index = lt->first;
    AST::head_map[index] = "main";

    AST::architecture.add_procedure(curr_proc);

    logme_handle("DEFINITION: main");
}

ident handleCommands(ident COMMANDS_ID, ident NEXT_COMMAND_ID) {
    long long comms_id = std::stoll(COMMANDS_ID);
    long long next_comm_id = std::stoll(NEXT_COMMAND_ID);

    long long comms_begin_id = providers[comms_id]._begin_id;
    long long comms_end_id = providers[comms_id]._end_id;
    
    long long next_comm_begin_id = providers[next_comm_id]._begin_id;
    long long next_comm_end_id = providers[next_comm_id]._end_id;
    
    AST::add_edge(comms_end_id, next_comm_begin_id, true);

    providers.push_back(EdgeProvider(comms_begin_id, next_comm_end_id));

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);          
}

ident handleAssignment(ident IDENTIFIER_ID, ident EXPRESSION_ID) {
    long long expr_id = stoll(EXPRESSION_ID);
    ptr(Value) val = new_ptr(Value, IDENTIFIER_ID);

    markInitialized(val);

    AST::get_vertex(providers[expr_id]._begin_id)->instructions[0].lvalue = val;
    AST::get_vertex(providers[expr_id]._begin_id)->instructions[0].type_of_instruction = content_type::_ASS;

    curr_vertex_id--;
    logme_handle("ASSIGNMENT: " + IDENTIFIER_ID + ":=" + EXPRESSION_ID);
    curr_vertex_id++;
    
    return EXPRESSION_ID;
}

ident handleIfElse(ident CONDITION_ID, ident IF_COMMANDS_ID, ident ELSE_COMMANDS_ID) {
    long long cond_id = stoll(CONDITION_ID);
    long long if_comms_id = stoll(IF_COMMANDS_ID);
    long long el_comms_id = stoll(ELSE_COMMANDS_ID);

    logme_handle("HANDLE_IF_ELSE");

    AST::add_empty_vertex(curr_vertex_id);

    long long cond_begin_id = providers[cond_id]._begin_id;

    long long if_comms_begin_id = providers[if_comms_id]._begin_id;
    long long if_comms_end_id = providers[if_comms_id]._end_id;
    
    long long el_comms_begin_id = providers[el_comms_id]._begin_id;
    long long el_comms_end_id = providers[el_comms_id]._end_id;
    
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
    long long cond_id = stoll(CONDITION_ID);
    long long comms_id = stoll(COMMANDS_ID);
    
    logme_handle("# HANDLE_IF");
    
    AST::add_empty_vertex(curr_vertex_id);

    long long cond_begin_id = providers[cond_id]._begin_id;
    long long comms_begin_id = providers[comms_id]._begin_id;
    long long comms_end_id = providers[comms_id]._end_id;
    
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

    checkSetAndUsed();

    long long cond_id = stoll(CONDITION_ID);
    long long comms_id = stoll(COMMANDS_ID);
    
    AST::add_empty_vertex(curr_vertex_id);

    long long cond_begin_id = providers[cond_id]._begin_id;
    long long comms_begin_id = providers[comms_id]._begin_id;
    long long comms_end_id = providers[comms_id]._end_id;

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
    curr_proc->set_name(PROC_NAME);

    ident arg_pid = "";
    bool is_table = false;
    long long position = 0;

    ARGS_DECL+=',';

    for(auto c : ARGS_DECL) {
        if(c == ',') {
            isIdUsed(arg_pid);
            curr_proc->add_arg(new_ptr(Argument, arg_pid, position, is_table));
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

    return PROC_NAME;
}

ident handleVDecl(ident PID) {
    isIdUsed(PID);
    logme_handle("Declare var: " + PID);
    curr_proc->add_var(new_ptr(Variable, PID));
    return PID;
}

ident handleTDecl(ident PID, ident num) {
    isIdUsed(PID);
    long long val = std::stoll(num);
    logme_handle("Declare table: " + PID + "[" + num + "]");
    curr_proc->add_tab(new_ptr(Table, PID, val));
    return PID;
}

ident handleRepeat(ident COMMANDS_ID, ident CONDITION_ID) {
    logme_handle("################# REPEAT #################");

    checkSetAndUsed();
    
    long long comms_id = stoll(COMMANDS_ID);
    long long cond_id = stoll(CONDITION_ID);
    
    AST::add_empty_vertex(curr_vertex_id);


    long long comms_begin_id = providers[comms_id]._begin_id;
    long long comms_end_id = providers[comms_id]._end_id;
    long long cond_begin_id = providers[cond_id]._begin_id;
    long long cond_end_id = providers[cond_id]._end_id;

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
    std::shared_ptr<std::vector<ptr(Value)>> params = std::make_shared<std::vector<ptr(Value)>>(); 
    Instruction instruction;
    instruction.type_of_instruction = content_type::_CALL;

    proc_id = extract_proc_name(PROC_CALL);
    checkProcName(proc_id);
    instruction.proc_id = proc_id;

    ident params_id = PROC_CALL.substr(proc_id.size()+1);
    
    extract_proc_params(params, params_id);

    checkSequence(params, proc_id);

    markParamsInitialized(params, proc_id);

    instruction.args = (*params);
    
    AST::add_vertex(curr_vertex_id, instruction);

    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));

    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
 
}

ident handleCondition(ident VAL1, ident OP, long long OP_TYPE, ident VAL2) {
    set_head();

    Instruction instruction;
    instruction.type_of_instruction = content_type::_COND;

    instruction.expr = new_ptr(Expression, new_ptr(Value, VAL1), OP_TYPE, new_ptr(Value, VAL2));

    checkIfInitialized(instruction.expr->left);
    checkIfInitialized(instruction.expr->right);

    AST::add_vertex(curr_vertex_id, instruction);

    logme_handle("CONDITION:" + VAL1 + " " + OP + " "+ VAL2);
    
    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleExpression(ident VAL1, ident OP, long long OP_TYPE, ident VAL2) {
    set_head();

    ptr(Value) left = new_ptr(Value, VAL1);
    ptr(Value) right;

    Instruction instruction;

    checkIfInitialized(left);
    if(OP_TYPE != operator_type::_NONE) {
        right = new_ptr(Value, VAL2);
        checkIfInitialized(right);
    }

    instruction.expr = new_ptr(Expression, left, OP_TYPE, right);

    AST::add_vertex(curr_vertex_id, instruction);

    logme_handle("Expression: " + VAL1 + " " + OP + " "+ VAL2);
    
    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleRead(ident VAL2) {
    set_head();

    Instruction instruction;
    instruction.type_of_instruction = content_type::_READ;
    instruction.expr = new_ptr(Expression, nullptr, operator_type::_NONE, new_ptr(Value, VAL2));

    if(instruction.expr->right->identifier->type == PID && curr_proc->isVar(VAL2)) {
        curr_proc->get_var(VAL2)->initialized = true;
    }
    
    AST::add_vertex(curr_vertex_id, instruction);

    logme_handle("READ:" + VAL2);

    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

ident handleWrite(ident VAL2) {
    set_head();

    Instruction instruction;
    instruction.type_of_instruction = content_type::_WRITE;
    instruction.expr = new_ptr(Expression, nullptr, operator_type::_NONE, new_ptr(Value, VAL2));

    checkIfInitialized(instruction.expr->right);

    AST::add_vertex(curr_vertex_id, instruction);

    logme_handle("WRITE:" + VAL2);
    
    providers.push_back(EdgeProvider(curr_vertex_id, curr_vertex_id));
    
    curr_vertex_id++;
    return std::to_string(curr_vertex_id - 1);
}

void handleIdentifier1(ident PID) {
    if(curr_proc->isTab(PID) || (curr_proc->isArg(PID) && curr_proc->get_arg(PID)->table)) {
        error("Wrong use of table [" + PID + "]!", true);
    }
    if((curr_proc->isVar(PID) == false) && (curr_proc->isArg(PID) == false)) {
        error("Variable [" + PID + "] has not been declared!", true);
    }
}

void handleIdentifier2(ident TAB_PID) {
    // if((!variable_tab_))
    if((curr_proc->isTab(TAB_PID) == false) && (curr_proc->isArg(TAB_PID) == false)) {
        error("Table [" + TAB_PID + "] has not been declared!", true);
    }
}

void handleIdentifier3(ident TAB_PID, ident IDX_PID) {
    handleIdentifier2(TAB_PID);
    handleIdentifier1(IDX_PID);
}