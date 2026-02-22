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
