#include <cctype>
#include <optional>
#include <stdexcept>
#include <iostream>
#include "lexer.hpp"

static inline bool is_letter(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

static inline bool is_alnum(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

static inline bool is_digit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

const char* Lexer::token_to_str(TokenType t) {
    switch (t) {
        case TokenType::Identifier:           return "Identifier";
        case TokenType::Equals:               return "Equals";
        case TokenType::LParen:               return "LParen";
        case TokenType::RParen:               return "RParen";
        case TokenType::Comma:                return "Comma";
        case TokenType::NewLine:              return "NewLine";
        case TokenType::Colon:                return "Colon";
        case TokenType::End:                  return "End";
        case TokenType::StringLiteral:        return "StringLiteral";
        case TokenType::Integer:              return "Integer";
        case TokenType::Float:                return "Float";
        case TokenType::Return:               return "Return";
        case TokenType::Type:                 return "Type";
    }
    return "Unknown";
}

static const std::optional<TokenType> single_char_token(char c) {
    switch (c) {
        case '(': return TokenType::LParen;
        case ')': return TokenType::RParen;
        case ',': return TokenType::Comma;
        case '=': return TokenType::Equals;
        case ':': return TokenType::Colon;
        default:  return std::nullopt;
    }
}

void Lexer::debug_log(const std::vector<Token>& tokens) {
    for (const Token& t : tokens) {
        std::cout << token_to_str(t.type);
        if (t.lexeme) {
            std::cout << "('" << *t.lexeme << "')";
        }
        std::cout << '\n';
    }
}

void Lexer::scan_identifier() {
    std::string lexeme;
    lexeme.push_back(src_[idx_]);
    idx_++;
    col_++;

    while (idx_ < src_.size() && is_alnum(src_[idx_])) {
        lexeme.push_back(src_[idx_]);
        idx_++;
        col_++;
    }

    if (identifier_token.contains(lexeme)) {
        auto token = identifier_token.at(lexeme);
        out_.push_back({ token, std::move(lexeme) });
        return;
    }

    out_.push_back({TokenType::Identifier, std::move(lexeme)});
}

void Lexer::scan_number() {
    std::string value;
    bool is_float = false;

    while (idx_ < src_.size() && is_digit(src_[idx_])) {
        value += src_[idx_++];
        col_++;
    }

    if (idx_ < src_.size() && src_[idx_] == '.') {
        is_float = true;
        value += src_[idx_++];
        col_++;

        if (idx_ >= src_.size() || !is_digit(src_[idx_])) {
            throw std::runtime_error(
                "Malformed float at line " +
                std::to_string(line_) + ", column " +
                std::to_string(col_)
            );
        }

        while (idx_ < src_.size() && is_digit(src_[idx_])) {
            value += src_[idx_++];
            col_++;
        }
    }

    if (is_float) {
        out_.push_back({TokenType::Float, std::move(value)});
    } else {
        out_.push_back({TokenType::Integer, std::move(value)});
    }
}

void Lexer::scan_string_literal() {
    idx_++;
    col_++;

    std::string value;

    while (idx_ < src_.size() && src_[idx_] != '"') {
        if (src_[idx_] == '\n') {
            throw std::runtime_error("Unterminated string literal");
        }
        value += src_[idx_++];
        col_++;
    }

    if (idx_ >= src_.size()) {
        throw std::runtime_error("Unterminated string literal");
    }

    idx_++;
    col_++;

    out_.push_back({TokenType::StringLiteral, std::move(value)});
}

std::vector<Token> Lexer::lex() {
    while (idx_ < src_.size()) {
        const char c = src_[idx_];

        if (c == '\n') {
            out_.push_back({TokenType::NewLine, std::nullopt});
            line_++;
            col_ = 1;
            ++idx_; continue;
        }

        if (auto t = single_char_token(c)) {
            out_.push_back({*t, std::nullopt});
            idx_++;
            col_++;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c))) {
            idx_++;
            col_++;
            continue;
        }

        if (c == '"') {
            scan_string_literal();
            continue;
        }

        if (is_digit(c)) {
            scan_number();
            continue;
        }

        if (is_letter(c)) {
            scan_identifier();
            continue;
        }

        throw std::runtime_error(
            "Unexpected character '" + std::string(1, c) + "' at line " +
            std::to_string(line_) + ", column " + std::to_string(col_)
        );
    }

    out_.push_back({TokenType::NewLine, std::nullopt});
    out_.push_back({TokenType::End, std::nullopt});
    return out_;
}

