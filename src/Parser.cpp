#include "../include/AST.h"
#include <stdexcept>
#include <sstream>

Parser::Parser(const std::vector<Token>& tok) : tokens(tok) {}

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();

    while (!isAtEnd()) {
        try {
            auto func = function();
            if (func) {
                program->functions.push_back(std::move(func));
            }
        } catch (const std::exception& e) {
            synchronize();
        }
    }

    return program;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::checkNext(TokenType type) const {
    if (current + 1 >= (int)tokens.size()) return false;
    return tokens[current + 1].type == type;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END;
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::peekNext() const {
    if (current + 1 >= (int)tokens.size()) return tokens[tokens.size() - 1];
    return tokens[current + 1];
}

Expr Parser::expression() {
    return equality();
}

Expr Parser::equality() {
    Expr left = comparison();

    while (check(TokenType::EQ) || check(TokenType::NEQ)) {
        std::string op = advance().lexeme;
        Expr right = comparison();
        left = new BinaryNode(left, op, right, left->line);
    }

    return left;
}

Expr Parser::comparison() {
    Expr left = term();

    while (check(TokenType::GT) || check(TokenType::LT) || check(TokenType::GTE) || check(TokenType::LTE)) {
        std::string op = advance().lexeme;
        Expr right = term();
        left = new BinaryNode(left, op, right, left->line);
    }

    return left;
}

Expr Parser::term() {
    Expr left = factor();

    while (check(TokenType::PLUS) || check(TokenType::MINUS) || check(TokenType::PLUS_EQ) || check(TokenType::MINUS_EQ)) {
        std::string op = advance().lexeme;
        Expr right = factor();
        left = new BinaryNode(left, op, right, left->line);
    }

    return left;
}

Expr Parser::factor() {
    Expr left = unary();

    while (check(TokenType::STAR) || check(TokenType::SLASH) || check(TokenType::PERCENT)) {
        std::string op = advance().lexeme;
        Expr right = unary();
        left = new BinaryNode(left, op, right, left->line);
    }

    return left;
}

Expr Parser::unary() {
    if (check(TokenType::NOT) || check(TokenType::MINUS)) {
        std::string op = advance().lexeme;
        Expr operand = unary();
        return new UnaryNode(op, operand, peek().line);
    }

    return call();
}

Expr Parser::call() {
    Expr expr = primary();

    if (check(TokenType::LPAREN)) {
        advance();
        auto callNode = new CallNode("", expr->line);
        callNode->name = ((IdentNode*)expr)->name;

        if (!check(TokenType::RPAREN)) {
            callNode->args.push_back(expression());
            while (check(TokenType::COMMA)) {
                advance();
                callNode->args.push_back(expression());
            }
        }

        if (!check(TokenType::RPAREN)) {
            throw std::runtime_error("Expected ')' at line " + std::to_string(peek().line));
        }
        advance();

        return callNode;
    }

    return expr;
}

Expr Parser::primary() {
    if (check(TokenType::NUMBER)) {
        double val = std::stod(advance().lexeme);
        return new NumberNode(val, peek().line);
    }

    if (check(TokenType::FLOAT)) {
        double val = std::stod(advance().lexeme);
        return new NumberNode(val, peek().line);
    }

    if (check(TokenType::STRING)) {
        std::string val = advance().lexeme;
        return new StringNode(val, peek().line);
    }

    if (check(TokenType::IDENT)) {
        std::string name = advance().lexeme;
        if (check(TokenType::ASSIGN)) {
            advance();
            Expr value = expression();
            return new AssignNode(name, value, peek().line);
        }
        return new IdentNode(name, peek().line);
    }

    if (check(TokenType::LPAREN)) {
        advance();
        Expr expr = expression();
        if (!check(TokenType::RPAREN)) {
            throw std::runtime_error("Expected ')' at line " + std::to_string(peek().line));
        }
        advance();
        return expr;
    }

    throw std::runtime_error("Unexpected token: " + peek().lexeme + " at line " + std::to_string(peek().line));
}

Expr Parser::declaration() {
    if (check(TokenType::INT) || check(TokenType::FLOAT) || check(TokenType::CHAR)) {
        std::string type = advance().lexeme;
        std::string name = peek().lexeme;
        advance();

        Expr init = nullptr;
        if (check(TokenType::ASSIGN)) {
            advance();
            init = expression();
        }

        return new DeclarationNode(name, type, init ? init->line : type == "int" ? 0 : 0);
    }

    return statement();
}

Expr Parser::statement() {
    if (check(TokenType::IF)) return ifStatement();
    if (check(TokenType::WHILE)) return whileStatement();
    if (check(TokenType::FOR)) return forStatement();
    if (check(TokenType::RETURN)) return returnStatement();
    if (check(TokenType::PRINT)) return printStatement();
    if (check(TokenType::READ)) return readStatement();
    if (check(TokenType::LBRACE)) return compound();
    if (check(TokenType::SEMICOLON)) {
        advance();
        return nullptr;
    }

    Expr expr = expression();
    if (check(TokenType::SEMICOLON)) advance();
    return expr;
}

Expr Parser::ifStatement() {
    int ln = peek().line;
    advance();

    if (!check(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'if'");
    }
    advance();

    Expr condition = expression();

    if (!check(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after condition");
    }
    advance();

    Expr thenBranch = statement();

    Expr elseBranch = nullptr;
    if (check(TokenType::ELSE)) {
        advance();
        elseBranch = statement();
    }

    return new IfNode(condition, thenBranch, elseBranch, ln);
}

Expr Parser::whileStatement() {
    int ln = peek().line;
    advance();

    if (!check(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'while'");
    }
    advance();

    Expr condition = expression();

    if (!check(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after condition");
    }
    advance();

    Expr body = statement();

    return new WhileNode(condition, body, ln);
}

Expr Parser::forStatement() {
    int ln = peek().line;
    advance();

    if (!check(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'for'");
    }
    advance();

    Expr init = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        init = expression();
    }
    if (check(TokenType::SEMICOLON)) advance();

    Expr condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    if (check(TokenType::SEMICOLON)) advance();

    Expr increment = nullptr;
    if (!check(TokenType::RPAREN)) {
        increment = expression();
    }
    if (!check(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after for clauses");
    }
    advance();

    Expr body = statement();

    return new ForNode(init, condition, increment, body, ln);
}

Expr Parser::returnStatement() {
    int ln = peek().line;
    advance();

    Expr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }

    if (!check(TokenType::SEMICOLON)) {
        throw std::runtime_error("Expected ';' after return");
    }
    advance();

    return new ReturnNode(value, ln);
}

Expr Parser::printStatement() {
    int ln = peek().line;
    advance();

    if (!check(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'print'");
    }
    advance();

    Expr value = expression();

    if (!check(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after print argument");
    }
    advance();

    if (!check(TokenType::SEMICOLON)) {
        throw std::runtime_error("Expected ';' after print");
    }
    advance();

    return new PrintNode(value, ln);
}

Expr Parser::readStatement() {
    int ln = peek().line;
    advance();

    if (!check(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after 'read'");
    }
    advance();

    if (!check(TokenType::IDENT)) {
        throw std::runtime_error("Expected variable name in read");
    }
    std::string name = advance().lexeme;

    if (!check(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after read argument");
    }
    advance();

    if (!check(TokenType::SEMICOLON)) {
        throw std::runtime_error("Expected ';' after read");
    }
    advance();

    return new ReadNode(name, ln);
}

Expr Parser::compound() {
    int ln = peek().line;
    advance();

    auto compound = std::make_unique<CompoundNode>(ln);

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        compound->statements.push_back(declaration());
    }

    if (!check(TokenType::RBRACE)) {
        throw std::runtime_error("Expected '}' at line " + std::to_string(peek().line));
    }
    advance();

    return compound.release();
}

std::pair<std::string, std::string> Parser::parameter() {
    if (!check(TokenType::INT) && !check(TokenType::FLOAT) && !check(TokenType::CHAR) && !check(TokenType::VOID)) {
        throw std::runtime_error("Expected type");
    }

    std::string type = advance().lexeme;
    std::string name = advance().lexeme;
    return {type, name};
}

std::vector<std::pair<std::string, std::string>> Parser::parameterList() {
    std::vector<std::pair<std::string, std::string>> params;

    if (check(TokenType::RPAREN)) {
        return params;
    }

    params.push_back(parameter());

    while (check(TokenType::COMMA)) {
        advance();
        params.push_back(parameter());
    }

    return params;
}

std::vector<Expr> Parser::argumentList() {
    std::vector<Expr> args;

    if (check(TokenType::RPAREN)) {
        return args;
    }

    args.push_back(expression());

    while (check(TokenType::COMMA)) {
        advance();
        args.push_back(expression());
    }

    return args;
}

Expr Parser::function() {
    if (!check(TokenType::INT) && !check(TokenType::FLOAT) && !check(TokenType::CHAR) && !check(TokenType::VOID)) {
        throw std::runtime_error("Expected return type");
    }

    std::string returnType = advance().lexeme;

    if (!check(TokenType::IDENT)) {
        throw std::runtime_error("Expected function name");
    }
    std::string name = advance().lexeme;

    if (!check(TokenType::LPAREN)) {
        throw std::runtime_error("Expected '(' after function name");
    }
    advance();

    auto params = parameterList();

    if (!check(TokenType::RPAREN)) {
        throw std::runtime_error("Expected ')' after parameters");
    }
    advance();

    if (!check(TokenType::LBRACE)) {
        throw std::runtime_error("Expected '{' after function signature");
    }
    advance();

    auto body = std::make_unique<CompoundNode>(peek().line);

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        body->statements.push_back(declaration());
    }

    if (!check(TokenType::RBRACE)) {
        throw std::runtime_error("Expected '}' at line " + std::to_string(peek().line));
    }
    advance();

    auto func = new FunctionNode(name, returnType, body->line);
    func->params = params;
    func->body = body.release();
    return func;
}

void Parser::synchronize() {
    while (!isAtEnd()) {
        if (peek().type == TokenType::SEMICOLON) {
            advance();
            return;
        }
        advance();
    }
}