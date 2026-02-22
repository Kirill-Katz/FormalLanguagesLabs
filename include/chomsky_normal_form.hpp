#pragma once

#include <climits>
#include <string>
#include "cnf_grammar.hpp"

// The orderings START,TERM,BIN,DEL,UNIT and START,BIN,DEL,UNIT,TERM lead to the least (i.e. quadratic) blow-up.

class ChomskyNormalForm {
public:
    explicit ChomskyNormalForm(Grammar g)
        : grammar_{std::move(g)} {};

    void normalize() {
        START();
        TERM();
        BIN();
        DEL();
        dedup_productions();
        UNIT();
        dedup_productions();
        eliminate_inaccesible_sym();
        eliminate_non_productive_sym();
    }

    Grammar result() const { return grammar_; }

private:
    void dedup_productions();

    void eliminate_inaccesible_sym();
    void eliminate_non_productive_sym();

    std::string fresh_non_terminal(const std::string& pref);

    void START();
    void TERM();
    void BIN();
    void DEL();
    void UNIT();

    Grammar grammar_;
};

inline std::string ChomskyNormalForm::fresh_non_terminal(const std::string& prefix) {
    if (!grammar_.non_terminals.contains(prefix) &&
        !grammar_.terminals.contains(prefix)) {
        grammar_.non_terminals.insert(prefix);
        return prefix;
    }

    int id = 0;
    while (true) {
        std::string candidate = prefix + std::to_string(id++);
        if (!grammar_.non_terminals.contains(candidate) &&
            !grammar_.terminals.contains(candidate)) {
            grammar_.non_terminals.insert(candidate);
            return candidate;
        }
    }
};

inline void ChomskyNormalForm::eliminate_inaccesible_sym() {
    std::set<Grammar::Symbol> visited;

    auto dfs = [&](
        auto&& self,
        const std::string& node
    ) -> void {
        if (!grammar_.non_terminals.contains(node)) return;
        if (!visited.insert(node).second) return;

        auto it = grammar_.productions.find(node);
        if (it == grammar_.productions.end()) return;

        for (const auto& rhs : it->second) {
            for (const auto& sym : rhs) {
                self(self, sym);
            }
        }
    };

    dfs(dfs, grammar_.start_symbol);

    std::erase_if(grammar_.productions, [&](const auto& p) {
        return !visited.contains(p.first);
    });

    grammar_.non_terminals = std::move(visited);
}

inline void ChomskyNormalForm::eliminate_non_productive_sym() {
    std::set<Grammar::LHS> productive;
    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> uses;
    std::unordered_map<std::string, std::vector<int>> need;

    for (auto& [lhs, rhses] : grammar_.productions) {
        need[lhs].resize(rhses.size(), 0);

        for (size_t i = 0; i < rhses.size(); ++i) {
            for (auto& sym : rhses[i]) {
                if (grammar_.non_terminals.contains(sym)) {
                    uses[sym].push_back({ lhs, i });
                    need[lhs][i]++;
                }
            }
        }
    }

    auto propagate_productiveness = [&](auto&& self, const auto& node) {
        if (!productive.insert(node).second) {
            return;
        }
        for (auto [non_term, prod_id] : uses[node]) {
            need[non_term][prod_id]--;

            if (need[non_term][prod_id] == 0) {
                self(self, non_term);
            }
        }
    };

    for (auto& [lhs, rhses] : grammar_.productions) {
        for (size_t i = 0; i < rhses.size(); ++i) {
            if (need[lhs][i] == 0) {
                propagate_productiveness(propagate_productiveness, lhs);
            }
        }
    }

    std::erase_if(grammar_.productions, [&](const auto& p) {
        return !productive.contains(p.first);
    });

    for (auto& [lhs, rhses] : grammar_.productions) {
        std::erase_if(rhses, [&](const auto& rhs) {
            return std::ranges::any_of(rhs, [&](const auto& sym) {
                return grammar_.non_terminals.contains(sym) && !productive.contains(sym);
            });
        });
    }

    grammar_.non_terminals = std::move(productive);
}

inline void ChomskyNormalForm::dedup_productions() {
    for (auto& [lhs, rhses] : grammar_.productions) {
        std::ranges::sort(rhses);
        auto [first, last] = std::ranges::unique(rhses);
        rhses.erase(first, last);
    }
}

inline void ChomskyNormalForm::START() {
    std::string start_sym = fresh_non_terminal("S");

    grammar_.productions.insert({ start_sym, { { grammar_.start_symbol } } });
    grammar_.start_symbol = start_sym;
}

inline void ChomskyNormalForm::TERM() {
    std::unordered_map<std::string, std::string> processed_terminals;

    for (auto& [lhs, rhses] : grammar_.productions) {
        for (auto& rhs : rhses) {
            if (rhs.size() <= 1) continue;

            for (auto& sym : rhs) {
                if (grammar_.terminals.contains(sym)) {
                    if (!processed_terminals.contains(sym)) {
                        processed_terminals.insert({ sym, fresh_non_terminal("N" + sym) });
                    }

                    sym = processed_terminals.at(sym);
                }
            }
        }
    }

    for (const auto& [terminal, new_nt] : processed_terminals) {
        grammar_.productions.insert({ new_nt, {{ terminal }} });
    }
}

inline void ChomskyNormalForm::BIN() {
    Grammar::Productions new_productions;

    for (auto& [lhs, rhses] : grammar_.productions) {
        for (auto& rhs : rhses) {
            if (rhs.size() <= 2) continue;

            std::string prev = fresh_non_terminal("A");
            std::vector<std::string> new_rhs = { rhs[0], prev };

            for (size_t i = 1; i + 2 < rhs.size(); ++i) {
                std::string cur = prev;
                std::string next = fresh_non_terminal("A");

                new_productions[cur].push_back({ rhs[i], next });
                prev = next;
            }

            new_productions[prev].push_back({ rhs[rhs.size() - 2], rhs[rhs.size() - 1] });
            rhs = std::move(new_rhs);
        }
    }

    for (auto& [non_term, rhss] : new_productions) {
        auto& dst = grammar_.productions[non_term];
        dst.insert(dst.end(), rhss.begin(), rhss.end());
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

    auto propagate_nullability = [&](auto&& self, const auto& lhs) {
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
                size_t old_size = ans.size();
                ans.resize(old_size * 2);

                for (size_t i = 0; i < old_size; ++i) {
                    ans[old_size + i] = ans[i];
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

            if (lhs != grammar_.start_symbol && !answer.empty() && answer.back().empty()) {
                last = answer.end() - 1;
            }

            if (first < last) {
                rhsese.insert(rhsese.end(), first, last);
            }
        }
    }

    // last step is to remove all the epsilons
    for (auto& [lhs, rhses] : grammar_.productions) {
        if (lhs == grammar_.start_symbol) {
            continue;
        }

        std::erase_if(rhses, [](const auto& rhs) {
            return rhs.empty();
        });
    }
}

inline void ChomskyNormalForm::UNIT() {
    std::unordered_map<std::string, std::vector<std::string>> unit_graph;

    auto is_unit_edge = [&](const auto& rhs) {
        return rhs.size() == 1 && grammar_.non_terminals.contains(rhs[0]);
    };

    for (auto& [lhs, rhses] : grammar_.productions) {
        for (size_t i = 0; i < rhses.size(); ++i) {
            auto& rhs = rhses[i];

            if (is_unit_edge(rhs)) {
                unit_graph[lhs].push_back(rhs[0]);
            }
        }
    }

    const auto& grammar = grammar_;
    auto dfs = [&](
        auto&& self,
        const std::string& node,
        std::unordered_set<std::string>& visited,
        std::vector<Grammar::RHS>& ans
    ) -> void {
        if (visited.contains(node)) return;
        visited.insert(node);

        auto pit = grammar.productions.find(node);
        if (pit != grammar.productions.end()) {
            for (const auto& rhs : pit->second) {
                if (!is_unit_edge(rhs)) {
                    ans.push_back(rhs);
                }
            }
        }

        auto it = unit_graph.find(node);
        if (it != unit_graph.end()) {
            for (auto& child : it->second) {
                self(self, child, visited, ans);
            }
        }
    };

    Grammar::Productions new_productions;
    for (const auto& [lhs, _] : grammar_.productions) {
        std::vector<Grammar::RHS> new_rhses;
        std::unordered_set<std::string> visited;
        dfs(dfs, lhs, visited, new_rhses);
        new_productions[lhs] = std::move(new_rhses);
    }

    grammar_.productions = std::move(new_productions);
}


