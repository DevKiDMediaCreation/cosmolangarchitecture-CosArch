//
// Create by DevKiD 15.09.2023
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "./tokenization.hpp"

// Integrate Cosmolang Linker and Cosmolang Assembler ICL and ICA
std::string tokens_to_asm(const std::vector<Token> &tokens) {
    std::stringstream output;
    output << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token &token = tokens.at(i);
        if (token.type == TokenType::exit) {
            if (i + 1 < tokens.size() && tokens.at(i + 1).type == TokenType::int_lit) {
                if (i + 2 < tokens.size() && tokens.at(i + 2).type == TokenType::semi) {
                    output << "     mov rax, 60\n";
                    output << "     mov rdi, " << tokens.at(i + 1).value.value() << "\n";
                    output << "     syscall\n";
                }
            }
        }
    }

    return output.str();
}

int main(int argc, char *argv[]) {
    std::cout << "Running Cosarch v0.0.1. Cosmolang Architecture Programming Language Research Labs" << std::endl;
    if (argc != 2) { // < 2
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "cosarch <input.cos>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();

        if (contents.empty()) {
            std::cerr << "File is empty." << std::endl;
            return EXIT_FAILURE;
        }
    }

    system("rm -fr ../data && mkdir -p ../data");

    /**if (argv[2] != nullptr) {
        std::string command = "rm -fr " + std::string(argv[2]) + " && mkdir -p " + std::string(argv[2]);
        system(command.c_str());
    } else {
        system("rm -fr ../data && mkdir -p ../data");
    }*/

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();
    {
        /**if (argv[2] != nullptr) {
            std::fstream file(std::string(argv[2]) + "/out.asm", std::ios::out);
            file << tokens_to_asm(tokens);
        } else {
            std::fstream file("../data/out.asm", std::ios::out);
            file << tokens_to_asm(tokens);
        }*/
        std::fstream file("../data/out.asm", std::ios::out);
        file << tokens_to_asm(tokens);
    }

    system("nasm -f elf64 ../data/out.asm -o ../data/out.o && ld ../data/out.o -o ../data/out");

    // For later
    /**if(argv[3] == "--debug") {
        std::cout << tokens_to_asm(tokens) << std::endl;
    }*/

    /**if (argv[2] != nullptr) {
        std::string command =
                "nasm -f elf64 " + std::string(argv[2]) + " /out.asm -o " + std::string(argv[2]) + "/out.o && ld " +
                std::string(argv[2]) + "/out.o -o " + std::string(argv[2]) + "/out";
        system(command.c_str());
    } else {
        system("nasm -f elf64 ../data/out.asm -o ../data/out.o && ld ../data/out.o -o ../data/out");
    }*/

    /**if(argv[2] == "-r") {
        system("../data/out && echo $?");
    }*/ // Add a -r run option to run the program after compiling

    std::cout << "Compiling successfully. File build: " << argv[1] << std::endl;

    /**if (argv[3] == "-r") {
        if (argv[2] == nullptr) {
            system("../data/out && echo $?");
        } else {
        }
    }*/

    return EXIT_SUCCESS;
}

// 28:51