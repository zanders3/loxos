#pragma once

struct Token;
class Environment;

void lox_run(Environment& environment, const char* source, int sourceLen);
void lox_error(const Token& token, const char* message);
void lox_error(int line, const char* message);
