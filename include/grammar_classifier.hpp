#pragma once

#include <string>
#include "shared.hpp"

// Type 3:
// LHS should be exactly a single non terminal, RHS should be either a terminal or a terminal + non terminal
// like this:
// A -> Ba
// A -> a
// and also A -> Ba must be be mixed with something like A -> aB
//
// Type 2:
// LHS is still only one single non terminla, but RHS can be of arbitrary size with the terminal between two non terminals
// like this:
// S -> aSb
// S -> eps
// more generally:
// A -> y where y can be anything
//
// Type 1: LHS can be a terminla + non terminal and RHS can be of arb. size with both terminals and non terminals
// like this:
// aA -> ab
// more generally:
// a A b -> a y b AND |RHS| >= |LHS| where a, b are called context and can be both terminals and non terminals and y is not eps
//
// Type 0: anything is allows, LHS can be anything RHS can be anything
// more generally:
// a -> b where a and b can be anything


class GrammarClassifier {
public:
    GrammarClassifier(
        const Productions& production,
        const NonTerm& non_terminals,
        const Term& terminals
    ): production_(production), non_term_(non_terminals), term_(terminals) {};

    int classify_grammar();
    inline bool check_3();
    inline bool check_2();
    inline bool check_1();
    inline bool check_0();

private:

    const Productions& production_;
    const NonTerm& non_term_;
    const Term& term_;
};

inline bool GrammarClassifier::check_3() {
    enum class Linearity {
        unknown,
        left,
        right
    };

    Linearity lin = Linearity::unknown;
    for (const auto& [lhs, rhses] : production_) {
        if (lhs.size() != 1 || !non_term_.contains(lhs[0])) {
            return false;
        }

        for (const auto& rhs : rhses) {
            if (rhs.empty()) {
                return false;
            }

            if (rhs.size() > 2) {
                return false;
            }

            if (rhs.size() == 1) {
                if (!term_.contains(rhs[0])) {
                    return false;
                }
                continue;
            }

            bool is_right = term_.contains(rhs[0]) && non_term_.contains(rhs[1]);
            bool is_left  = non_term_.contains(rhs[0]) && term_.contains(rhs[1]);

            if (!is_right && !is_left) {
                return false;
            }

            if (lin == Linearity::unknown) {
                lin = is_right ? Linearity::right : Linearity::left;
            } else if ((lin == Linearity::right && !is_right) ||
                       (lin == Linearity::left  && !is_left)) {
                return false;
            }
        }
    }

    return true;
}

inline bool GrammarClassifier::check_2() {
    for (const auto& [lhs, rhses] : production_) {
        if (lhs.size() != 1 || !non_term_.contains(lhs[0])) {
            return false;
        }

        for (const auto& rhs : rhses) {
            for (char c : rhs) {
                if (!term_.contains(c) && !non_term_.contains(c)) {
                    return false;
                }
            }
        }
    }

    return true;
}

inline bool GrammarClassifier::check_1() {
    for (const auto& [lhs, rhses] : production_) {
        int lhs_non_terms = 0;
        for (char c : lhs) {
            lhs_non_terms += non_term_.contains(c);
        }

        if (lhs_non_terms == 0) return false;

        for (const auto& rhs : rhses) {
            if (rhs.size() < lhs.size() || rhs.empty()) {
                return false;
            }
        }
    }

    return true;
}

inline bool GrammarClassifier::check_0() {
    return true;
}

inline int GrammarClassifier::classify_grammar() {
    if (check_3()) {
        return 3;
    }

    if (check_2()) {
        return 2;
    }

    if (check_1()) {
        return 1;
    }

    return 0;
}
