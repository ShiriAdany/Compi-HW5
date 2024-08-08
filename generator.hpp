#pragma once

#include <string>
#include "cg.hpp"
#include "types.hpp"

// Forward declarations
class Exp;
class Operator;
class Num;

extern long v;
extern CodeBuffer& cb;
std::string freshVar();

void generateMainStart(const std::string& sp);
void generateMainEnd();
void opCommand(Exp* e, Exp* left, Exp* right, Operator* o);
void relopCommand(Exp* e, Exp* left, Exp* right, Operator* o);
void releqCommand(Exp* e, Exp* left, Exp* right, Operator* o);
void trueCommand(Exp* e);
void falseCommand(Exp* e);
void orCommand(Exp* e, Exp* left, Exp* right);
void andCommand(Exp* e, Exp* left, Exp* right);
void notCommand(Exp* e);
void numCommand(Exp* e, Num* num);
