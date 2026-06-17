#include "Lexer.hpp"
#include <cctype>

std::vector<Token> Lexer::tokenize(const std::string& input) {
    input_ = input;
    pos_ = 0;
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;

        start_ = (uint)pos_;
        char c = advance();

        if (std::isdigit(c)) {
            while (std::isdigit(peek())) advance();
            if (peek() == '.' && std::isdigit(input_[pos_ + 1])) {
                advance(); // consume '.'
                while (std::isdigit(peek())) advance();
            }
            tokens.push_back(makeToken(TokenType::TK_LIT_NUM, input_.substr(start_, pos_ - start_)));
        } else if (std::isalpha(c)) {
            while (std::isalnum(peek())) advance();
            tokens.push_back(makeToken(TokenType::TK_IDENTIFIER, input_.substr(start_, pos_ - start_)));
        } else {
            switch (c) {
                case '=': tokens.push_back(makeToken(TokenType::TK_EQ, "=")); break;
                case '+': tokens.push_back(makeToken(TokenType::TK_PLUS, "+")); break;
                case '-': tokens.push_back(makeToken(TokenType::TK_MINUS, "-")); break;
                case '*': tokens.push_back(makeToken(TokenType::TK_ASTERISK, "*")); break;
                case '/': tokens.push_back(makeToken(TokenType::TK_SLASH, "/")); break;
                case '%': tokens.push_back(makeToken(TokenType::TK_PERCENT, "%")); break;
                case '(': tokens.push_back(makeToken(TokenType::TK_LPAREN, "(")); break;
                case ')': tokens.push_back(makeToken(TokenType::TK_RPAREN, ")")); break;
                default: break; // Ignore unknown characters for now
            }
        }
    }

    tokens.push_back(makeToken(TokenType::TK_EOF, ""));
    return tokens;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return input_[pos_];
}

char Lexer::advance() {
    return input_[pos_++];
}

bool Lexer::isAtEnd() const {
    return pos_ >= input_.length();
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek())) {
        advance();
    }
}

Token Lexer::makeToken(TokenType type, std::string lexeme) {
    return Token(type, std::move(lexeme), {start_, (uint)pos_});
}