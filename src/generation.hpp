#pragma once

#include "parser.hpp"

#include <string>
#include <sstream>
#include <unordered_map>
#include <cassert>

class Generator {
public:
    inline explicit Generator(NodeProg prog) : m_prog(std::move(prog)) {

    }

    void gen_term(const NodeTerm *term) {
        struct TermVisitor {
            Generator *gen;

            void operator()(const NodeTermIntLit *term_int_lit) const {
                gen->m_output << "\tmov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen->push("rax");
            }

            void operator()(const NodeTermIdent *term_ident) const {
                if (!gen->m_vars.contains(term_ident->ident.value.value())) {
                    std::cerr << "Undeclared identifier. Identifier not found: " << term_ident->ident.value.value()
                              << std::endl;
                    exit(EXIT_FAILURE);
                }
                const auto &var = gen->m_vars.at(term_ident->ident.value.value());
                std::stringstream offset;
                // QWORD for 64-bit - *64, DWORD for 32-bit - *32, WORD for 16-bit - *16, BYTE for 8-bit - *8
                gen->m_output << "\t; Call var: " << term_ident->ident.value.value() << "\n";
                offset << "QWORD [rsp + " << (gen->m_stack_size - var.stack_loc - 1) * 8 << "]";
                gen->push(offset.str());
            }
        };
        TermVisitor visitor({.gen = this});
        std::visit(visitor, term->var);
    }

    void gen_expr(const NodeExpr *expr) {
        struct ExprVisitor {
            Generator *gen;

            void operator()(const NodeTerm *term) const {
                gen->gen_term(term);
            }

            void operator()(const NodeBinExpr *bin_expr) const {
                gen->gen_expr(bin_expr->add->lhs);
                gen->gen_expr(bin_expr->add->rhs);
                gen->m_output << "\t; Add\n";
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tadd rax, rbx\n";
                gen->push("rax");
            }
        };
        ExprVisitor visitor{.gen = this};
        std::visit(visitor, expr->var);
    }

    // Exit syscall by var
    void gen_stmt(const NodeStmt *stmt) {
        struct StmtVisitor {
            Generator *gen;

            void operator()(const NodeStmtExit *stmt_exit) const {
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "\t; Exit by var.\n";
                gen->m_output << "\tmov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "\tsyscall\n";
            }

            void operator()(const NodeStmtLet *stmt_let) const {
                if (gen->m_vars.contains(stmt_let->ident.value.value())) {
                    std::cerr << "Identifier already used: " << stmt_let->ident.value.value() << std::endl;
                    exit(EXIT_FAILURE);
                }
                gen->m_vars.insert({stmt_let->ident.value.value(), Var{.stack_loc = gen->m_stack_size}});
                gen->gen_expr(stmt_let->expr);
            }

            void operator()(const NodeBinExpr *bin_expr) const {
                assert(false);
            }
        };

        StmtVisitor visitor{.gen = this};
        std::visit(visitor, stmt->var);
    }

    // Default exit syscall
    [[nodiscard]] std::string gen_prog() {
        m_output << "global _start\n_start:\n";

        for (const NodeStmt *stmt: m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "\tmov rax, 60\n";
        m_output << "\tmov rdi, 0\n";
        m_output << "\tsyscall\n";
        std::cout << "Executable code generation successfully is done." << std::endl;

        std::cout << "Total size of output: " << m_output.str().size() << std::endl;
        // Get the number of lines
        // std::cout << "Total lines of output: " << std::count(m_output.str().begin(), m_output.str().end(), '\n')
        //          << std::endl;

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