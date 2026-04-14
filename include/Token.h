#pragma once

#include <string>
#include <vector>

enum class TokenType {
    END,
    IDENT,
    NUMBER,
    STRING,
    INT,
    FLOAT,
    CHAR,
    VOID,
    IF,
    ELSE,
    WHILE,
    FOR,
    RETURN,
    PRINT,
    READ,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    EQ,
    NEQ,
    LT,
    GT,
    LTE,
    GTE,
    AND,
    OR,
    NOT,
    ASSIGN,
    PLUS_EQ,
    MINUS_EQ,
    STAR_EQ,
    SLASH_EQ,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    SEMICOLON,
    COLON
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType t, std::string l, int ln) : type(t), lexeme(std::move(l)), line(ln) {}
};

class Lexer {
public:
    std::vector<Token> tokens;
    int current = 0;
    int start = 0;
    int line = 1;
    std::string source;

    Lexer(const std::string& src);

    std::vector<Token> tokenize();

private:
    char advance();
    char peek();
    char peekNext();
    bool match(char expected);
    bool isAtEnd();

    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& lexeme);

    void string();
    void number();
    void identifier();

    void skipWhitespace();
    void skipComment();

    void scanToken();
};