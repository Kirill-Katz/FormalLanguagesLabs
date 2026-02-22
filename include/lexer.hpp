#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <optional>
#include <regex>

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
};

struct Rule {
    TokenType type;
    std::regex pattern;
    bool skip;
};

class Lexer {
public:
    Lexer(const std::string src)
    : src_{src} {};

    std::vector<Token> lex();
    const char* token_to_str(TokenType t);
    void debug_log(const std::vector<Token>& tokens);


private:
    void scan_identifier();
    void scan_string_literal();
    void scan_number();

    std::vector<Token> out_;
    const std::string src_;
    size_t col_ = 1;
    size_t line_ = 1;
    size_t idx_ = 0;

    const std::unordered_map<std::string, TokenType> identifier_token = {
        {"return", TokenType::Return},
        {"VecF64", TokenType::Type},
    };

    const std::vector<Rule> rules = {

    };
};

