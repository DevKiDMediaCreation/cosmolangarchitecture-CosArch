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
        std::fstream file("../data/out.asm", std::ios::out);
        file << generator.generate();
    }

    system("nasm -f elf64 ../data/out.asm -o ../data/out.o && ld ../data/out.o -o ../data/out");

    std::cout << "Compiling successfully. File build: " << argv[1] << std::endl;

    return EXIT_SUCCESS;
}

// Create by DevKiD 15.09.2023