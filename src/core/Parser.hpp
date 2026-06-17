#pragma once

#include <vector>
#include <memory>
#include "Token.hpp"
#include "Ast.hpp"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    std::unique_ptr<AstNode> parse();

private:
    std::vector<Token> tokens_;
    size_t pos_ = 0;

    std::unique_ptr<AstNode> parseAssignmentOrExpression();
    std::unique_ptr<AstNode> parseExpression();
    std::unique_ptr<AstNode> parseTerm();
    std::unique_ptr<AstNode> parseFactor();
    std::unique_ptr<AstNode> parsePrimary();

    bool match(TokenType type);
    Token advance();
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    Token consume(TokenType type, const std::string& message);
};