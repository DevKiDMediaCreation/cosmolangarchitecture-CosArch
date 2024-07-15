#include "log.hpp"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <fstream>


std::unordered_map<int, std::string> Log::error_codes = {
        {12,   "Unknown Error"},
        {101,  "Invalid token"},
        {102,  "Syntax error"},
        {103,  "Undefined variable"},
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

std::vector<Log::_log> Log::warnings;
std::vector<Log::_log> Log::infos;
std::vector<Log::_log> Log::log;
std::vector<Log::_log> Log::fatal;
std::vector<Log::_log> Log::errors;

void Log::error(const std::string &msg) {
    std::cerr << "Error: " << msg << std::endl;
    addError("Error by String", EXIT_FAILURE, "Error msg: " + msg);
}

void Log::error(const int code) {

    if (auto it = error_codes.find(code); it != error_codes.end()) {
        std::cerr << "Error code " << code << ": " << it->second << std::endl;
        addError("Error code: ", code, it->second);
    }

    std::cerr << "Unknown error code: " << code << std::endl;
    addError("Unknown Error code", 12, "Unknown error code: " + std::to_string(code));
}

void Log::error(const int code, const std::string &additionalMsg) {
    //system("ipl -i -c --exit");

    if (auto it = error_codes.find(code); it != error_codes.end()) {
        std::cerr << "Error code " << code << ": " << it->second << ". " << additionalMsg << std::endl;
        addError(additionalMsg, code, it->second);
    }

    std::cerr << "Unknown error code: " << code << ". " << additionalMsg << std::endl;
    addError("Unknown Error code: " + std::to_string(code) + ". " + additionalMsg, 12, "Unknown error code: " + std::to_string(code));
}

void Log::add(const std::string &msg) {
    _log log_entry;
    log_entry.time = time(nullptr);
    log_entry.msg = msg;
    log_entry.type = "Log";
    log.push_back(log_entry);
}

void Log::addWarning(const std::string &msg) {
    _log log_entry;
    log_entry.time = time(nullptr);
    log_entry.msg = msg;
    log_entry.type = "Warning";
    warnings.push_back(log_entry);
}

void Log::addInfo(const std::string &msg) {
    _log log_entry;
    log_entry.time = time(nullptr);
    log_entry.msg = msg;
    log_entry.type = "Info";
    infos.push_back(log_entry);
}

void Log::addFatal(const std::string &msg) {
    _log log_entry;
    log_entry.time = time(nullptr);
    log_entry.msg = msg;
    log_entry.type = "Fatal";
    fatal.push_back(log_entry);
}

void Log::addError(const std::string &msg, const int code, const std::string &details) {
    _log log_entry;
    log_entry.time = time(nullptr);
    log_entry.msg = msg;
    log_entry.details = details;
    log_entry.code = code;
    log_entry.type = "Error";
    errors.push_back(log_entry);
    createFile(code);
}

std::string cTime(long long milliseconds) {
    std::time_t t = milliseconds / 1000;
    std::tm tm = *std::localtime(&t);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buffer);
}

void Log::generatingLog(const int code) {
    auto now = std::chrono::system_clock::now();
    auto now_ms = time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
    std::string filename = std::to_string(now_ms) + ".log";

    std::ofstream file(filename);
    file << "Log file generated at " << time(nullptr) << std::endl;
    file << "Warnings: " << warnings.size() << std::endl;
    file << "Infos: " << infos.size() << std::endl;
    file << "Error: " << errors.size() << std::endl;
    file << "Fatal: " << fatal.size() << std::endl;
    file << "Log: " << log.size() << std::endl;

    // Create a single vector and sort it by time
    std::vector<_log> all_logs;
    all_logs.insert(all_logs.end(), warnings.begin(), warnings.end());
    all_logs.insert(all_logs.end(), infos.begin(), infos.end());
    all_logs.insert(all_logs.end(), errors.begin(), errors.end());
    all_logs.insert(all_logs.end(), fatal.begin(), fatal.end());
    all_logs.insert(all_logs.end(), log.begin(), log.end());

    std::sort(all_logs.begin(), all_logs.end(), [](const _log &a, const _log &b) {
        return a.time < b.time;
    });

    for (const auto &log_entry: all_logs) {
        file << cTime(log_entry.time) << ": " << log_entry.type << ": " << log_entry.msg << std::endl;
    }

    file << "Exit code: " << code << std::endl;

    file.close();
    std::cout << "Log file generated at " << filename << std::endl;
}

void Log::createFile() {
    generatingLog(EXIT_SUCCESS);
    exit(EXIT_SUCCESS);
}

void Log::createFile(const int code) {
    generatingLog(code);
    exit(code);
} // Private