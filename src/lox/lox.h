#pragma once

struct Token;

void lox_run(const char* source, int sourceLen);
void lox_error(const Token& token, const char* message);
void lox_error(int line, const char* message);
