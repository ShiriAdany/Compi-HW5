#include "generator.hpp"

long v = 0;

std::string freshVar() {
    std::string res = "a" + std::to_string(v);
    v++;
    return res;
}

void generateMainStart() {
    cb.emit("define i32 @main(){\n");
    cb.emit("declare i32 @printf(i8*, ...)\ndeclare void @exit(i32)\ndeclare i32 @scanf(i8*, ...)");
    cb.emit("@.divZero = internal constant [23 x i8] c\"Error division by zero\\00\"");
}

void generateMainEnd() {
    cb.emit("ret i32 0\n}");
}

std::string relopCommand(Exp* left, Exp* right, Operator* o)
{
    std::string target = freshVar();
    std::string type = Exp::resultType(left,right);
    std::string relop = type == "INT" ?  "s" : "u";

    if(o->op == "<") relop += "lt";
    else if(o->op == "<=") relop += "le";
    else if (o->op == ">") relop += "gt";
    else relop += "ge";

    cb.emit("%" + target + " = icmp " + relop + " i32 %" + left->var + " %" + right->var);
    return target;

}