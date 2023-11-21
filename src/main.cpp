//
// Create by DevKiD 15.09.2023
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>
#include <chrono>


#include "./generation.hpp"

int main(int argc, char *argv[]) {

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Running Cosarch v0.0.1. Cosmolang Architecture Programming Language Research Labs" << std::endl;
    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "cosarch <input.cos>" << std::endl;
        return EXIT_FAILURE;
    }

    // READ FILE
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

    if (!system("rm -fr ../data && mkdir -p ../data")) {} // Why? Because the IDE doesn't like only the system()

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();

    // Integrate Cosmolang Linker and Cosmolang Assembler ICL and ICA
    Parser parser(std::move(tokens));
    std::optional<NodeProg> prog = parser.parse_prog();
    if (!prog.has_value()) {
        std::cerr << "Invalid program." << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());
    {
        std::fstream file("../data/out.asm", std::ios::out);
        file << generator.gen_prog();
    }

    if (system("nasm -f elf64 ../data/out.asm -o ../data/out.o && ld ../data/out.o -o ../data/out")) {}

    std::cout << "Compiling successfully. File build: " << argv[1] << std::endl;

    // Calculate the time
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Execution Time to Translate. Compiler Time: " << duration << "ms" << std::endl;

    return EXIT_SUCCESS;
}

// Create by DevKiD 15.09.2023