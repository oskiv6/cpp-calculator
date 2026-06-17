#pragma once

#include <vector>
#include <string>
#include "Token.hpp"

class Lexer {
public:
    Lexer() = default;
    
    std::vector<Token> tokenize(const std::string& input);

private:
    std::string input_;
    size_t pos_ = 0;
    uint start_ = 0;

    char peek() const;
    char advance();
    bool isAtEnd() const;
    void skipWhitespace();
    
    Token makeToken(TokenType type, std::string lexeme);
};