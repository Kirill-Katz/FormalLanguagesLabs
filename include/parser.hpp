#pragma once

#include <memory>
#include <vector>

#include "ast.hpp"
#include "lexer.hpp"

class Parser {
public:
    explicit Parser(std::vector<Token> tokens)
    : tokens_(std::move(tokens)) {}

    std::unique_ptr<Program> parse();

private:
    const Token& peek(size_t offset = 0) const;
    bool check(TokenType type, size_t offset = 0) const;
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& message);

    std::unique_ptr<Statement> statement();
    std::unique_ptr<TypeDeclarationStmt> type_declaration();
    std::unique_ptr<AssignmentStmt> assignment();
    std::unique_ptr<ReturnStmt> return_statement();
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> primary();
    std::unique_ptr<CallExpr> call_expression(const std::string& callee);
    void consume_statement_terminator();
    [[noreturn]] void error_here(const Token& token, const std::string& message) const;

    std::vector<Token> tokens_;
    size_t pos_ = 0;
};
