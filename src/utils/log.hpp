#pragma once

#include <iostream>
#include <unordered_map>
#include <optional>
#include <vector>
#include <ctime>


class Log {
public:
    static void error(const std::string& msg);
    static void error(const int code);
    static void error(const int code, const std::string& additionalMsg);

    static void add(const std::string& msg);
    static void addWarning(const std::string& msg);
    static void addInfo(const std::string& msg);
    static void addFatal(const std::string& msg);
    static void createFile();
    static void createFile(const int code);

private:
    struct _log {
        time_t time;
        std::string msg;
        std::string details;
        int code;
        int checkpoint;
        std::string type;
    };

    static std::unordered_map<int, std::string> error_codes;
    static std::vector<_log> warnings;
    static std::vector<_log> infos;
    static std::vector<_log> log;
    static std::vector<_log> fatal;
    static std::vector<_log> errors;


    static void addError(const std::string &msg, const int code, const std::string &details);
    static void generatingLog(const int code);
};