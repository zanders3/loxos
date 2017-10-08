#pragma once

void lox_run(const char* source, int sourceLen);
void lox_error(int line, const char* message);
