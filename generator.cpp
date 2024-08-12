#include "generator.hpp"

long v = 0;

std::string freshVar() {
    std::string res = "a" + std::to_string(v);
    v++;
    return res;
}

void generateMainStart(const std::string& sp) {
    std::ifstream file("print_functions.llvm");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string funcs = buffer.str();
    cb.emit(funcs);

    cb.emit("@.divZero = internal constant [23 x i8] c\"Error division by zero\\00\"\n");
    cb.emit("define i32 @main(){\n");
    cb.emit("%" + sp + " = alloca i32, i32 50");

}

void generateMainEnd() {
    cb.emit("ret i32 0\n}");
}

void opCommand(Exp* e, Exp* left, Exp* right, Operator* o){
    std::string type = Exp::resultType(left,right);
    std::string op = "";
    std::string res = "";
    if(o->op == "+") op = "add";
    else if (o->op == "-") op = "sub";
    else if (o->op == "*") op = "mul";
    else{
        if(type == "INT") op = "sdiv";
        else op = "udiv";
        std::string cond = freshVar();
        std::string errorDiv = cb.freshLabel();
        std::string goodDiv = cb.freshLabel();


        res = "%" + cond + "=icmp eq i32 0 ,%" + right->var + "\n" +
              "br i1 %" + cond + ",label %" + errorDiv + " ,label %" + goodDiv + "\n" +
              errorDiv  + ":" + "\n" +
              "call i32 (i8*, ...) @printf(i8* getelementptr ([23 x i8], [23 x i8]* @.divZero, i32 0, i32 0))" + "\n" +
              "ret i32 0" + "\n" +
              goodDiv + ":" + "\n";

    }
    std::string target = freshVar();
    res += "%" + target + " = " + op + " " + "i32" + " %" + left->var + ", %" + right->var;
    if(type == "BYTE")
    {
        std::string tmpVar = freshVar();
        res += "\n%" + tmpVar + " = " + "and i32 %" + target+ ", 255";
        target = tmpVar;
    }
    cb.emit(res);
    e->var = target;
}


void relopCommand(Exp* e, Exp* left, Exp* right, Operator* o)
{
    std::string cond = freshVar();
    std::string trueLab = cb.freshLabel();
    std::string falseLab = cb.freshLabel();
    std::string type = Exp::resultType(left,right);
    std::string relop = type == "INT" ?  "s" : "u";

    if(o->op == "<") relop += "lt";
    else if(o->op == "<=") relop += "le";
    else if (o->op == ">") relop += "gt";
    else relop += "ge";

    cb.emit("%" + cond + " = icmp " + relop + " i32 %" + left->var + ", %" + right->var);
    cb.emit("br i1 %" + cond + ", label %" + trueLab + ", label %" + falseLab);

    e->trueLabel = trueLab;
    e->falseLabel = falseLab;

}

void releqCommand(Exp* e, Exp* left, Exp* right, Operator* o)
{
    std::string target = freshVar();
    std::string trueLab = cb.freshLabel();
    std::string falseLab = cb.freshLabel();
    std::string releq = "";

    if(o->op == "==") releq += "eq";
    else releq += "ne";

    cb.emit("%" + target + " = icmp " + releq + " i32 %" + left->var + ", %" + right->var);
    cb.emit("br i1 %" + target + ", label %" + trueLab + ", label %" + falseLab);

    e->trueLabel = trueLab;
    e->falseLabel = falseLab;

}

void trueCommand(Exp* e) {
    std::string trueLab = cb.freshLabel();
    std::string falseLab = cb.freshLabel();
    cb.emit("br label %" + trueLab);
    e->trueLabel = trueLab;
    e->falseLabel = falseLab;
}

void falseCommand(Exp* e) {
    std::string trueLab = cb.freshLabel();
    std::string falseLab = cb.freshLabel();
    cb.emit("br label %" + falseLab);
    e->trueLabel = trueLab;
    e->falseLabel = falseLab;
}

void orCommand(Exp* e, Exp* left, Exp* right) {
    e->trueLabel = right->trueLabel;
    e->falseLabel = right->falseLabel;
    cb.emit(left->trueLabel + ":\nbr label %" + e->trueLabel);
}

void andCommand(Exp* e, Exp* left, Exp* right) {
    e->trueLabel = right->trueLabel;
    e->falseLabel = right->falseLabel;
    cb.emit(left->falseLabel + ":\nbr label %" + e->falseLabel);
}

void notCommand(Exp* e) {
    std::string tempLabel = e->trueLabel;
    e->trueLabel = e->falseLabel;
    e->falseLabel = tempLabel;
}

void numCommand(Exp* e, Num* num) {
    e->var = freshVar();
    cb.emit("%" + e->var + " = add i32 0, " + to_string(num->getVal()));
}