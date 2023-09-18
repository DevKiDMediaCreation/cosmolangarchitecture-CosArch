//
// Create by DevKiD 15.09.2023
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "./parser.hpp"
#include "./tokenization.hpp"
#include "./generation.hpp"


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

    // Integrate Cosmolang Linker and Cosmolang Assembler ICL and ICA
    Parser parser(std::move(tokens));
    std::optional<NodeExit> tree = parser.parse();
    if (!tree.has_value()) {
        std::cerr << "No exit statement found" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());
    {
        /**if (argv[2] != nullptr) {
            std::fstream file(std::string(argv[2]) + "/out.asm", std::ios::out);
            file << tokens_to_asm(tokens);
        } else {
            std::fstream file("../data/out.asm", std::ios::out);
            file << tokens_to_asm(tokens);
        }*/
        std::fstream file("../data/out.asm", std::ios::out);
        file << generator.generate();
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

// Create by DevKiD 15.09.2023