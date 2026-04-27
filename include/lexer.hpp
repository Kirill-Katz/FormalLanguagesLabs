#pragma once
#include <cstddef>
#include <iostream>
#include <optional>
#include <regex>
#include <string>
#include <vector>

enum class TokenType {
    Identifier,
    Equals,
    LParen,
    RParen,
    Comma,
    NewLine,
    Colon,
    StringLiteral,
    Integer,
    Float,
    Return,
    Type,
    End
};

struct Token {
    TokenType type;
    std::optional<std::string> lexeme;
    size_t line;
    size_t column;
};

struct Rule {
    TokenType type;
    std::regex pattern;
    bool skip;
};

class Lexer {
public:
    explicit Lexer(std::string src)
    : src_{std::move(src)} {}

    std::vector<Token> lex();
    const char* token_to_str(TokenType t) const;
    void debug_log(const std::vector<Token>& tokens);

private:
    void advance_position(const std::string& lexeme);
    Token make_token(TokenType type, std::optional<std::string> lexeme, size_t line, size_t column) const;

    std::vector<Token> out_;
    const std::string src_;
    size_t col_ = 1;
    size_t line_ = 1;
    size_t idx_ = 0;

    const std::vector<Rule> rules_ = {
        {TokenType::NewLine, std::regex(R"(\n)"), false},
        {TokenType::End, std::regex(R"([ \t\r]+)"), true},
        {TokenType::Return, std::regex(R"(return\b)"), false},
        {TokenType::Type, std::regex(R"(VecF64\b)"), false},
        {TokenType::Float, std::regex(R"([0-9]+\.[0-9]+)"), false},
        {TokenType::Integer, std::regex(R"([0-9]+)"), false},
        {TokenType::StringLiteral, std::regex(R"("([^"\\]|\\.)*")"), false},
        {TokenType::Identifier, std::regex(R"([A-Za-z_][A-Za-z0-9_]*)"), false},
        {TokenType::Equals, std::regex(R"(=)"), false},
        {TokenType::LParen, std::regex(R"(\()"), false},
        {TokenType::RParen, std::regex(R"(\))"), false},
        {TokenType::Comma, std::regex(R"(,)"), false},
        {TokenType::Colon, std::regex(R"(:)"), false},
    };
};
