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
String::String(const std::string& v){
    this->val = v;
    this->name = freshVar();

    val.erase(0, 1); // Remove the first character
    val.erase(val.length() - 1, 1);

    this->length=val.length() + 1;
    cb.emitGlobal("@."+name + " = internal constant [" + to_string(length) +" x i8] c\"" + val +"\\00\"");
}

//------------------------------------------------------------------
Statement::Statement(Node* node){
    // Constructor implementation
}

Statement::Statement(Type* type, Id* id){
    Symbol* sym = new Symbol(id->name,stack->offsets.back(),type->type);

    std::string ptr = freshVar();
    cb.emit("%" + ptr + " = getelementptr i32, i32* %" + sp + ", i32 " + to_string(stack->offsets.back()));
    cb.emit("store i32 0, i32* %" + ptr);

    stack->insertSymbol(*sym);
}

Statement::Statement(Type* type, Id* id, Exp* exp){
    if (!checkAssign(type->type, exp->type)) {
        output::errorMismatch(yylineno);
        exit(1);
    }
    Symbol* sym = new Symbol(id->name,stack->offsets.back(),type->type);
    // if bool or byte, zero extend
    if (type->type == "BOOL") {
        std::string endLabel = cb.freshLabel();
        cb.emit(exp->trueLabel + ":");
        cb.emit("br label %" + endLabel);
        cb.emit(exp->falseLabel + ":");
        cb.emit("br label %" + endLabel);
        cb.emit(endLabel + ":");

        exp->var = freshVar();
        cb.emit("%" + exp->var + "= phi i32 [ 1, %" + exp->trueLabel + " ], [ 0, %" + exp->falseLabel + " ]");

    } else if (type->type == "BYTE") {
        std::string tempVar = freshVar();
        cb.emit("%" + tempVar + " = and i32 %" + exp->var + ", 255");
        exp->var = tempVar;
    }

    std::string ptr = freshVar();
    cb.emit("%" + ptr + " = getelementptr i32, i32* %" + sp + ", i32 " + to_string(stack->offsets.back()));
    cb.emit("store i32 %" + exp->var + ", i32* %" + ptr);
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


Statement::Statement(Id* id, Exp* exp){
    Symbol* sym = stack->search(id->name, false);
    if (!checkAssign(sym->type, exp->type)) {
        output::errorMismatch(yylineno);
        exit(1);
    }

    if (sym->type == "BOOL") {
        std::string endLabel = cb.freshLabel();
        cb.emit(exp->trueLabel + ":");
        cb.emit("br label %" + endLabel);
        cb.emit(exp->falseLabel + ":");
        cb.emit("br label %" + endLabel);
        cb.emit(endLabel + ":");

        exp->var = freshVar();
        cb.emit("%" + exp->var + "= phi i32 [ 1, %" + exp->trueLabel + " ], [ 0, %" + exp->falseLabel + " ]");

    } else if (sym->type == "BYTE") {
        std::string tempVar = freshVar();
        cb.emit("%" + tempVar + " = and i32 %" + exp->var + ", 255");
        exp->var = tempVar;
    }

    std::string ptr = freshVar();
    cb.emit("%" + ptr + " = getelementptr i32, i32* %" + sp + ", i32 " + to_string(sym->offset));
    cb.emit("store i32 %" + exp->var + ", i32* %" + ptr);
}


Statement::Statement(const std::string name, Exp* exp){

}

Statement::Statement(Exp* exp, bool is_return){
    // Constructor implementation
}

Statement::Statement(Call* call){
    // Constructor implementation
}

//------------------------------------------------------------------
Call::Call(Id* id, Exp* exp){

    Symbol* sym = stack->search(id->name, true);
    Call::checkFunc(sym, exp);
    this->returnType = sym->type;
    this->argType = sym->argType;
    this->name = sym->name;
    std::string retVal = freshVar();
    this->var = retVal;
    if (returnType == "INT")
        cb.emit("%"+retVal+" = call i32 @readi(i32 %" + exp->var+ ")");
    else if(name == "printi"){
        cb.emit("call void @printi(i32 %" + exp->var+ ")");
    }
    else{
        cb.emit("call void @print(i8* getelementptr (["+ to_string(exp->length) +" x i8], ["+ to_string(exp->length) +" x i8]* @." + exp->var + ", i32 0, i32 0))");
    }
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
Exp::Exp(Exp *exp) : type(exp->type), var(exp->var) {}

Exp::Exp(Exp* exp, Type *type) {

    Type* casted_type = dynamic_cast<Type*>(type);
    this->type = casted_type->type;
    if(this->type == "BYTE"){
        std::string tmpVar = freshVar();
        cb.emit("\n%" + tmpVar + " = " + "and i32 %" + exp->var+ ", 255");
        this->var = tmpVar;
    }
    else{
        this->var = exp->var;
    }


}

Exp::Exp(const std::string& type):type(type), var(""){}

Exp::Exp(String* str){
    this->type = "STRING";
    this->var = str->name;
    this->length = str->length;
}

Exp::Exp(Id* id)
{
    Symbol *symbol = stack->search(id->name, false);
    type = symbol->type;
    var = freshVar();
    std::string ptr = freshVar();
    cb.emit("%" + ptr + " = getelementptr i32, i32* %" + sp + ", i32 " + to_string(symbol->offset));
    cb.emit("%" + var + " = load i32, i32* %" + ptr);

    if(type == "BOOL"){
        std::string trueLab = cb.freshLabel();
        std::string falseLab = cb.freshLabel();
        std::string cond = freshVar();
        cb.emit("%" + cond + "=icmp eq i32 0 ,%" + var);
        cb.emit("br i1 %" + cond + ",label %" + falseLab + " ,label %" + trueLab);
        trueLabel = trueLab;
        falseLabel = falseLab;
    }

}

Exp::Exp(Call* call)
{
    //Symbol *symbol = stack->search(call->name, true);
    type = call->returnType;
    // todo: ?

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