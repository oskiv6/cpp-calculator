#include "Interpreter.hpp"
#include <stdexcept>
#include <cmath>

// ====

double Interpreter::evaluate(const AstNode* node) {
    if (!node) return 0;
    return visit(node);
}

double Interpreter::visit(const AstNode* node) {
    // Determine node type and delegate
    if (auto lit = dynamic_cast<const AstLiteral*>(node)) return visitLiteral(lit);
    if (auto bin = dynamic_cast<const AstBinaryExpr*>(node)) return visitBinary(bin);
    if (auto var = dynamic_cast<const AstVar*>(node)) return visitVar(var);
    if (auto assign = dynamic_cast<const AstAssignment*>(node)) return visitAssignment(assign);

    throw std::runtime_error("Unknown AST node type");
}

double Interpreter::visitLiteral(const AstLiteral* node) {
    try {
        return std::stod(node->token().lexeme());
    } catch (...) {
        throw std::runtime_error("Invalid number literal: " + node->token().lexeme());
    }
}

double Interpreter::visitBinary(const AstBinaryExpr* node) {
    double left = visit(node->left());
    double right = visit(node->right());

    switch (node->op().type()) {
        case TokenType::TK_PLUS:     return left + right;
        case TokenType::TK_MINUS:    return left - right;
        case TokenType::TK_ASTERISK: return left * right;
        case TokenType::TK_SLASH: {
            if (right == 0) throw std::runtime_error("Division by zero");
            return left / right;
        }
        case TokenType::TK_PERCENT:  return std::fmod(left, right);
        default: throw std::runtime_error("Unsupported operator in interpreter");
    }
}

double Interpreter::visitVar(const AstVar* node) {
    auto it = variables_.find(node->identifier());
    if (it == variables_.end()) {
        throw std::runtime_error("Undefined variable: " + node->identifier());
    }
    return it->second;
}

double Interpreter::visitAssignment(const AstAssignment* node) {
    double value = visit(node->value());
    variables_[node->identifier()] = value;
    return value;
}
