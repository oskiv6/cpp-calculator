#pragma once

#include "../util/Util.hpp"
#include "Token.hpp"

#include <string>
#include <memory>
#include <vector>

// ====

class AstNode {
public:
    virtual ~AstNode() = default;

    explicit AstNode(Span span) : span_(span) {}

    Span span() const { return span_; }

private:
    Span span_;
};

class AstVar : public AstNode {
public:
    AstVar(Span span, std::string identifier)
        : AstNode(span), identifier_(std::move(identifier)) {}

    const std::string& identifier() const { return identifier_; }

private:
    std::string identifier_;
};

class AstLiteral : public AstNode {
public:
    AstLiteral(Span span, Token token)
        : AstNode(span), token_(std::move(token)) {}

    const Token& token() const { return token_; }

private:
    Token token_;
};

class AstBinaryExpr : public AstNode {
public:
    AstBinaryExpr(Span span, std::unique_ptr<AstNode> left, std::unique_ptr<AstNode> right, Token op)
        : AstNode(span),
          left_(std::move(left)),
          right_(std::move(right)),
          op_(std::move(op)) {}

    const AstNode* left() const { return left_.get(); }
    const AstNode* right() const { return right_.get(); }
    const Token& op() const { return op_; }

private:
    // 3. std::unique_ptr manages memory automatically (RAII).
    std::unique_ptr<AstNode> left_;
    std::unique_ptr<AstNode> right_;
    Token op_;
};

class AstAssignment : public AstNode {
public:
    AstAssignment(Span span, std::string identifier, std::unique_ptr<AstNode> value)
        : AstNode(span), identifier_(std::move(identifier)), value_(std::move(value)) {}

    const std::string& identifier() const { return identifier_; }
    const AstNode* value() const { return value_.get(); }

private:
    std::string identifier_;
    std::unique_ptr<AstNode> value_;
};
