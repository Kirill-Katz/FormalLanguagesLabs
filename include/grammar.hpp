#pragma once

#include <string>
#include <vector>
#include <random>
#include "finite_automaton.hpp"
#include "shared.hpp"

//Variant 4:
//VN={S, L, D},
//VT={a, b, c, d, e, f, j},
//P={
//    S → aS
//    S → bS
//    S → cD
//    S → dL
//    S → e
//    L → eL
//    L → fL
//    L → jD
//    L → e
//    D → eD
//    D → d
//}

struct pair_hash {
    std::size_t operator()(const std::pair<char,char>& p) const noexcept {
        return std::hash<char>{}(p.first) ^
               (std::hash<char>{}(p.second) << 1);
    }
};

class GrammarGenerator {
public:
    const std::vector<char> non_terminal = { 'S', 'L', 'D' };
    const std::vector<char> terminal = { 'a', 'b', 'c','d','e','f','j'};

    const Productions P = {
        { "S", { "aS", "bS", "cD", "dL", "e" } },
        { "L", { "eL", "fL", "jD", "e" } },
        { "D", { "eD", "d" } }
    };

    std::string generate_string() const;
    FiniteAutomaton to_finite_automaton() const;

    mutable std::mt19937 gen{std::random_device{}()};
};

inline FiniteAutomaton GrammarGenerator::to_finite_automaton() const {
    return FiniteAutomaton(P, 'S');
}

inline std::string GrammarGenerator::generate_string() const {
    std::string ans = "";
    std::string next = "S";

    while (true) {
        const auto& rules = P.at(next);

        std::uniform_int_distribution<> dist(0, rules.size() - 1);
        const std::string& chosen = rules[dist(gen)];

        ans += chosen[0];

        if (chosen.size() == 1) {
            break;
        }

        next = chosen[1];
    }

    return ans;
}

