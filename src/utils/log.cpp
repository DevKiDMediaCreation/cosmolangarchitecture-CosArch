#include "log.hpp"

#include <iostream>
#include <unordered_map>


std::unordered_map<int, std::string> Log::error_codes = {
        {101, "Invalid token"},
        {102, "Syntax error"},
        {103, "Undefined variable"},
        {1029, "Char parsing error."},
        {1948, "Invalid expression. Console Usage Error"},
        {2054, "Empty File"},
        {2301, "Invalid Program"},
        {2302, "Invalid statement"},
        {3956, "Expected expression. Paren Expression Error."},
        {4568, "Invalid expression. Exit-Code Paran Expression Error"},
        {4569, "Invalid expression. Ident Error"},
        {4570, "Undeclared identifier"},
        {4571, "Identifier already used"},
        {9983, "Unable to parse expression"},
        {9984, "Unreachable: Invalid Binary Expression"}
};

void Log::error(const std::string& msg) {

    std::cerr << "Error: " << msg << std::endl;
    exit(EXIT_FAILURE);
}

void Log::error(const int code) {

    if (auto it = error_codes.find(code); it != error_codes.end()) {
        std::cerr << "Error code " << code << ": " << it->second << std::endl;
        exit(code);
    }

    std::cerr << "Unknown error code: " << code << std::endl;
    exit(EXIT_FAILURE);
}

void Log::error(const int code, const std::string& additionalMsg) {
    //system("ipl -i -c --exit");

    if (auto it = error_codes.find(code); it != error_codes.end()) {
        std::cerr << "Error code " << code << ": " << it->second << ". " << additionalMsg << std::endl;
        exit(code);
    }

    std::cerr << "Unknown error code: " << code << ". " << additionalMsg << std::endl;
    exit(EXIT_FAILURE);
}