#pragma once

#include "parser.hpp"

#include <string>
#include <sstream>
#include <unordered_map>

class Generator {
public:
    inline explicit Generator(NodeProg prog) : m_prog(std::move(prog)) {

    }

    void gen_expr(const NodeExpr &expr) {
        struct ExprVisitor {
            Generator *gen;

            void operator()(const NodeExprIntLit &expr_int_lit) const {
                gen->m_output << "\tmov rax, " << expr_int_lit.int_lit.value.value() << "\n";
                gen->push("rax");
            }

            void operator()(const NodeExprIdent &expr_ident) const {
                if (!gen->m_vars.contains(expr_ident.ident.value.value())) {
                    std::cerr << "Undeclared identifier. Identifier not found: " << expr_ident.ident.value.value()
                              << std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto &var = gen->m_vars.at(expr_ident.ident.value.value());
                std::stringstream offset;
                // QWORD for 64-bit - *64, DWORD for 32-bit - *32, WORD for 16-bit - *16, BYTE for 8-bit - *8
                gen->m_output << "\t; Call var: " << expr_ident.ident.value.value() << "\n";
                offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc - 1) * 8 << "]";
                gen->push(offset.str());
            }
        };
        ExprVisitor visitor{.gen = this};
        std::visit(visitor, expr.var);
    }

    // Exit syscall by var
    void gen_stmt(const NodeStmt &stmt) {
        struct StmtVisitor {
            Generator *gen;

            void operator()(const NodeStmtExit &stmt_exit) const {
                gen->gen_expr(stmt_exit.expr);
                gen->m_output << "\t; Exit by var.\n";
                gen->m_output << "\tmov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "\tsyscall\n";
            }

            void operator()(const NodeStmtLet &stmt_let) const {
                if (gen->m_vars.contains(stmt_let.ident.value.value())) {
                    std::cerr << "Identifier already used: " << stmt_let.ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let.ident.value.value(), Var{.stack_loc = gen->m_stack_size}});
                gen->gen_expr(stmt_let.expr);
            }
        };

        StmtVisitor visitor{.gen = this};
        std::visit(visitor, stmt.var);
    }

    // Default exit syscall
    [[nodiscard]] std::string gen_prog() {
        m_output << "global _start\n_start:\n";

        for (const NodeStmt &stmt: m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "\tmov rax, 60\n";
        m_output << "\tmov rdi, 0\n";
        m_output << "\tsyscall\n";
        std::cout << "Executable code generation successfully is done." << std::endl;
        return m_output.str();
    }

private:

    void push(const std::string &reg) {
        m_output << "\tpush " << reg << "\n";
        m_stack_size++;
    }

    void pop(const std::string &reg) {
        m_output << "\tpop " << reg << "\n";
        m_stack_size--;
    }

    struct Var {
        size_t stack_loc;

    };

    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::unordered_map<std::string, Var> m_vars{};
};