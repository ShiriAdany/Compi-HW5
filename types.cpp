#include "types.hpp"
#include <cstdlib>
#include <iostream>
extern ScopeStack* stack;

//------------------------------------------------------------------
Num::Num(const std::string& str) {
    try {
        val = std::stoi(str);
    } catch (const std::out_of_range& e) {
        output::errorMismatch(yylineno);
        exit(1);
    } catch (const std::invalid_argument& e) {
        output::errorMismatch(yylineno);
        exit(1);
    }
}

void Num::checkNumByte() {
    if (val > 255) {
        output::errorByteTooLarge(yylineno, std::to_string(val));
        exit(1);
    }
}

//------------------------------------------------------------------
Id::Id(const std::string& n) : name(n) {}

//------------------------------------------------------------------
String::String(const std::string& v) : val(v) {}

//------------------------------------------------------------------
Statement::Statement(Node* node) {
    // Constructor implementation
}

Statement::Statement(Type* type, Id* id) {
    Symbol* sym = new Symbol(id->name,stack->offsets.back(),type->type);
    stack->insertSymbol(*sym);
}

Statement::Statement(Type* type, Id* id, Exp* exp) {
    if (!checkAssign(type->type, exp->type)) {
        output::errorMismatch(yylineno);
        exit(1);
    }
    Symbol* sym = new Symbol(id->name,stack->offsets.back(),type->type);
    stack->insertSymbol(*sym);
}

bool Statement::checkAssign(std::string& left, std::string& right) {
    if (left != right) {
        if (left == "INT" && right == "BYTE") {
            return true;
        } else {
            return false;
        }
    }

    return true;
}


Statement::Statement(Id* id, Exp* exp) {
    Symbol* sym = stack->search(id->name, false);
    if (!checkAssign(sym->type, exp->type)) {
        output::errorMismatch(yylineno);
        exit(1);
    }
}


Statement::Statement(const std::string name, Exp* exp) {
    // Constructor implementation
}

Statement::Statement(Exp* exp, bool is_return) {
    // Constructor implementation
}

Statement::Statement(Call* call) {
    // Constructor implementation
}

//------------------------------------------------------------------
Call::Call(Id* id, Exp* exp){

    Symbol* sym = stack->search(id->name, true);
    Call::checkFunc(sym, exp);
    this->returnType = sym->type;
    this->argType = sym->argType;
    this->name = sym->name;
}

void Call::checkFunc(Symbol* sym, Exp* exp) {
    if (sym->argType.empty()) {
        output::errorUndefFunc(yylineno, sym->name);
        exit(1);
    }

    if (exp->type != sym->argType) {
        if (exp->type == "BYTE" && sym->argType == "INT") {
            return;
        }
        output::errorPrototypeMismatch(yylineno, sym->name, sym->argType);
        exit(1);
    }
}

//------------------------------------------------------------------
Type::Type(const std::string& type) : type(type) {}

//------------------------------------------------------------------
Exp::Exp(Exp *exp) : type(exp->type) {}

Exp::Exp(Exp* exp,Type *type) {

    Type* casted_type = dynamic_cast<Type*>(type);
    this->type = casted_type->type;

}

Exp::Exp(const std::string& type):type(type){}

Exp::Exp(Id* id)
{
    Symbol *symbol = stack->search(id->name, false);
    type = symbol->type;

}

Exp::Exp(Call* call)
{
    //Symbol *symbol = stack->search(call->name, true);
    type = call->returnType;

}
void Exp::checkNumeric(Exp* left, Exp* right) {
    if (left->type == "STRING" || left->type == "BOOL" ||
        right->type == "STRING" || right->type == "BOOL") {
        output::errorMismatch(yylineno);
        exit(1);
    }
}

void Exp::checkCasting(Type* type, Exp* exp) {
    if (type->type == "STRING" || type->type == "BOOL" ||
        exp->type == "STRING" || exp->type == "BOOL") {
        output::errorMismatch(yylineno);
        exit(1);
    }
}

void Exp::checkBool(Exp* exp) {
    if (exp->type != "BOOL") {
        output::errorMismatch(yylineno);
        exit(1);
    }
}

std::string Exp::resultType(Exp* left, Exp* right) {
    if (left->type == "BYTE" && right->type == "BYTE") {
        return "BYTE";
    } else {
        return "INT";
    }
}

//-------------------------------------------------

Operator::Operator(const std::string &o) : op(o){}

std::string Operator::opCommand(Exp* left, Exp* right, Operator* o){
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


        res = "%" + cond + "=icmp eq i32 0 %" + right->var + "\n" +
        "br i1 %" + cond + ",label %" + errorDiv + " ,label %" + goodDiv + "\n" +
        errorDiv  + ":" + "\n" +
        "call i32 (i8*, ...) @printf(i8* getelementptr ([23 x i8], [23 x i8]* @.divZero, i32 0, i32 0))" + "\n" +
        "ret i32 0" + "\n" +
        goodDiv + ":" + "\n";

    }
    std::string target = freshVar();
    res += "%" + target + " = " + op + " " + "i32" + " %" + left->var + " %" + right->var;
    if(type == "BYTE")
    {
        res += "\n%" + target + " = " + "and i32 %" + target+ ", 255";
    }
    cb.emit(res);
    return target;
}