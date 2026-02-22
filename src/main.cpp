#include <iostream>
#include <unordered_set>
#include <fstream>
#include <sstream>

#include "grammar_classifier.hpp"
#include "finite_automaton.hpp"
#include "grammar.hpp"
#include "lexer.hpp"
#include "regex_ast.hpp"
#include "regex_ast_interpreter.hpp"
#include "regex_lexer.hpp"
#include "chomsky_normal_form.hpp"

constexpr int n = 5;
void solve_lab1() {
    GrammarGenerator grammar_generator;

    std::vector<std::string> grammar_generator_strings(n);
    for (int i = 0; i < n; ++i) {
        grammar_generator_strings[i] = grammar_generator.generate_string();
    }

    std::cout << "Generated the following strings :" << '\n';
    std::cout << "------------------------" << "\n";
    for (int i = 0; i < n; ++i) {
        std::cout << grammar_generator_strings[i] << '\n';
    }
    std::cout << "\n\n";

    std::cout << "Validating the strings: " << '\n';
    std::cout << "------------------------" << "\n";

    FiniteAutomaton fa = grammar_generator.to_finite_automaton();
    for (int i = 0; i < n; ++i) {
        std::cout << grammar_generator_strings[i] << ' ' << (fa.validate_string(grammar_generator_strings[i]) == 1 ? "YES" : "NO") << '\n';
    }

    std::cout << "abcdefgabcdefggg" << ' ' << (fa.validate_string("abcdefgabcdefggg") == 1 ? "YES" : "NO") << '\n';
    std::cout << "\n\n";
}

void solve_lab2() {
    const std::unordered_set<char> non_terminal = { 'S', 'L', 'D' };
    const std::unordered_set<char> terminal = { 'a', 'b', 'c','d','e','f','j'};
    const std::unordered_map<std::string, std::vector<std::string>> grammar = {
        { "S", { "aS", "bS", "cD", "dL", "e" } },
        { "L", { "eL", "fL", "jD", "e" } },
        { "D", { "eD", "d" } }
    };

    GrammarClassifier grammar_classifier(grammar, non_terminal, terminal);
    std::cout << "Lab 1 grammar is of type " << grammar_classifier.classify_grammar() << '\n';
    std::cout << "------------------------" << "\n\n";

    //Q = {q0,q1,q2,q3},
    //∑ = {a,b},
    //F = {q3},
    //δ(q0,a) = q1,
    //δ(q0,a) = q2,
    //δ(q1,b) = q1,
    //δ(q1,a) = q2,
    //δ(q2,a) = q1,
    //δ(q2,b) = q3.

    States states = { "q0", "q1", "q2", "q3" };
    Alphabet alphabet = { 'a', 'b' };
    InitialState initial = "q0";
    FinalStates finals = { "q3" };
    Transitions transitions;
    transitions[{ "q0", 'a' }].insert("q1");
    transitions[{ "q0", 'a' }].insert("q2");
    transitions[{ "q1", 'b' }].insert("q1");
    transitions[{ "q1", 'a' }].insert("q2");
    transitions[{ "q2", 'a' }].insert("q1");
    transitions[{ "q2", 'b' }].insert("q3");

    FiniteAutomaton fa{
        std::move(states),
        std::move(alphabet),
        std::move(initial),
        std::move(finals),
        std::move(transitions),
    };

    Productions fa_regular_grammar = fa.to_regular_grammar();
    std::cout << "Variant 4 FA: " << '\n';
    std::cout << "------------------------" << "\n";
    fa.print_fa();
    std::cout << "\n\n";

    std::cout << "Variant 4 FA is " << (fa.is_deterministic() ? "FOR SURE" : "NOT") << " deterministic" << '\n';
    std::cout << "------------------------" << "\n\n";

    std::cout << "Regular grammar from variant 4 FA: " << '\n';
    std::cout << "------------------------" << '\n';
    for (const auto& [lhs, rhses] : fa_regular_grammar) {
        for (const auto& rhs : rhses) {
            std::cout << lhs << " -> " << rhs << '\n';
        }
    }
    std::cout << "\n\n";

    FiniteAutomaton dfa_variant = fa.convert_to_dfa();

    std::cout << "NDFA to DFA conversion for variant 4: " << '\n';
    std::cout << "------------------------" << '\n';
    dfa_variant.print_fa();
    std::cout << "\n\n";
}

void solve_lab3(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open file: " << path << "\n";
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    Lexer lexer(buffer.str());

    auto tokens = lexer.lex();
    lexer.debug_log(tokens);
};

void solve_lab4() {
    std::vector<std::string> regexes = {
        "(S|T)(U|V)W*Y+24",
        "L(U|N)O^3P*Q(2|3)",
        "R*S(T|U|V)W(X|Y|Z)^2"
    };

    for (const auto& regex : regexes) {
        RegexLexer lexer(regex);
        auto tokens = lexer.lex();

        RegexASTBuilder builder(tokens);
        auto ast = builder.build();

        RegexASTInterpreter interpreter;
        std::string result = interpreter.generate(*ast);

        std::cout << result << '\n';
    }
}

void solve_lab5() {
    Grammar grammar;

    grammar.start_symbol = "S";
    grammar.non_terminals = {"S", "A", "B", "C", "D"};
    grammar.terminals = {"a", "b"};
    grammar.productions = {
        {"S", {{"a", "B"}, {"A"}}},
        {"A", {{"B"}, {"A", "S"}, {"b", "B", "A", "B"}, {"b"}}},
        {"B", {{"b"}, {"b", "S"}, {"a", "D"}, {}}},
        {"D", {{"A", "A"}}},
        {"C", {{"B", "a"}}}
    };
    grammar.print_grammar();

    ChomskyNormalForm chomsky_normal_form(grammar);
    chomsky_normal_form.normalize();
    Grammar normalized_grammar = chomsky_normal_form.result();

    normalized_grammar.print_grammar();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <lab_number>\n";
        return 1;
    }

    int lab = std::atoi(argv[1]);

    switch (lab) {
        case 1:
            solve_lab1();
            break;
        case 2:
            solve_lab2();
            break;
        case 3:
            if (argc < 3) {
                std::cerr << "Lab 3 requires file path\n";
                return 1;
            }

            solve_lab3(argv[2]);
            break;

        case 4:
            solve_lab4();
            break;

        case 5:
            solve_lab5();
            break;

        default:
            std::cerr << "Invalid lab number\n";
            return 1;
    }

    return 0;
}
