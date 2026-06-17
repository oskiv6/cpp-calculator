#pragma once

#include "../util/Util.hpp"
#include "Token.hpp"

#include <string>
#include <memory>
#include <vector>
#include <cstdio>

// ====

class AstNode {
public:
    virtual ~AstNode() = default;
    explicit AstNode(Span span) : span_(span) {}
    Span span() const { return span_; }

    virtual void print(int indent = 0) const = 0;
    void printIndent(int indent) const {
        for (int i = 0; i < indent; ++i) printf("  ");
    }

private:
    Span span_;
};

class AstVar : public AstNode {
public:
    AstVar(Span span, std::string identifier)
        : AstNode(span), identifier_(std::move(identifier)) {}
    const std::string& identifier() const { return identifier_; }

    void print(int indent = 0) const override {
        printIndent(indent);
        printf("Variable: %s\n", identifier_.c_str());
    }

private:
    std::string identifier_;
};

class AstLiteral : public AstNode {
public:
    AstLiteral(Span span, Token token)
        : AstNode(span), token_(std::move(token)) {}
    const Token& token() const { return token_; }

    void print(int indent = 0) const override {
        printIndent(indent);
        printf("Literal: %s\n", token_.lexeme().c_str());
    }

private:
    Token token_;
};

class AstBinaryExpr : public AstNode {
public:
    AstBinaryExpr(Span span, std::unique_ptr<AstNode> left, std::unique_ptr<AstNode> right, Token op)
        : AstNode(span), left_(std::move(left)), right_(std::move(right)), op_(std::move(op)) {}

    const AstNode* left() const { return left_.get(); }
    const AstNode* right() const { return right_.get(); }
    const Token& op() const { return op_; }

    void print(int indent = 0) const override {
        printIndent(indent);
        printf("BinaryExpr (%s):\n", op_.lexeme().c_str());
        left_->print(indent + 1);
        right_->print(indent + 1);
    }

private:
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

    void print(int indent = 0) const override {
        printIndent(indent);
        printf("Assignment (%s):\n", identifier_.c_str());
        value_->print(indent + 1);
    }

private:
    std::string identifier_;
    std::unique_ptr<AstNode> value_;
};
