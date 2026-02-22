#pragma once

#include "shared.hpp"

#include <string>

// (S|T)(U|V)W*Y+24
// L(U|N)O^3p*Q(2|3)
// R*S(T|U|V)W(X|Y|Z)^2

class RegexLexer {
public:
    RegexLexer(const std::string src) : source_{src} {};
    std::vector<RegexToken> lex();

private:
    std::vector<RegexToken> out_;
    const std::string source_;
};

inline std::vector<RegexToken> RegexLexer::lex() {
    out_.clear();
    for (size_t i = 0; i < source_.size(); ++i) {
        char c = source_[i];

        switch (c) {
            case '|':
                out_.push_back({ RegexTokenType::Or, std::nullopt });
                break;

            case '*':
                out_.push_back({ RegexTokenType::Star, std::nullopt });
                break;

            case '+':
                out_.push_back({ RegexTokenType::Plus, std::nullopt });
                break;

            case '^':
                out_.push_back({ RegexTokenType::Caret, std::nullopt });
                break;

            case '(':
                out_.push_back({ RegexTokenType::LParen, std::nullopt });
                break;

            case ')':
                out_.push_back({ RegexTokenType::RParen, std::nullopt });
                break;

            case '?':
                out_.push_back({ RegexTokenType::QMark, std::nullopt });
                break;

            default: {
                if (std::isdigit(c)) {
                    size_t start = i;
                    while (i < source_.size() && std::isdigit(source_[i])) {
                        ++i;
                    }

                    out_.push_back({
                        RegexTokenType::Number,
                        source_.substr(start, i - start)
                    });
                    --i;
                }
                else {
                    out_.push_back({
                        RegexTokenType::Char,
                        std::string(1, c)
                    });
                }
                break;
            }
        }
    }

    return out_;
}
