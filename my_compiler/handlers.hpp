#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "definitions.hpp"
#include "types.hpp"

ident handleCondition(ident VAL_ID_1, int OP_TYPE, ident VAL_ID_2);
ident handleRepeat(ident COMMANDS_ID, ident CONDITION_ID);
ident handleIf(ident CONDITION_ID, ident COMMANDS_ID);
ident handleIfElse(ident CONDITION_ID, ident IF_COMMANDS_ID, ident ELSE_COMMANDS_ID);
ident handleWhile(ident CONDITION_ID, ident COMMAND_ID);
ident handleProcCall(ident PROC_CALL_ID);
ident handleAssignment(ident IDENTIFIER_ID, ident EXPRESSION_ID);
ident handleWhile(ident CONDITION_ID, ident COMMANDS_ID);
ident handleCommands(ident COMMANDS_ID, ident COMMAND_ID);

void handleMain1();
void handleMain2(ident DECLARATIONS_ID, ident COMMANDS_ID);
void handleProcedures1(ident PROCEDURES_ID, ident PROC_HEAD, ident COMMANDS_ID);
void handleProcedures2(ident PROCEDURES_ID, ident PROC_HEAD, ident DECLARATIONS_ID, ident COMMANDS_ID);

#define handleExpression handleCondition
#define handleWrite handleCondition
#define handleRead handleCondition

#endif  // HANDLERS_HPP