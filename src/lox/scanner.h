#pragma once
#include "karray.h"

enum class TokenType
{
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,
    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,
    // Literals
    IDENTIFIER, STRING, NUMBER,
    // Keywords
    AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
    EOF
};

const char* tokentype_to_string(const TokenType token);

struct Token
{
    TokenType type;
    char lexeme[32];
    int numberLiteral;
    const char* stringLiteral;
    int line;

    ~Token();
};

void scanner_scan(const char* source, int sourceLen, Array<Token>& tokens);