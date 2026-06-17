#include "Pipeline.h"

#include <fstream>

// ====

std::string __read_contents(std::string path);

Pipeline::Pipeline(std::string path) {

    content = __read_contents(path);

}


void Pipeline::run() {



}


std::string __read_contents(std::string path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}
