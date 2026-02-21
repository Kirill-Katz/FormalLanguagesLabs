#pragma once

#include "shared.hpp"
#include <algorithm>
#include <climits>
#include <string>

// The orderings START,TERM,BIN,DEL,UNIT and START,BIN,DEL,UNIT,TERM lead to the least (i.e. quadratic) blow-up.

class ChomskyNormalForm {
public:
    explicit ChomskyNormalForm(Grammar g)
        : grammar_{std::move(g)} {};

    void normalize();
    const Grammar& result() const { return grammar_; }

private:
    void START();
    void TERM();
    void BIN();
    void DEL();
    void UNIT();

    Grammar grammar_;
};

inline void ChomskyNormalForm::START() {
    grammar_.productions.insert({ "S0", { { grammar_.start_symbol } } });
    grammar_.start_symbol = "S0";
    grammar_.non_terminals.insert("S0");
}

inline void ChomskyNormalForm::TERM() {
    std::unordered_map<std::string, std::string> processed_terminals;

    for (auto& [lhs, rhses] : grammar_.productions) {
        for (auto& rhs : rhses) {
            if (rhs.size() <= 1) continue;

            for (auto& sym : rhs) {
                if (grammar_.terminals.contains(sym)) {

                    if (!processed_terminals.contains(sym)) {
                        processed_terminals.insert({ sym, "N" + sym });
                    }

                    sym = processed_terminals.at(sym);
                }
            }
        }
    }

    for (const auto& [terminal, new_nt] : processed_terminals) {
        grammar_.productions.insert({ new_nt, {{ terminal }} });
        grammar_.non_terminals.insert(new_nt);
    }
}

inline void ChomskyNormalForm::BIN() {
    int a = 0;
    Grammar::Productions new_productions;

    for (auto& [lhs, rhses] : grammar_.productions) {
        for (auto& rhs : rhses) {
            if (rhs.size() <= 2) continue;

            std::string A0 = "A" + std::to_string(a++);
            std::vector<std::string> new_rhs = { rhs[0], A0 };

            for (size_t i = 1; i + 2 < rhs.size(); ++i) {
                std::string curA = "A" + std::to_string(a - 1);
                std::string nextA = "A" + std::to_string(a++);

                new_productions[curA].push_back({ rhs[i], nextA });
            }

            std::string lastA = "A" + std::to_string(a - 1);
            new_productions[lastA].push_back({ rhs[rhs.size() - 2], rhs[rhs.size() - 1] });

            rhs = std::move(new_rhs);
        }
    }

    for (auto& [non_term, rhss] : new_productions) {
        auto& dst = grammar_.productions[non_term];
        dst.insert(dst.end(), rhss.begin(), rhss.end());
        grammar_.non_terminals.insert(non_term);
    }
}


inline void ChomskyNormalForm::DEL() {
    std::set<Grammar::LHS> nullable;

    std::unordered_map<std::string, std::vector<int>> need;
    std::unordered_map<std::string, std::vector<std::pair<std::string, size_t>>> uses;

    for (auto& [lhs, rhses] : grammar_.productions) {
        need[lhs].resize(rhses.size(), 0);

        size_t i = 0;
        for (auto& rhs : rhses) {
            for (auto& sym : rhs) {
                if (grammar_.non_terminals.contains(sym)) {
                    uses[sym].push_back({ lhs, i });
                    need[lhs][i]++;
                }
            }
            ++i;
        }
    }

    auto propagate_nullability = [&](auto self, const auto& lhs) {
        if (!nullable.insert(lhs).second) {
            return;
        }

        for (auto& [non_term, prod_id] : uses[lhs]) {
            if (--need[non_term][prod_id] == 0) {
                self(self, non_term);
            }
        }
    };

    for (auto& [lhs, rhses] : grammar_.productions) {
        for (auto& rhs : rhses) {
            if (rhs.empty()) {
                propagate_nullability(propagate_nullability, lhs);
                break;
            }
        }
    }

    auto generate_options = [&](const auto& rhs) {
        std::vector<Grammar::RHS> ans;
        ans.push_back({});

        for (const auto& sym : rhs) {
            if (nullable.contains(sym)) {
                size_t answers = ans.size();
                ans.reserve(answers * 2);
                ans.insert(ans.end(), ans.begin(), ans.end());

                for (size_t i = 0; i < answers; ++i) {
                    ans[i].push_back(sym);
                }
            } else {
                for (auto& opt : ans) {
                    opt.push_back(sym);
                }
            }
        }

        return ans;
    };

    for (auto& [lhs, rhsese] : grammar_.productions) {
        size_t rhs_count = rhsese.size();
        // for the love of god do not use iterators here
        for (size_t i = 0; i < rhs_count; ++i) {
            if (rhsese[i].empty()) continue;
            auto answer = generate_options(rhsese[i]);
            // skip the first element from answer bc that's the original rhs
            // skip the last element if the rhs is epsilon, becase it will be {}
            // proof by trust me bro
            auto first = answer.begin() + 1;
            auto last = answer.end();

            if (need[lhs][i] == 0 && lhs != grammar_.start_symbol) {
                last = answer.end() - 1;
            }

            rhsese.insert(rhsese.end(), first, last);
        }
    }

    // last step is to remove all the epsilons
    for (auto& [lhs, rhses] : grammar_.productions) {
        if (lhs == grammar_.start_symbol) {
            continue;
        }

        rhses.erase(
            std::remove_if(rhses.begin(), rhses.end(),
            [](const auto& rhs) { return rhs.empty(); }),
            rhses.end()
        );
    }
}
