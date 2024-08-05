#pragma once

#include "hw3_output.hpp"
#include <string>
#include <vector>

extern int yylineno;

class Symbol {
public:
    std::string name;
    int offset;
    std::string type;
    std::string argType;

    Symbol(std::string name, int offset, std::string type, std::string argType = "");
};

class SymbolTable {
private:
    SymbolTable* parent;

public:
    std::vector<Symbol*> table;

    SymbolTable(SymbolTable* p = nullptr);

    void addSymbol(Symbol& sym);
    bool doesExist(std::string& name);
    Symbol* get(std::string& name);
};

class ScopeStack {
private:


public:
    std::vector<SymbolTable*> tables;
    std::vector<int> offsets;
    ScopeStack() = default;
    ~ScopeStack();
    void insertFunctions();
    void addScope();
    void removeScope();
    void insertSymbol(Symbol& sym);
    Symbol* search(std::string& name, bool isFunc);
};
