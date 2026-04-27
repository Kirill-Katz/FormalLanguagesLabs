#include <stdexcept>
#include <string>
#include <utility>

#include "parser.hpp"

const Token& Parser::peek(size_t offset) const {
    const size_t index = pos_ + offset;
    if (index >= tokens_.size()) {
        return tokens_.back();
    }

    return tokens_[index];
}

bool Parser::check(TokenType type, size_t offset) const {
    return peek(offset).type == type;
}

bool Parser::match(TokenType type) {
    if (!check(type)) {
        return false;
    }

    ++pos_;
    return true;
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (!check(type)) {
        error_here(peek(), message);
    }

    return tokens_[pos_++];
}

[[noreturn]] void Parser::error_here(const Token& token, const std::string& message) const {
    throw std::runtime_error(
        "Parse error at line " + std::to_string(token.line) +
        ", column " + std::to_string(token.column) + ": " + message
    );
}

void Parser::consume_statement_terminator() {
    if (match(TokenType::End)) {
        return;
    }

    if (!match(TokenType::NewLine)) {
        error_here(peek(), "expected end of statement");
    }

    while (match(TokenType::NewLine)) {
    }
}

std::unique_ptr<CallExpr> Parser::call_expression(const std::string& callee) {
    auto call = std::make_unique<CallExpr>(callee);
    consume(TokenType::LParen, "expected '(' after function name");

    if (!check(TokenType::RParen)) {
        do {
            call->arguments.push_back(expression());
        } while (match(TokenType::Comma));
    }

    consume(TokenType::RParen, "expected ')' after function arguments");
    return call;
}

std::unique_ptr<Expression> Parser::primary() {
    if (check(TokenType::Identifier) && check(TokenType::LParen, 1)) {
        const std::string callee = consume(TokenType::Identifier, "expected function name").lexeme.value();
        return call_expression(callee);
    }

    if (check(TokenType::Identifier)) {
        return std::make_unique<IdentifierExpr>(
            consume(TokenType::Identifier, "expected identifier").lexeme.value()
        );
    }

    if (check(TokenType::StringLiteral)) {
        return std::make_unique<LiteralExpr>(
            "StringLiteral",
            consume(TokenType::StringLiteral, "expected string literal").lexeme.value()
        );
    }

    if (check(TokenType::Float)) {
        return std::make_unique<LiteralExpr>(
            "FloatLiteral",
            consume(TokenType::Float, "expected float literal").lexeme.value()
        );
    }

    if (check(TokenType::Integer)) {
        return std::make_unique<LiteralExpr>(
            "IntegerLiteral",
            consume(TokenType::Integer, "expected integer literal").lexeme.value()
        );
    }

    error_here(peek(), "expected expression");
}

std::unique_ptr<Expression> Parser::expression() {
    return primary();
}

std::unique_ptr<TypeDeclarationStmt> Parser::type_declaration() {
    const std::string name = consume(TokenType::Identifier, "expected identifier").lexeme.value();
    consume(TokenType::Colon, "expected ':' in type declaration");
    const std::string type_name = consume(TokenType::Type, "expected known type").lexeme.value();

    auto node = std::make_unique<TypeDeclarationStmt>(name, type_name);
    if (match(TokenType::Equals)) {
        node->value = expression();
    }

    consume_statement_terminator();
    return node;
}

std::unique_ptr<AssignmentStmt> Parser::assignment() {
    const std::string target = consume(TokenType::Identifier, "expected assignment target").lexeme.value();
    consume(TokenType::Equals, "expected '=' in assignment");

    auto node = std::make_unique<AssignmentStmt>(target);
    node->value = expression();
    consume_statement_terminator();
    return node;
}

std::unique_ptr<ReturnStmt> Parser::return_statement() {
    consume(TokenType::Return, "expected 'return'");

    auto node = std::make_unique<ReturnStmt>();
    node->value = expression();
    consume_statement_terminator();
    return node;
}

std::unique_ptr<Statement> Parser::statement() {
    if (check(TokenType::Return)) {
        return return_statement();
    }

    if (check(TokenType::Identifier) && check(TokenType::Colon, 1)) {
        return type_declaration();
    }

    if (check(TokenType::Identifier) && check(TokenType::Equals, 1)) {
        return assignment();
    }

    error_here(peek(), "expected declaration, assignment, or return statement");
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();

    while (match(TokenType::NewLine)) {
    }

    while (!check(TokenType::End)) {
        program->statements.push_back(statement());
    }

    consume(TokenType::End, "expected end of input");
    return program;
}
