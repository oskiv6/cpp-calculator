#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "core/Lexer.hpp"
#include "core/Parser.hpp"
#include "core/Ast.hpp"
#include "interpreter/Interpreter.hpp"

struct Config {
    bool debug = false;
    std::string filename = "";
};

void processInput(const std::string& input, Lexer& lexer, Interpreter& interpreter, bool debug) {
    if (input.empty()) return;

    try {
        auto tokens = lexer.tokenize(input);
        Parser parser(tokens);
        auto root = parser.parse();

        if (root) {
            if (debug) {
                std::cout << "--- AST Structure ---" << std::endl;
                root->print();
                std::cout << "---------------------" << std::endl;
            }
            
            double result = interpreter.evaluate(root.get());
            std::cout << "Result: " << result << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void runRepl(Lexer& lexer, Interpreter& interpreter, bool debug) {
    std::string line;
    std::cout << "Cpp-Calculator REPL (Type 'exit' to quit)" << std::endl;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line) || line == "exit") {
            break;
        }
        processInput(line, lexer, interpreter, debug);
    }
}

void runFile(const std::string& filename, Lexer& lexer, Interpreter& interpreter, bool debug) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        processInput(line, lexer, interpreter, debug);
    }
}

int main(int argc, char* argv[]) {
    Config config;
    Lexer lexer;
    Interpreter interpreter;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-d") {
            config.debug = true;
        } else if (arg == "run" && i + 1 < argc) {
            config.filename = argv[++i];
        }
    }

    if (!config.filename.empty()) {
        runFile(config.filename, lexer, interpreter, config.debug);
    } else {
        runRepl(lexer, interpreter, config.debug);
    }

    return 0;
}