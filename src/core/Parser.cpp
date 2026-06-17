#include "Parser.hpp"
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : tokens_(std::move(tokens)) {}

std::unique_ptr<AstNode> Parser::parse() {
    if (tokens_.empty()) return nullptr;
    return parseAssignmentOrExpression();
}

std::unique_ptr<AstNode> Parser::parseAssignmentOrExpression() {
    // Check for assignment: identifier = expr
    if (tokens_.size() > pos_ + 1 && 
        tokens_[pos_].type() == TokenType::TK_IDENTIFIER && 
        tokens_[pos_ + 1].type() == TokenType::TK_EQ) {
        
        Token name = advance(); // identifier
        advance(); // =
        auto value = parseExpression();
        
        Span span = { name.span().start, value->span().end };
        return std::make_unique<AstAssignment>(span, name.lexeme(), std::move(value));
    }

    return parseExpression();
}

std::unique_ptr<AstNode> Parser::parseExpression() {
    auto left = parseTerm();

    while (match(TokenType::TK_PLUS) || match(TokenType::TK_MINUS)) {
        Token op = previous();
        auto right = parseTerm();
        Span span = { left->span().start, right->span().end };
        left = std::make_unique<AstBinaryExpr>(span, std::move(left), std::move(right), op);
    }

    return left;
}

std::unique_ptr<AstNode> Parser::parseTerm() {
    auto left = parseFactor();

    while (match(TokenType::TK_ASTERISK) || match(TokenType::TK_SLASH) || match(TokenType::TK_PERCENT)) {
        Token op = previous();
        auto right = parseFactor();
        Span span = { left->span().start, right->span().end };
        left = std::make_unique<AstBinaryExpr>(span, std::move(left), std::move(right), op);
    }

    return left;
}

std::unique_ptr<AstNode> Parser::parseFactor() {
    return parsePrimary();
}

std::unique_ptr<AstNode> Parser::parsePrimary() {
    if (match(TokenType::TK_LIT_NUM)) {
        return std::make_unique<AstLiteral>(previous().span(), previous());
    }

    if (match(TokenType::TK_IDENTIFIER)) {
        return std::make_unique<AstVar>(previous().span(), previous().lexeme());
    }

    throw std::runtime_error("Expect expression.");
}

bool Parser::match(TokenType type) {
    if (peek().type() == type) {
        advance();
        return true;
    }
    return false;
}

Token Parser::advance() {
    if (!isAtEnd()) pos_++;
    return previous();
}

Token Parser::peek() const {
    if (isAtEnd()) return tokens_.back();
    return tokens_[pos_];
}

Token Parser::previous() const {
    return tokens_[pos_ - 1];
}

bool Parser::isAtEnd() const {
    return pos_ >= tokens_.size() || tokens_[pos_].type() == TokenType::TK_EOF;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (peek().type() == type) return advance();
    throw std::runtime_error(message);
}