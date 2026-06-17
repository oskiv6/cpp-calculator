#pragma once

#include <string>
#include <vector>

#include "Token.hpp"

// ====


class Pipeline {
private:
    // prcoessed file
    std::string content;

    std::vector<Token> tokens = std::vector<Token>();

public:

    Pipeline(std::string path);
    ~Pipeline();

    void run();

};
