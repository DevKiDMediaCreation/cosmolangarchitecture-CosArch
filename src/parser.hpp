#pragma once

#include <variant>

#include "arena.hpp"
#include "tokenization.hpp"


struct NodeTermIntLit {
    Token int_lit;
};

struct NodeTermIdent {
    Token ident;
};

struct NodeExpr;

struct NodeTermParen {
    NodeExpr *expr;
};

struct NodeBinExprAdd {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprMulti {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprSub {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExprDiv {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct NodeBinExpr {
    std::variant<NodeBinExprAdd *, NodeBinExprMulti *, NodeBinExprSub *, NodeBinExprDiv *> var;
};

struct NodeTerm {
    std::variant<NodeTermIntLit *, NodeTermIdent *, NodeTermParen *> var;
};

struct NodeExpr {
    std::variant<NodeTerm *, NodeBinExpr *> var;
};

struct NodeStmtExit {
    NodeExpr *expr;
};

struct NodeStmtLet {
    Token ident;
    NodeExpr *expr;
};

struct NodeStmt;

struct NodeStmtScope {
    std::vector<NodeStmt *> stmts;
};

struct NodeStmt {
    std::variant<NodeStmtExit *, NodeStmtLet *, NodeStmtScope *> var;
};

struct NodeProg {
    std::vector<NodeStmt *> stmts;
};

class Parser {
public:
    inline explicit Parser(std::vector<Token> tokens)
            : m_tokens(std::move(tokens)), m_allocator(1024 * 1024 * 4) // 4 mb
    {
    }

    std::optional<NodeTerm *> parse_term() {
        if (auto int_lit = try_consume(TokenType::int_lit)) {
            auto term_int_lit = m_allocator.alloc<NodeTermIntLit>();
            term_int_lit->int_lit = int_lit.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_int_lit;
            return term;
        } else if (auto ident = try_consume(TokenType::ident)) {
            auto expr_ident = m_allocator.alloc<NodeTermIdent>();
            expr_ident->ident = ident.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = expr_ident;
            return term;
        } else if (auto open_paren = try_consume(TokenType::open_paren)) {
            auto expr = parse_expr();
            if (!expr.has_value()) {
                std::cerr << "Expected expression. Paren Expression Error. Error-Code: #3956" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected `)`");
            auto term_paren = m_allocator.alloc<NodeTermParen>();
            term_paren->expr = expr.value();
            auto term = m_allocator.alloc<NodeTerm>();
            term->var = term_paren;
            return term;
        } else {
            return {};
        }
    }

    std::optional<NodeExpr *> parse_expr(int min_prec = 0) { // prec = precendence
        std::optional<NodeTerm *> term_lhs = parse_term();
        if (!term_lhs.has_value()) {
            return {};
        }
        auto expr_lhs = m_allocator.alloc<NodeExpr>();
        expr_lhs->var = term_lhs.value();

        while (true) {
            std::optional<Token> curr_tok = peek();
            std::optional<int> prec;

            if (curr_tok.has_value()) {
                prec = bin_prec(curr_tok->type);
                if (!prec.has_value() || prec < min_prec) {
                    break;
                }
            } else {
                break;
            }

            Token op = consume();
            int next_min_prec = prec.value() + 1;
            auto expr_rhs = parse_expr(next_min_prec);
            if (!expr_rhs.has_value()) {
                std::cerr << "Unable to parse expression. Error-Code: #9983" << std::endl;
                exit(EXIT_FAILURE);
            }

            auto expr = m_allocator.alloc<NodeBinExpr>();
            auto expr_lhs_cache = m_allocator.alloc<NodeExpr>();
            if (op.type == TokenType::plus) {
                auto add = m_allocator.alloc<NodeBinExprAdd>();
                expr_lhs_cache->var = expr_lhs->var;
                add->lhs = expr_lhs_cache;
                add->rhs = expr_rhs.value();
                expr->var = add;
            } else if (op.type == TokenType::minus) {
                auto sub = m_allocator.alloc<NodeBinExprSub>();
                expr_lhs_cache->var = expr_lhs->var;
                sub->lhs = expr_lhs_cache;
                sub->rhs = expr_rhs.value();
                expr->var = sub;
            } else if (op.type == TokenType::star) {
                auto multi = m_allocator.alloc<NodeBinExprMulti>();
                expr_lhs_cache->var = expr_lhs->var;
                multi->lhs = expr_lhs_cache;
                multi->rhs = expr_rhs.value();
                expr->var = multi;
            } else if (op.type == TokenType::div) {
                auto div = m_allocator.alloc<NodeBinExprDiv>();
                expr_lhs_cache->var = expr_lhs->var;
                div->lhs = expr_lhs_cache;
                div->rhs = expr_rhs.value();
                expr->var = div;
            } else {
                // Unreachable
                std::cerr << "Unreachable: Invalid Binary Expression. Error-Code: #9984" << std::endl;
                exit(EXIT_FAILURE);
            }
            expr_lhs->var = expr;
        }
        return expr_lhs;
    }

    std::optional<NodeStmt *> parse_stmt() {
        if (peek().value().type == TokenType::exit && peek(1).has_value()
            && peek(1).value().type == TokenType::open_paren) {
            consume();
            consume();
            auto stmt_exit = m_allocator.alloc<NodeStmtExit>();
            if (auto node_expr = parse_expr()) {
                stmt_exit->expr = node_expr.value();
            } else {
                std::cerr << "Invalid expression. Exit-Code Paran Expression Error. Error-Code: # 4568" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::close_paren, "Expected `)`");
            try_consume(TokenType::semi, "Expected `;`");
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_exit;
            return stmt;
        } else if (
                peek().has_value() && peek().value().type == TokenType::let && peek(1).has_value()
                && peek(1).value().type == TokenType::ident && peek(2).has_value()
                && peek(2).value().type == TokenType::eq) {
            consume();
            auto stmt_let = m_allocator.alloc<NodeStmtLet>();
            stmt_let->ident = consume();
            consume();
            if (auto expr = parse_expr()) {
                stmt_let->expr = expr.value();
            } else {
                std::cerr << "Invalid expression. Ident Error. Error-Code: #4569" << std::endl;
                exit(EXIT_FAILURE);
            }
            try_consume(TokenType::semi, "Expected `;`");
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_let;
            return stmt;
        }
        else if (auto open_curly = try_consume(TokenType::open_curly)) {
            auto stmt_scope = m_allocator.alloc<NodeStmtScope>();
            while (peek().has_value() && peek().value().type != TokenType::close_curly) {
                if (auto stmt = parse_stmt()) {
                    stmt_scope->stmts.push_back(stmt.value());
                } else {
                    Log::error(2302, "Unclear statement. Probably accessing a variable that is not declared in the scope of the statement.");
                }
            }
            try_consume(TokenType::close_curly, "Expected `}`");
            auto stmt = m_allocator.alloc<NodeStmt>();
            stmt->var = stmt_scope;
            return stmt;
        }

        else {
            return {};
        }
    }

    std::optional<NodeProg> parse_prog() {
        NodeProg prog;
        while (peek().has_value()) {
            if (auto stmt = parse_stmt()) {
                prog.stmts.push_back(stmt.value());
            } else {
                Log::error(2302);
            }
        }
        return prog;
    }

private:
    [[nodiscard]] inline std::optional<Token> peek(int offset = 0) const {
        if (m_index + offset >= m_tokens.size()) {
            return {};
        } else {
            return m_tokens.at(m_index + offset);
        }
    }

    inline Token consume() {
        return m_tokens.at(m_index++);
    }

    inline Token try_consume(TokenType type, const std::string &err_msg) {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        } else {
            std::cerr << err_msg << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    inline std::optional<Token> try_consume(TokenType type) {
        if (peek().has_value() && peek().value().type == type) {
            return consume();
        } else {
            return {};
        }
    }

    const std::vector<Token> m_tokens;
    size_t m_index = 0;
    ArenaAllocator m_allocator;
};