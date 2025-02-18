#pragma once

#include "symbols.hpp"
#include "hw3_output.hpp"
#include "generator.hpp"
#include "cg.hpp"
#include <string>

extern int yylineno;
extern CodeBuffer& cb;
extern std::string sp;

struct Node {
    std::string value;

    Node(const std::string& value = "") : value(value) {}
    Node(const Node& node) : value(node.value) {}
    virtual ~Node() = default;

    //virtual std::string prettyPrint();
};

#define YYSTYPE Node*


class Num : public Node {
private:
    int val;
public:
    Num(const std::string& str);
    int getVal() const { return val; }  // Add getter for val
    void checkNumByte();
};

class Id : public Node {
public:
    std::string name;
    Id(const std::string& n);
};

class String : public Node {
public:
    std::string val;
    std::string name;
    int length;
    String(const std::string& v);
};

class Exp;

class Type : public Node {
public:
    std::string type;
    Type(const std::string& type);
};

class Call;

class Exp : public Node {
public:
    std::string type;
    std::string var;
    std::string trueLabel;
    std::string falseLabel;
    int length;
    Exp() : type("void"), var("") {}
    Exp(Exp *exp);
    Exp(Id* id);
    Exp(Call* call);
    Exp(Exp* exp,Type *type);
    Exp(const std::string& type);
    Exp(String* str);

    static void checkNumeric(Exp* left, Exp* right);
    static void checkCasting(Type* type, Exp* exp);
    static void checkBool(Exp* exp);
    static std::string resultType(Exp* left, Exp* right);
};

class Operator : public Node{
public:
    std::string op;
    Operator(const std::string& o);

};

class Call : public Node {
public:
    std::string returnType;
    std::string name;
    std::string argType;
    std::string var;
    Call(Id *id, Exp *exp);

    void checkFunc(Symbol* sym, Exp* exp);
};

class Program : public Node {};

class Statement : public Node {
public:
    std::string nextLabel;
    Statement(Node* node);
    Statement(Type* type, Id* id, Exp* exp);
    Statement(Type* type, Id* id);
    Statement(Id* id, Exp* exp);
    Statement(const std::string name, Exp* exp);
    Statement(Exp* exp, bool is_return);
    Statement(Call* call);
    bool checkAssign(std::string& left, std::string& right);

    virtual ~Statement() = default;
};

class Statements : public Node {
public:
    Statements(Statement *statement) : Node() {};

    Statements(Statements *statements, Statement *statement) : Node() {};

    virtual ~Statements() = default;
};

class N : public Node {
public:
    std::string next;
    N(const std::string& s) : next(s) {};
};


