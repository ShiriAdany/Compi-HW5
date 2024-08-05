#pragma once

#include <string>
#include "cg.hpp"
#include "types.hpp"

// Forward declarations
class Exp;
class Operator;

extern long v;
extern CodeBuffer& cb;
std::string freshVar();

void generateMainStart();
void generateMainEnd();
std::string relopCommand(Exp* left, Exp* right, Operator* o);

