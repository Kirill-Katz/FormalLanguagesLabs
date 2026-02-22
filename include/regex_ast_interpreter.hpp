#pragma once

#include <random>
#include <variant>
#include "regex_ast.hpp"

class RegexASTInterpreter {
public:
    std::string generate(const RegexAST& node);
private:
    std::mt19937 gen_{std::random_device{}()};
};

inline std::string RegexASTInterpreter::generate(const RegexAST& ast) {
    return std::visit([this](auto&& node) -> std::string {
        using T = std::decay_t<decltype(node)>;

        if constexpr (std::is_same_v<T, LiteralNode>) {
            return { node.value };
        }

        else if constexpr (std::is_same_v<T, OrNode>) {
            std::uniform_int_distribution<> dist(0, 1);

            if (dist(gen_) == 1) {
                return generate(*node.left);
            } else {
                return generate(*node.right);
            }
        }

        else if constexpr (std::is_same_v<T, ConcatNode>) {
            std::string result;
            for (const auto& child : node.children) {
                result += generate(*child);
            }
            return result;
        }

        else if constexpr (std::is_same_v<T, StarNode>) {
            std::uniform_int_distribution<> dist(0, 5);
            int k = dist(gen_);

            std::string result;
            for (int i = 0; i < k; ++i) {
                result += generate(*node.left);
            }

            return result;
        }

        else if constexpr (std::is_same_v<T, PlusNode>) {
            std::uniform_int_distribution<> dist(1, 5);
            int k = dist(gen_);

            std::string result;
            for (int i = 0; i < k; ++i) {
                result += generate(*node.left);
            }

            return result;
        }

        else if constexpr (std::is_same_v<T, RepeatNode>) {
            std::string result;
            for (int i = 0; i < node.count; ++i)
                result += generate(*node.left);

            return result;
        }

        else if constexpr (std::is_same_v<T, QMarkNode>) {
            std::string result;

            std::uniform_int_distribution<> dist(0, 1);
            if (dist(gen_) == 1) {
                result += generate(*node.left);
            }

            return result;
        }
    }, ast);
}
