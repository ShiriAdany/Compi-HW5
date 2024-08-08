#include "symbols.hpp"

#include <iostream>
 ScopeStack* stack = nullptr;

//-----------------------------------------------------------

Symbol::Symbol(std::string name, int offset, std::string type, std::string argType)
        : name(std::move(name)), offset(offset), type(std::move(type)), argType(std::move(argType)) { }

//-----------------------------------------------------------

SymbolTable::SymbolTable(SymbolTable* p) : parent(p) { }

void SymbolTable::addSymbol(Symbol& sym) {
    for (auto it = stack->tables.rbegin(); it != stack->tables.rend(); ++it) {
        SymbolTable* t = *it;
        if (t->doesExist(sym.name)) {
            output::errorDef(yylineno,sym.name);
            exit(1);
        }
    }
    table.push_back(&sym);
}

bool SymbolTable::doesExist(std::string& name) {
    for (auto* sym : table) {
        if (name == sym->name) {
            return true;
        }
    }
    return false;
}



Symbol* SymbolTable::get(std::string& name) {
    for (auto* sym : table) {
        if (name == sym->name) {
            return sym;
        }
    }
    return nullptr;
}

//-----------------------------------------------------------
void ScopeStack::insertFunctions() {
    SymbolTable* t = tables.back();
    Symbol* print = new Symbol("print",0,"VOID","STRING");
    t->addSymbol(*print);
    Symbol* printi = new Symbol("printi",0,"VOID","INT");
    t->addSymbol(*printi);
    Symbol* readi = new Symbol("readi",0,"INT","INT");
    t->addSymbol(*readi);
}


void ScopeStack::addScope() {
    SymbolTable* newTable = tables.empty() ? new SymbolTable(nullptr) : new SymbolTable(tables.back());
    tables.push_back(newTable);

    if (offsets.empty()) {
        offsets.push_back(0);
    } else {
        offsets.push_back(offsets.back());
    }
}

void ScopeStack::removeScope() {
    output::endScope();
    if(!tables.empty()){
        SymbolTable* currTable = tables.back();
        for (auto it = currTable->table.begin(); it != currTable->table.end(); ++it) {
            Symbol* sym = *it;
            if(sym->argType != "") //function
            {
                string type = output::makeFunctionType(sym->argType, sym->type);
                output::printID(sym->name, sym->offset, type);
            }
            else output::printID(sym->name,sym->offset,sym->type);
        }

        delete currTable;
        tables.pop_back();
    }
    if(!offsets.empty()) offsets.pop_back();

}


Symbol* ScopeStack::search(std::string& name, bool isFunc) {
    for (auto it = tables.rbegin(); it != tables.rend(); ++it) {
        SymbolTable* t = *it;
        if (t->doesExist(name)) {
            Symbol* sym = t->get(name);
            if ((isFunc && sym->argType.empty()) || (!isFunc && !sym->argType.empty())) {
                if (isFunc) {
                    output::errorUndefFunc(yylineno, name);
                } else {
                    output::errorUndef(yylineno, name);
                }
                exit(1);
            }
            return sym;
        }
    }

    if (isFunc) {
        output::errorUndefFunc(yylineno, name);
    } else {
        output::errorUndef(yylineno, name);
    }
    exit(1);
}


void ScopeStack::insertSymbol(Symbol& sym)
{
    SymbolTable* t = tables.back();
    t->addSymbol(sym);
    int size = offsets.size();
    offsets[size-1]++;
}

ScopeStack::~ScopeStack() {
    while (!tables.empty()) {
        removeScope();
    }
}



