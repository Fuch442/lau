#include "../include/Token.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& src) : source(src) {}

std::vector<Token> Lexer::tokenize() {
    tokens.clear();
    current = 0;
    start = 0;
    line = 1;

    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    addToken(TokenType::END);
    return tokens;
}

char Lexer::advance() {
    char c = source[current];
    current++;
    return c;
}

char Lexer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

char Lexer::peekNext() {
    if (current + 1 >= (int)source.size()) return '\0';
    return source[current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

bool Lexer::isAtEnd() {
    return current >= (int)source.size();
}

void Lexer::addToken(TokenType type) {
    addToken(type, source.substr(start, current - start));
}

void Lexer::addToken(TokenType type, const std::string& lexeme) {
    tokens.emplace_back(type, lexeme, line);
}

void Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string at line " + std::to_string(line));
    }

    advance();
    std::string val = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, val);
}

void Lexer::number() {
    while (std::isdigit(peek())) advance();

    if (peek() == '.' && std::isdigit(peekNext())) {
        advance();
        while (std::isdigit(peek())) advance();
        addToken(TokenType::FLOAT);
    } else {
        addToken(TokenType::NUMBER);
    }
}

void Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();

    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENT;

    if (text == "int") type = TokenType::INT;
    else if (text == "float") type = TokenType::FLOAT;
    else if (text == "char") type = TokenType::CHAR;
    else if (text == "void") type = TokenType::VOID;
    else if (text == "if") type = TokenType::IF;
    else if (text == "else") type = TokenType::ELSE;
    else if (text == "while") type = TokenType::WHILE;
    else if (text == "for") type = TokenType::FOR;
    else if (text == "return") type = TokenType::RETURN;
    else if (text == "print") type = TokenType::PRINT;
    else if (text == "read") type = TokenType::READ;

    addToken(type, text);
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else if (c == '\n') {
            line++;
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    if (peek() == '/') {
        if (peekNext() == '/') {
            while (peek() != '\n' && !isAtEnd()) advance();
        } else if (peekNext() == '*') {
            advance();
            advance();
            while (!isAtEnd()) {
                if (peek() == '*' && peekNext() == '/') {
                    advance();
                    advance();
                    break;
                }
                if (peek() == '\n') line++;
                advance();
            }
        }
    }
}

void Lexer::scanToken() {
    char c = advance();

    switch (c) {
        case '(': addToken(TokenType::LPAREN); break;
        case ')': addToken(TokenType::RPAREN); break;
        case '{': addToken(TokenType::LBRACE); break;
        case '}': addToken(TokenType::RBRACE); break;
        case '[': addToken(TokenType::LBRACKET); break;
        case ']': addToken(TokenType::RBRACKET); break;
        case ',': addToken(TokenType::COMMA); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case ':': addToken(TokenType::COLON); break;

        case '+':
            if (match('=')) addToken(TokenType::PLUS_EQ);
            else addToken(TokenType::PLUS);
            break;
        case '-':
            if (match('=')) addToken(TokenType::MINUS_EQ);
            else addToken(TokenType::MINUS);
            break;
        case '*':
            if (match('=')) addToken(TokenType::STAR_EQ);
            else addToken(TokenType::STAR);
            break;
        case '/':
            if (match('=')) addToken(TokenType::SLASH_EQ);
            else addToken(TokenType::SLASH);
            break;
        case '%': addToken(TokenType::PERCENT); break;

        case '=':
            if (match('=')) addToken(TokenType::EQ);
            else addToken(TokenType::ASSIGN);
            break;
        case '!':
            if (match('=')) addToken(TokenType::NEQ);
            else addToken(TokenType::NOT);
            break;
        case '<':
            if (match('=')) addToken(TokenType::LTE);
            else addToken(TokenType::LT);
            break;
        case '>':
            if (match('=')) addToken(TokenType::GTE);
            else addToken(TokenType::GT);
            break;

        case '&':
            if (match('&')) addToken(TokenType::AND);
            break;
        case '|':
            if (match('|')) addToken(TokenType::OR);
            break;

        case '"': string(); break;

        default:
            if (std::isdigit(c)) {
                current--;
                number();
            } else if (std::isalpha(c) || c == '_') {
                current--;
                identifier();
            } else if (c == ' ' || c == '\r' || c == '\t' || c == '\n') {
                if (c == '\n') line++;
            }
            break;
    }
}