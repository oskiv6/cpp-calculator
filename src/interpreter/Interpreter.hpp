#pragma once

#include "../core/Ast.hpp"
#include <unordered_map>
#include <string>

class Interpreter {
public:
    Interpreter() = default;

    // result of the expression
    double evaluate(const AstNode* node);

    // delete variables
    void clear() { variables_.clear(); }

private:
    // storage identifier -> value
    std::unordered_map<std::string, double> variables_;

    double visit(const AstNode* node);
    double visitLiteral(const AstLiteral* node);
    double visitBinary(const AstBinaryExpr* node);
    double visitVar(const AstVar* node);
    double visitAssignment(const AstAssignment* node);
};
