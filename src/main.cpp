#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>
#include <chrono>

#include "./tokenization.hpp"
#include "./parser.hpp"
#include "./generation.hpp"
#include "./utils/log.hpp"

int main(int argc, char *argv[]) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Add ipl

    if (argc != 2) {
        std::cerr << "Incorrect usage. Correct usage is..." << std::endl;
        std::cerr << "cosmolingua <input.cl>" << std::endl;
        Log::error(1948);
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::stringstream contents_stream;
        std::fstream input(argv[1], std::ios::in);
        contents_stream << input.rdbuf();
        contents = contents_stream.str();
        if (contents.empty()) {
            Log::error(2054);
            return EXIT_FAILURE;
        }
    }
    std::cout << "Reading successfully." << std::endl;

    Tokenizer tokenizer(std::move(contents));
    std::vector<Token> tokens = tokenizer.tokenize();
    std::cout << "AST and Tokenization successfully." << std::endl;

    // Integrate Cosmolang Linker and Cosmolang Assembler ICL and ICA
    Parser parser(std::move(tokens));
    std::optional<NodeProg> prog = parser.parse_prog();
    std::cout << "Parsing successfully." << std::endl;

    if (!prog.has_value()) {
        Log::error(2301);
        exit(EXIT_FAILURE);
    }

    Generator generator(prog.value());
    {
        std::fstream file("output.asm", std::ios::out);
        file << generator.gen_prog();
    }
    std::cout << "Generation successfully." << std::endl;

    // Later add Integrate Cosmolang Linker and Cosmolang Assembler ICL and ICA And ICO (Integrate Cosmolang Object)
    system("nasm -f elf64 output.asm -o output.o && ld output.o -o output");
    std::cout << "Linking and Assembling successfully. (Build)" << std::endl;


    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Compilation Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "ms" << std::endl;

    return 0;
}
