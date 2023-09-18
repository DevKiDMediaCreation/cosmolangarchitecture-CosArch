#pragma once

#include "parser.hpp"

class Generator {
public:
    inline explicit Generator(NodeExit root) : m_root(std::move(root)) {

    }

    [[nodiscard]] std::string generate() {
        std::stringstream output;
        output << "global _start\n_start:\n";
        output << "     mov rax, 60\n";
        output << "     mov rdi, " << m_root.expr.int_lit.value.value() << "\n";
        output << "     syscall\n";
        std::cout << "Executable code generation successfully is done." << std::endl;
        return output.str();
    }

private:
    const NodeExit m_root;
};