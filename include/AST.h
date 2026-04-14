#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

enum class NodeType {
    PROGRAM,
    FUNCTION,
    DECLARATION,
    PARAM_LIST,
    COMPOUND,
    IF,
    WHILE,
    FOR,
    RETURN,
    PRINT,
    READ,
    BINARY,
    UNARY,
    ASSIGN,
    CALL,
    IDENT,
    NUMBER,
    STRING,
    CHAR_LIT
};

struct ASTNode {
    NodeType type;
    int line;

    ASTNode(NodeType t, int ln) : type(t), line(ln) {}
    virtual ~ASTNode() = default;
};

using Expr = ASTNode*;

struct ProgramNode : ASTNode {
    std::vector<Expr> functions;

    ProgramNode() : ASTNode(NodeType::PROGRAM, 0) {}
};

struct FunctionNode : ASTNode {
    std::string name;
    std::string returnType;
    std::vector<std::pair<std::string, std::string>> params;
    Expr body;

    FunctionNode(const std::string& n, const std::string& rt, int ln)
        : ASTNode(NodeType::FUNCTION, ln), name(n), returnType(rt) {}
};

struct DeclarationNode : ASTNode {
    std::string name;
    std::string type;
    Expr init;

    DeclarationNode(const std::string& n, const std::string& t, int ln)
        : ASTNode(NodeType::DECLARATION, ln), name(n), type(t) {}
};

struct CompoundNode : ASTNode {
    std::vector<Expr> statements;

    CompoundNode(int ln) : ASTNode(NodeType::COMPOUND, ln) {}
};

struct IfNode : ASTNode {
    Expr condition;
    Expr thenBranch;
    Expr elseBranch;

    IfNode(Expr cond, Expr thenB, Expr elseB, int ln)
        : ASTNode(NodeType::IF, ln), condition(cond), thenBranch(thenB), elseBranch(elseB) {}
};

struct WhileNode : ASTNode {
    Expr condition;
    Expr body;

    WhileNode(Expr cond, Expr bod, int ln)
        : ASTNode(NodeType::WHILE, ln), condition(cond), body(bod) {}
};

struct ForNode : ASTNode {
    Expr init;
    Expr condition;
    Expr increment;
    Expr body;

    ForNode(Expr init, Expr cond, Expr inc, Expr bod, int ln)
        : ASTNode(NodeType::FOR, ln), init(init), condition(cond), increment(inc), body(bod) {}
};

struct ReturnNode : ASTNode {
    Expr value;

    ReturnNode(Expr val, int ln) : ASTNode(NodeType::RETURN, ln), value(val) {}
};

struct PrintNode : ASTNode {
    Expr value;

    PrintNode(Expr val, int ln) : ASTNode(NodeType::PRINT, ln), value(val) {}
};

struct ReadNode : ASTNode {
    std::string name;

    ReadNode(const std::string& n, int ln) : ASTNode(NodeType::READ, ln), name(n) {}
};

struct BinaryNode : ASTNode {
    Expr left;
    std::string op;
    Expr right;

    BinaryNode(Expr l, const std::string& o, Expr r, int ln)
        : ASTNode(NodeType::BINARY, ln), left(l), op(o), right(r) {}
};

struct UnaryNode : ASTNode {
    std::string op;
    Expr operand;

    UnaryNode(const std::string& o, Expr opnd, int ln)
        : ASTNode(NodeType::UNARY, ln), op(o), operand(opnd) {}
};

struct AssignNode : ASTNode {
    std::string name;
    Expr value;

    AssignNode(const std::string& n, Expr val, int ln)
        : ASTNode(NodeType::ASSIGN, ln), name(n), value(val) {}
};

struct CallNode : ASTNode {
    std::string name;
    std::vector<Expr> args;

    CallNode(const std::string& n, int ln)
        : ASTNode(NodeType::CALL, ln), name(n) {}
};

struct IdentNode : ASTNode {
    std::string name;

    IdentNode(const std::string& n, int ln) : ASTNode(NodeType::IDENT, ln), name(n) {}
};

struct NumberNode : ASTNode {
    double value;

    NumberNode(double val, int ln) : ASTNode(NodeType::NUMBER, ln), value(val) {}
};

struct StringNode : ASTNode {
    std::string value;

    StringNode(const std::string& val, int ln) : ASTNode(NodeType::STRING, ln), value(val) {}
};

struct CharNode : ASTNode {
    char value;

    CharNode(char val, int ln) : ASTNode(NodeType::CHAR_LIT, ln), value(val) {}
};

class Parser {
public:
    std::vector<Token> tokens;
    int current = 0;

    Parser(const std::vector<Token>& tok);

    std::unique_ptr<ProgramNode> parse();

private:
    bool check(TokenType type) const;
    bool checkNext(TokenType type) const;
    bool isAtEnd() const;
    Token& advance();
    const Token& peek() const;
    const Token& peekNext() const;

    Expr expression();
    Expr equality();
    Expr comparison();
    Expr term();
    Expr factor();
    Expr unary();
    Expr call();
    Expr primary();

    Expr declaration();
    Expr statement();
    Expr ifStatement();
    Expr whileStatement();
    Expr forStatement();
    Expr returnStatement();
    Expr printStatement();
    Expr readStatement();
    Expr compound();

    std::pair<std::string, std::string> parameter();
    std::vector<std::pair<std::string, std::string>> parameterList();
    std::vector<Expr> argumentList();

    Expr function();

    void synchronize();
};