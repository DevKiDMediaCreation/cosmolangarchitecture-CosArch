#pragma once

#include <iostream>
#include <unordered_map>
#include <optional>

class Log {
public:
    static void error(const std::string& msg);
    static void error(const int code);
    static void error(const int code, const std::string& additionalMsg);

private:
    static std::unordered_map<int, std::string> error_codes;
};