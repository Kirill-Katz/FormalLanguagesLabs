#pragma once

#include <string>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include "shared.hpp"

//Variant 4
//Q = {q0,q1,q2,q3},
//∑ = {a,b},
//F = {q3},
//δ(q0,a) = q1,
//δ(q0,a) = q2,
//δ(q1,b) = q1,
//δ(q1,a) = q2,
//δ(q2,a) = q1,
//δ(q2,b) = q3.

class FiniteAutomaton {
public:
    FiniteAutomaton(
        States states,
        Alphabet alphabet,
        InitialState initial_state,
        FinalStates final_states,
        Transitions transitions
    );

    FiniteAutomaton(
        const Grammar& P,
        char start_symbol
    );

    void print_fa() const;
    bool is_deterministic() const;

    FiniteAutomaton convert_to_dfa() const;

    Grammar to_regular_grammar() const;
    Transitions to_dfa() const;
    bool validate_string(const std::string& input) const;
private:
    std::unordered_map<State, std::string> non_terminals_map_;
    States states_;
    Alphabet alphabet_;
    InitialState initial_state_;
    FinalStates final_states_;
    Transitions transitions_;
};

inline FiniteAutomaton::FiniteAutomaton(
    States states,
    Alphabet alphabet,
    InitialState initial_state,
    FinalStates final_states,
    Transitions transitions
) : states_(std::move(states)),
    alphabet_(std::move(alphabet)),
    initial_state_(std::move(initial_state)),
    final_states_(std::move(final_states)),
    transitions_(std::move(transitions))
{
    std::vector<State> ordered(states_.begin(), states_.end());
    std::sort(ordered.begin(), ordered.end());

    char non_term = 'A';
    int final_counter = 1;

    for (const auto& state : ordered) {
        if (!final_states_.contains(state)) {
            non_terminals_map_[state] = std::string(1, non_term++);
        }
    }

    for (const auto& state : ordered) {
        if (final_states_.contains(state)) {
            non_terminals_map_[state] = "F" + std::to_string(final_counter++);
        }
    }
};

inline FiniteAutomaton::FiniteAutomaton(const Grammar& P, char start_symbol) {
    for (const auto& [lhs, rules] : P) {
        states_.insert(lhs);

        for (const auto& rule : rules) {
            char terminal = rule[0];
            alphabet_.insert(terminal);

            if (rule.size() == 2) {
                states_.insert(std::string(1, rule[1]));
            }
        }
    }

    std::string final_state = "F";
    states_.insert(final_state);

    initial_state_ = std::string(1, start_symbol);

    for (const auto& [lhs, rules] : P) {
        for (const auto& rule : rules) {
            char terminal = rule[0];

            if (rule.size() == 2) {
                std::string to = std::string(1, rule[1]);
                transitions_[{lhs, terminal}].insert(to);
            }
            else if (rule.size() == 1) {
                transitions_[{lhs, terminal}].insert(final_state);
            }
        }
    }

    final_states_.insert(final_state);

    std::vector<State> ordered(states_.begin(), states_.end());
    std::sort(ordered.begin(), ordered.end());

    char non_term = 'A';
    int final_counter = 1;

    for (const auto& state : ordered) {
        if (!final_states_.contains(state)) {
            non_terminals_map_[state] = std::string(1, non_term++);
        }
    }

    for (const auto& state : ordered) {
        if (final_states_.contains(state)) {
            non_terminals_map_[state] =
                "F" + std::to_string(final_counter++);
        }
    }
}

inline bool FiniteAutomaton::validate_string(
    const std::string& input
) const {
    std::unordered_set<std::string> current = { initial_state_ };

    for (char c : input) {
        std::unordered_set<std::string> next;

        for (const auto& state : current) {
            auto it = transitions_.find({ state, c });

            if (it != transitions_.end()) {
                next.insert(it->second.begin(), it->second.end());
            }
        }

        if (next.empty()) {
            return false;
        }

        current = std::move(next);
    }

    for (const auto& state : current) {
        if (final_states_.contains(state)) {
            return true;
        }
    }

    return false;
}

inline bool FiniteAutomaton::is_deterministic() const {
    for (const auto& [key, next_states] : transitions_) {
        if (next_states.size() > 1) {
            return false;
        }
    }
    return true;
};

static std::string encode(const States& s) {
    std::vector<State> v(s.begin(), s.end());
    std::sort(v.begin(), v.end());

    std::string res;
    for (size_t i = 0; i < v.size(); ++i) {
        res += v[i];
        if (i + 1 < v.size()) res += "|";
    }
    return res;
}

inline FiniteAutomaton FiniteAutomaton::convert_to_dfa() const {
    if (is_deterministic()) {
        return FiniteAutomaton(*this);
    }

    std::queue<States> bfs;
    std::unordered_set<State> visited;

    Transitions dfa_transitions;
    States dfa_states;
    FinalStates dfa_finals;

    States start = { initial_state_ };

    bfs.push(start);
    visited.insert(encode(start));
    dfa_states.insert(encode(start));

    while (!bfs.empty()) {
        States current = bfs.front(); bfs.pop();
        std::string current_name = encode(current);

        for (const auto& s : current) {
            if (final_states_.contains(s)) {
                dfa_finals.insert(current_name);
                break;
            }
        }

        for (char a : alphabet_) {
            States next_union;

            for (const auto& s : current) {
                auto it = transitions_.find({s, a});

                if (it != transitions_.end()) {
                    next_union.insert(it->second.begin(), it->second.end());
                }
            }

            if (next_union.empty())
                continue;

            std::string next_name = encode(next_union);
            dfa_transitions[{current_name, a}] = {next_name};

            if (!visited.contains(next_name)) {
                visited.insert(next_name);
                bfs.push(next_union);
                dfa_states.insert(next_name);
            }
        }
    }

    return FiniteAutomaton(
        dfa_states,
        alphabet_,
        encode(start),
        dfa_finals,
        dfa_transitions
    );
}

inline Grammar FiniteAutomaton::to_regular_grammar() const {
    Grammar grammar;

    for (const auto& [pair_key, next_states] : transitions_) {
        const auto& [state, c] = pair_key;

        auto state_nt = non_terminals_map_.at(state);

        for (const auto& next_state : next_states) {
            auto next_state_nt = non_terminals_map_.at(next_state);
            grammar[state_nt].push_back(std::string(1, c) + next_state_nt);

            if (final_states_.contains(next_state)) {
                grammar[state_nt].push_back(std::string(1,c));
            }
        }
    }

    return grammar;
};

inline void FiniteAutomaton::print_fa() const {
    std::vector<State> ordered_states(states_.begin(), states_.end());
    std::sort(ordered_states.begin(), ordered_states.end());

    std::vector<char> ordered_alphabet(alphabet_.begin(), alphabet_.end());
    std::sort(ordered_alphabet.begin(), ordered_alphabet.end());

    std::vector<State> ordered_finals(final_states_.begin(), final_states_.end());
    std::sort(ordered_finals.begin(), ordered_finals.end());

    std::cout << "Q = {";
    for (size_t i = 0; i < ordered_states.size(); ++i) {
        std::cout << ordered_states[i];
        if (i + 1 < ordered_states.size()) std::cout << ",";
    }
    std::cout << "},\n";

    std::cout << "Σ = {";
    for (size_t i = 0; i < ordered_alphabet.size(); ++i) {
        std::cout << ordered_alphabet[i];
        if (i + 1 < ordered_alphabet.size()) std::cout << ",";
    }
    std::cout << "},\n";

    std::cout << "F = {";
    for (size_t i = 0; i < ordered_finals.size(); ++i) {
        std::cout << ordered_finals[i];
        if (i + 1 < ordered_finals.size()) std::cout << ",";
    }
    std::cout << "},\n";

    for (const auto& state : ordered_states) {
        for (char c : ordered_alphabet) {
            auto it = transitions_.find({state, c});
            if (it != transitions_.end()) {
                std::vector<State> next(it->second.begin(), it->second.end());
                std::sort(next.begin(), next.end());
                for (const auto& target : next) {
                    std::cout << "δ(" << state << "," << c << ") = "
                              << target << ",\n";
                }
            }
        }
    }
}

