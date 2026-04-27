#include <iostream>
#include <regex>
#include <stdexcept>

#include "lexer.hpp"

const char* Lexer::token_to_str(TokenType t) const {
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

void Lexer::advance_position(const std::string& lexeme) {
    for (char c : lexeme) {
        if (c == '\n') {
            ++line_;
            col_ = 1;
            continue;
        }

        ++col_;
    }
}

Token Lexer::make_token(TokenType type, std::optional<std::string> lexeme, size_t line, size_t column) const {
    return Token{type, std::move(lexeme), line, column};
}

void Lexer::debug_log(const std::vector<Token>& tokens) {
    for (const Token& t : tokens) {
        std::cout << token_to_str(t.type)
                  << " @ " << t.line << ':' << t.column;

        if (t.lexeme) {
            std::cout << "('" << *t.lexeme << "')";
        }
        std::cout << '\n';
    }
}

std::vector<Token> Lexer::lex() {
    out_.clear();
    idx_ = 0;
    line_ = 1;
    col_ = 1;

    while (idx_ < src_.size()) {
        const char* begin = src_.c_str() + idx_;
        bool matched = false;

        for (const Rule& rule : rules_) {
            std::cmatch match;
            if (!std::regex_search(begin, match, rule.pattern, std::regex_constants::match_continuous)) {
                continue;
            }

            matched = true;
            const size_t token_line = line_;
            const size_t token_column = col_;
            std::string lexeme = match.str();

            idx_ += lexeme.size();
            advance_position(lexeme);

            if (rule.skip) {
                break;
            }

            if (rule.type == TokenType::StringLiteral) {
                out_.push_back(make_token(
                    rule.type,
                    lexeme.substr(1, lexeme.size() - 2),
                    token_line,
                    token_column
                ));
                break;
            }

            switch (rule.type) {
                case TokenType::Identifier:
                case TokenType::Integer:
                case TokenType::Float:
                case TokenType::Return:
                case TokenType::Type:
                    out_.push_back(make_token(rule.type, lexeme, token_line, token_column));
                    break;
                default:
                    out_.push_back(make_token(rule.type, std::nullopt, token_line, token_column));
                    break;
            }

            break;
        }

        if (!matched) {
            throw std::runtime_error(
                "Unexpected character '" + std::string(1, src_[idx_]) + "' at line " +
                std::to_string(line_) + ", column " + std::to_string(col_)
            );
        }
    }

    out_.push_back(make_token(TokenType::NewLine, std::nullopt, line_, col_));
    out_.push_back(make_token(TokenType::End, std::nullopt, line_, col_));
    return out_;
}
