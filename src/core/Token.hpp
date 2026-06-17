#pragma once

#include <string>

#include "../util/Util.hpp"

// ====

enum class TokenType {
    TK_LIT_NUM,     // 0-9
    TK_IDENTIFIER,  // abc

    TK_EQ,          // =
    TK_PLUS,        // +
    TK_MINUS,       // -
    TK_ASTERISK,    // *
    TK_SLASH,       // /
    TK_PERCENT,     // %

    TK_EOF
};

class Token {
public:
    Token(TokenType type, std::string lexeme, Span span)
        : type_(type), lexeme_(std::move(lexeme)), span_(span) {}

    TokenType type() const { return type_; }
    const std::string& lexeme() const { return lexeme_; }
    Span span() const { return span_; }

private:
    TokenType type_;
    std::string lexeme_;
    Span span_;

};
