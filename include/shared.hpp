#pragma once
#include <string>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <vector>
#include <iostream>
#include <algorithm>

struct state_char_pair_hash {
    template<typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const noexcept {
        std::size_t h1 = std::hash<T1>{}(p.first);
        std::size_t h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

using LHS = std::string;
using RHS = std::string;
using NonTerm = std::unordered_set<char>;
using Term = std::unordered_set<char>;
using Productions = std::unordered_map<LHS, std::vector<RHS>>;

struct Grammar {
    using Symbol = std::string;
    using RHS = std::vector<Symbol>;
    using LHS = Symbol;
    using Productions = std::unordered_map<LHS, std::vector<RHS>>;

    Symbol start_symbol;
    std::set<Symbol> non_terminals;
    std::set<Symbol> terminals;
    Productions productions;

    void print_grammar(std::ostream& os = std::cout) const {
        os << "Start symbol: " << start_symbol << "\n";
        os << "Non-terminals: {";
        {
            bool first = true;
            for (const auto& nt : non_terminals) {
                if (!first) os << ", ";
                os << nt;
                first = false;
            }
        }
        os << "}\n";

        os << "Terminals: {";
        {
            bool first = true;
            for (const auto& t : terminals) {
                if (!first) os << ", ";
                os << t;
                first = false;
            }
        }
        os << "}\n\n";

        std::vector<LHS> lhs_list;
        lhs_list.reserve(productions.size());

        for (const auto& [lhs, _] : productions) {
            lhs_list.push_back(lhs);
        }

        std::sort(lhs_list.begin(), lhs_list.end());

        for (const auto& lhs : lhs_list) {
            const auto& rhses = productions.at(lhs);

            os << lhs << " -> ";

            if (rhses.empty()) {
                os << "∅";
            } else {
                for (size_t i = 0; i < rhses.size(); ++i) {
                    if (i != 0) os << " | ";

                    const auto& rhs = rhses[i];
                    if (rhs.empty()) {
                        os << "ε";
                    } else {
                        for (size_t j = 0; j < rhs.size(); ++j) {
                            if (j != 0) os << ' ';
                            os << rhs[j];
                        }
                    }
                }
            }

            os << '\n';
        }
    }
};

using State = std::string;
using States = std::unordered_set<State>;
using FinalStates = std::unordered_set<State>;
using Alphabet = std::unordered_set<char>;
using InitialState = State;
using Transitions = std::unordered_map<
    std::pair<State, char>,
    States,
    state_char_pair_hash
>;

enum class RegexTokenType {
    Or, // |
    Star,
    RParen,
    LParen,
    Caret, // ^
    Plus,
    Number,
    Char,
    QMark
};

struct RegexToken {
    RegexTokenType type;
    std::optional<std::string> lexeme; //not char because of numbers
};
