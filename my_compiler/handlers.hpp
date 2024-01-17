#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "definitions.hpp"
#include "types.hpp"

void handleProcedures2(ident PROCEDURES_ID, ident PROC_HEAD, ident DECLARATIONS_ID, ident COMMANDS_ID);
ident handleProcedures1(ident PROCEDURES_ID, ident PROC_HEAD, ident COMMANDS_ID);

void handleMain2(ident DECLARATIONS_ID, ident COMMANDS_ID);
void handleMain1();

ident handleCommands(ident COMMANDS_ID, ident NEXT_COMMAND_ID);

ident handleProcHead(ident PROC_NAME, ident ARGS_DECL);

ident handleVDecl(ident PID);
ident handleTDecl(ident PID, ident size);

ident handleAssignment(ident IDENTIFIER_ID, ident EXPRESSION_ID);
ident handleIfElse(ident CONDITION_ID, ident IF_COMMANDS_ID, ident ELSE_COMMANDS_ID);
ident handleIf(ident CONDITION_ID, ident COMMANDS_ID);
ident handleWhile(ident CONDITION_ID, ident COMMAND_ID);
ident handleRepeat(ident COMMANDS_ID, ident CONDITION_ID);
ident handleProcCall(ident PROC_CALL_ID);
#define handleRead handleExpression
#define handleWrite handleExpression

ident handleExpression(ident VAL_ID_1, ident OP, int INS_TYPE, ident VAL_ID_2);
ident handleCondition(ident VAL_ID_1, ident OP, int INS_TYPE, ident VAL_ID_2);
void handleIdentifier1(ident PID);
void handleIdentifier2(ident TAB_PID);
void handleIdentifier3(ident TAB_PID, ident IDX_PID);


#endif  // HANDLERS_HPP