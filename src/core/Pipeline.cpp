#include "Pipeline.h"

#include "Lexer.hpp"
#include "Parser.hpp"

#include <fstream>
#include <memory>

// ====

std::string __read_contents(std::string path);

Pipeline::Pipeline(std::string path) {

    content = __read_contents(path);

}

Pipeline::~Pipeline() {

}

void Pipeline::run() {

    Lexer lexer;
    auto tokens = lexer.tokenize(content);
    Parser parser(tokens);
    std::unique_ptr<AstNode> root = parser.parse();

}


std::string __read_contents(std::string path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}
