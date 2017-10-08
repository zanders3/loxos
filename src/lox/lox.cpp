#include "lox.h"
#include "scanner.h"
#include "vga.h"

void lox_run(const char* source, int sourceLen)
{
    Array<Token> tokens;
    scanner_scan(source, sourceLen, tokens);
    for (const Token& tok : tokens)
    {
        if (tok.type == TokenType::STRING)
            vga.Print("STRING %?\n", tok.stringLiteral);
        else if (tok.type == TokenType::NUMBER)
            vga.Print("NUMBER %?\n", tok.numberLiteral);
        else
            vga.Print("%? %?\n", tokentype_to_string(tok.type), (const char*)tok.lexeme);
    }
    vga.Puts('\n');
}

void lox_error(int line, const char* message)
{
    vga.SetColor(VGAColor::Red, VGAColor::Black);
    vga.Print("%? at %?\n", message, line);
    vga.SetColor(VGAColor::White, VGAColor::Black);
}
