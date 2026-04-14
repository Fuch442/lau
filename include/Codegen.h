#pragma once

#include "AST.h"
#include <string>
#include <map>
#include <set>

class CodeGen {
public:
    std::string output;
    std::map<std::string, int> variables;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> functions;
    std::set<std::string> definedFunctions;
    int labelCounter = 0;
    int tempCounter = 0;

    CodeGen();

    void generate(ProgramNode* program);
    void generateFunction(FunctionNode* func);
    void generateStatement(Expr stmt);
    void generateExpression(Expr expr);

    std::string makeLabel();
    std::string makeTemp();

    void emit(const std::string& code);
    void emitLabel(const std::string& label);

    int getVariableOffset(const std::string& name);
    void addVariable(const std::string& name, int size);

    std::string convertFloat(float val);
    std::string convertInt(int val);
};