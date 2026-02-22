#pragma once

#include <string>
#include <optional>
#include <unordered_map>
#include <vector>
#include <set>
#include <iostream>
#include <algorithm>

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

static inline bool is_non_terminal(const Grammar& g, const std::string& s) {
    return g.non_terminals.contains(s);
}

static inline bool is_terminal(const Grammar& g, const std::string& s) {
    return g.terminals.contains(s);
}

static inline bool start_symbol_appears_on_rhs(const Grammar& g) {
    for (const auto& [lhs, rhses] : g.productions) {
        (void)lhs;
        for (const auto& rhs : rhses) {
            for (const auto& sym : rhs) {
                if (sym == g.start_symbol) {
                    return true;
                }
            }
        }
    }
    return false;
}

static inline bool is_cnf(const Grammar& g) {
    if (!g.non_terminals.contains(g.start_symbol)) {
        return false;
    }

    if (start_symbol_appears_on_rhs(g)) {
        return false;
    }

    for (const auto& [lhs, rhses] : g.productions) {
        if (!g.non_terminals.contains(lhs)) {
            return false;
        }

        for (const auto& rhs : rhses) {
            if (rhs.empty()) {
                if (lhs != g.start_symbol) {
                    return false;
                }
                continue;
            }

            if (rhs.size() == 1) {
                if (!is_terminal(g, rhs[0])) {
                    return false;
                }
                continue;
            }

            if (rhs.size() == 2) {
                if (!is_non_terminal(g, rhs[0]) || !is_non_terminal(g, rhs[1])) {
                    return false;
                }
                continue;
            }

            return false;
        }
    }

    return true;
}

static inline std::string serialize_state(const std::vector<std::string>& state) {
    std::string out;
    out.reserve(state.size() * 4);
    for (const auto& s : state) {
        out.append(s);
        out.push_back('\x1f');
    }
    return out;
}

static inline int terminal_count(const Grammar& g, const std::vector<std::string>& state) {
    int cnt = 0;
    for (const auto& s : state) {
        if (is_terminal(g, s)) {
            cnt++;
        }
    }
    return cnt;
}

static inline bool has_non_terminal(const Grammar& g, const std::vector<std::string>& state) {
    for (const auto& s : state) {
        if (is_non_terminal(g, s)) {
            return true;
        }
    }
    return false;
}

static inline std::optional<size_t> leftmost_non_terminal_pos(const Grammar& g, const std::vector<std::string>& state) {
    for (size_t i = 0; i < state.size(); ++i) {
        if (is_non_terminal(g, state[i])) {
            return i;
        }
    }
    return std::nullopt;
}

static inline std::string to_terminal_string(const Grammar& g, const std::vector<std::string>& state) {
    std::string out;
    for (const auto& s : state) {
        if (is_terminal(g, s)) {
            out.append(s);
        } else {
            out.append("?");
        }
    }
    return out;
}

struct CNFCheckResult {
    bool cnf_ok;
};

static inline CNFCheckResult check_cnf(
    const Grammar& cnf
) {
    CNFCheckResult out{};
    out.cnf_ok = is_cnf(cnf);
    return out;
}
