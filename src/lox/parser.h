#pragma once
#include "ast.h"
#include "scanner.h"

void parser_parse(const Array<Token>& tokens, Array<UniquePtr<Stmt>>& stmts);
