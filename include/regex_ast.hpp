#pragma once

#include "shared.hpp"

#include <stdexcept>
#include <vector>
#include <variant>
#include <memory>

// (S|T)(U|V)W*Y+24
// L(U|N)O^3p*Q(2|3)
// R*S(T|U|V)W(X|Y|Z)^2

struct LiteralNode;
struct ConcatNode;
struct OrNode;
struct StarNode;
struct PlusNode;
struct RepeatNode;

using RegexAST = std::variant<
    LiteralNode,
    ConcatNode,
    OrNode,
    StarNode,
    PlusNode,
    RepeatNode
>;

struct LiteralNode {
    std::string value;
};

struct ConcatNode {
    std::vector<std::unique_ptr<RegexAST>> children;
};

struct OrNode {
    std::unique_ptr<RegexAST> left;
    std::unique_ptr<RegexAST> right;
};

struct StarNode {
    std::unique_ptr<RegexAST> left;
};

struct PlusNode {
    std::unique_ptr<RegexAST> left;
};

struct RepeatNode {
    std::unique_ptr<RegexAST> left;
    int count;
};

class RegexASTBuilder {
public:
    RegexASTBuilder(const std::vector<RegexToken>& tokens)
    : tokens_{tokens} {};

    std::unique_ptr<RegexAST> build();

private:
    const RegexToken& peek() const {
        return tokens_[pos_];
    };

    const RegexToken& advance() {
        return tokens_[pos_++];
    };

    std::unique_ptr<RegexAST> expression();
    std::unique_ptr<RegexAST> concater();
    std::unique_ptr<RegexAST> base_wrapper();
    std::unique_ptr<RegexAST> base();
    std::unique_ptr<RegexAST> group();
    std::unique_ptr<RegexAST> atom();

    bool match(RegexTokenType type) {
        if (pos_ >= tokens_.size()) return false;
        return tokens_[pos_].type == type;
    };

    bool is_at_end() const {
        return pos_ >= tokens_.size();
    }

    const std::vector<RegexToken> tokens_;
    size_t pos_ = 0;
};

inline std::unique_ptr<RegexAST> RegexASTBuilder::atom() {
    LiteralNode lit;
    lit.value = advance().lexeme.value();

    return std::make_unique<RegexAST>(std::move(lit));
}

inline std::unique_ptr<RegexAST> RegexASTBuilder::group() {
    advance();

    auto node = expression();

    if (!match(RegexTokenType::RParen)) {
        throw std::runtime_error("Expected ')'");
    }

    advance();
    return node;
}

inline std::unique_ptr<RegexAST> RegexASTBuilder::base() {
    if (match(RegexTokenType::Char) || match(RegexTokenType::Number)) {
        return atom();
    }

    if (match(RegexTokenType::LParen)) {
        return group();
    }

    throw std::runtime_error("Unexpected token in base()");
}

inline std::unique_ptr<RegexAST> RegexASTBuilder::base_wrapper() {
    auto node = base();

    while (!is_at_end()) {
        if (match(RegexTokenType::Star)) {
            advance();

            StarNode star;
            star.left = std::move(node);
            node = std::make_unique<RegexAST>(std::move(star));
        }
        else if (match(RegexTokenType::Plus)) {
            advance();

            PlusNode plus;
            plus.left = std::move(node);
            node = std::make_unique<RegexAST>(std::move(plus));
        }
        else if (match(RegexTokenType::Caret)) {
            advance();

            if (!match(RegexTokenType::Number)) {
                throw std::runtime_error("Expected number after ^");
            }

            int count = std::stoi(advance().lexeme.value());

            RepeatNode repeat;
            repeat.left = std::move(node);
            repeat.count = count;

            node = std::make_unique<RegexAST>(std::move(repeat));
        } else {
            break;
        }
    }

    return node;

}

inline std::unique_ptr<RegexAST> RegexASTBuilder::concater() {
    std::vector<std::unique_ptr<RegexAST>> nodes;

    while (
        !is_at_end() &&
        (match(RegexTokenType::Char) || match(RegexTokenType::LParen) || match(RegexTokenType::Number))
    ) {
        nodes.push_back(base_wrapper());
    }

    if (nodes.empty()) {
        throw std::runtime_error("Expected term");
    }

    if (nodes.size() == 1) {
        return std::move(nodes[0]);
    }

    ConcatNode concat;
    concat.children = std::move(nodes);
    return std::make_unique<RegexAST>(std::move(concat));
}

inline std::unique_ptr<RegexAST> RegexASTBuilder::expression() {
    auto node = concater();

    while (!is_at_end() && match(RegexTokenType::Or)) {
        advance();
        auto right = concater();

        OrNode orNode;
        orNode.left = std::move(node);
        orNode.right = std::move(right);

        node = std::make_unique<RegexAST>(std::move(orNode));
    }

    return node;
}

inline std::unique_ptr<RegexAST> RegexASTBuilder::build() {
    pos_ = 0;
    return expression();
}
