#pragma once

#include "parser.hpp"
#include <cassert>

class Generator {
public:
    inline explicit Generator(NodeProg prog)
            : m_prog(std::move(prog)) {
    }

    void gen_term(const NodeTerm *term) {
        struct TermVisitor {
            Generator *gen;

            void operator()(const NodeTermIntLit *term_int_lit) const {
                gen->m_output << "\tmov rax, " << term_int_lit->int_lit.value.value() << "\n";
                gen->push("rax");
                Log::addProcess("Integer Literal: " + term_int_lit->int_lit.value.value());
            }

            void operator()(const NodeTermIdent *term_ident) const {
                auto it = std::ranges::find_if(gen->m_vars.cbegin(), gen->m_vars.cend(), [&](const Var &var) {
                    return var.name == term_ident->ident.value.value();
                });
                if (it == gen->m_vars.cend()) {
                    Log::error(4570, "Identifier: " + term_ident->ident.value.value());
                }
                std::stringstream offset;
                offset << "QWORD [rsp + " << (gen->m_stack_size - it->stack_loc - 1) * 8 << "]";
                gen->push(offset.str());

                Log::addProcess("Identifier: " + term_ident->ident.value.value());
            }

            void operator()(const NodeTermParen *term_paren) const {
                gen->gen_expr(term_paren->expr);
            }
        };
        TermVisitor visitor({.gen = this});
        std::visit(visitor, term->var);
    }

    void gen_bin_expr(const NodeBinExpr *bin_expr) {
        struct BinExprVisitor {
            Generator *gen;

            void operator()(const NodeBinExprAdd *add) const {
                gen->gen_expr(add->rhs);
                gen->gen_expr(add->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tadd rax, rbx\n";
                gen->push("rax");
                Log::addProcess("Addition with RAX and RBX in " + std::to_string(add->lhs->var.index()) + " and " +
                                std::to_string(add->rhs->var.index()));
            }

            void operator()(const NodeBinExprSub *sub) const {
                gen->gen_expr(sub->rhs);
                gen->gen_expr(sub->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tsub rax, rbx\n";
                gen->push("rax");
                Log::addProcess("Subtraction with RAX and RBX in " + std::to_string(sub->lhs->var.index()) + " and " +
                                std::to_string(sub->rhs->var.index()));
            }


            void operator()(const NodeBinExprMulti *multi) const {
                gen->gen_expr(multi->rhs);
                gen->gen_expr(multi->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tmul rbx\n";
                gen->push("rax");
                Log::addProcess(
                        "Multiplication with RAX and RBX in " + std::to_string(multi->lhs->var.index()) + " and " +
                        std::to_string(multi->rhs->var.index()));
            }

            void operator()(const NodeBinExprDiv *div) const {
                gen->gen_expr(div->rhs);
                gen->gen_expr(div->lhs);
                gen->pop("rax");
                gen->pop("rbx");
                gen->m_output << "\tdiv rbx\n";
                gen->push("rax");
                Log::addProcess("Division with RAX and RBX in " + std::to_string(div->lhs->var.index()) + " and " +
                                std::to_string(div->rhs->var.index()));
            }
        };

        BinExprVisitor visitor{.gen = this};
        std::visit(visitor, bin_expr->var);
    }

    void gen_expr(const NodeExpr *expr) {
        struct ExprVisitor {
            Generator *gen;

            void operator()(const NodeTerm *term) const {
                gen->gen_term(term);
            }

            void operator()(const NodeBinExpr *bin_expr) const {
                gen->gen_bin_expr(bin_expr);
            }
        };

        ExprVisitor visitor{.gen = this};
        std::visit(visitor, expr->var);
    }

    void gen_scope(const NodeScope *scope) {
        begin_scope();
        for (const NodeStmt *stmt: scope->stmts) {
            gen_stmt(stmt);
        }
        end_scope();
        Log::addProcess("Scope");
    }

    void gen_stmt(const NodeStmt *stmt) {
        struct StmtVisitor {
            Generator *gen;

            void operator()(const NodeStmtExit *stmt_exit) const {
                gen->gen_expr(stmt_exit->expr);
                gen->m_output << "\tmov rax, 60\n";
                gen->pop("rdi");
                gen->m_output << "\tsyscall\n";
                Log::addProcess("Exit with RDI");
            }

            void operator()(const NodeStmtLet *stmt_let) const {
                auto it = std::ranges::find_if(gen->m_vars.cbegin(), gen->m_vars.cend(), [&](const Var &var) {
                    return var.name == stmt_let->ident.value.value();
                });
                if (it != gen->m_vars.cend()) {
                    Log::error(4571, "Identifier: " + stmt_let->ident.value.value());
                }

                gen->m_vars.push_back({.name = stmt_let->ident.value.value(), .stack_loc = gen->m_stack_size});
                gen->gen_expr(stmt_let->expr);
                Log::addProcess("Let Identifier: " + stmt_let->ident.value.value());
            }

            void operator()(const NodeScope *scope) const {
                gen->gen_scope(scope);
            }

            void operator()(const NodeStmtIf *stmt_if) const {
                gen->gen_expr(stmt_if->expr);
                gen->pop("rax");
                std::string label = gen->create_label();
                gen->m_output << "\ttest rax, rax\n";
                gen->m_output << "\tjz " << label << "\n";
                gen->gen_scope(stmt_if->scope);
                gen->m_output << label << ":\n";

                Log::addProcess("If Statement of " + std::to_string(stmt_if->expr->var.index()));
            }
        };

        StmtVisitor visitor{.gen = this};
        std::visit(visitor, stmt->var);
    }

    [[nodiscard]] std::string gen_prog() {
        m_output << "global _start\n_start:\n";

        for (const NodeStmt *stmt: m_prog.stmts) {
            gen_stmt(stmt);
        }

        m_output << "\tmov rax, 60\n";
        m_output << "\tmov rdi, 0\n";
        m_output << "\tsyscall\n";
        return m_output.str();
    }

private:
    void push(const std::string &reg) {
        m_output << "\tpush " << reg << "\n";
        m_stack_size++;
        Log::addProcess("Stack Size: " + std::to_string(m_stack_size) + " Register: " + reg);
    }

    void pop(const std::string &reg) {
        m_output << "\tpop " << reg << "\n";
        m_stack_size--;
        Log::addProcess("Stack Size: " + std::to_string(m_stack_size) + " Register: " + reg);
    }

    void begin_scope() {
        m_scopes.push_back(m_vars.size());
        Log::addProcess("Scope Size: " + std::to_string(m_vars.size()) + ". Begin Scope.");
    }

    void end_scope() {
        size_t scope_size = m_vars.size() - m_scopes.back();
        m_output << "\tadd rsp, " << scope_size * 8 << "\n";
        m_stack_size -= scope_size;
        //m_vars.resize(m_scopes.back());
        for (int i = 0; i < scope_size; i++) {
            m_vars.pop_back();
        }
        m_scopes.pop_back();
        Log::addProcess("Scope Size: " + std::to_string(m_vars.size()) + ". End Scope.");
    }

    std::string create_label() {
        return ".L" + std::to_string(m_label_count++);
    }

    struct Var {
        std::string name;
        size_t stack_loc;
    };
    const NodeProg m_prog;
    std::stringstream m_output;
    size_t m_stack_size = 0;
    std::vector<Var> m_vars{};
    std::vector<size_t> m_scopes{};
    int m_label_count = 0;
};