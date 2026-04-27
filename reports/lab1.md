# REGULAR GRAMMAR AND FINITE AUTOMATON
## VARIANT 4

Course: Formal Languages & Finite Automata
Author: Chiril Caț

---

## INTRODUCTION

This laboratory work presents the implementation of a regular grammar together with its equivalent finite automaton. The purpose of the task is to show, in a concrete program, how a grammar can generate strings from a language and how the same language can be recognized by an automaton.

The work covers the complete path from representation to validation. The grammar is modeled explicitly in code, used to generate valid strings, transformed into a finite automaton, and then used again in automaton form to validate whether a given string belongs to the language. In this way, the laboratory demonstrates the practical equivalence between regular grammars and finite automata.

---

## THEORY

A formal language is a set of strings defined over a finite alphabet. In the case of regular languages, that set can be described either by a regular grammar or by a finite automaton. A regular grammar uses restricted productions such as `A -> aB` or `A -> a`, where the right-hand side contains at most one non-terminal. Because of this restricted form, such grammars can be mapped directly to automata.

A finite automaton describes the same language through states and transitions. Each non-terminal from the grammar corresponds naturally to a state, and each production becomes one or more transitions. A rule such as `A -> aB` becomes a transition from state `A` to state `B` on symbol `a`, while a rule such as `A -> a` becomes a transition to a final state. This one-to-one correspondence is the theoretical basis of the implementation.

---

## GRAMMAR MODEL

The grammar used in this laboratory has the non-terminals `S`, `L`, and `D`, the terminal alphabet `{a, b, c, d, e, f, j}`, and the following production rules:

```text
S -> aS | bS | cD | dL | e
L -> eL | fL | jD | e
D -> eD | d
```

This grammar is right-linear because any non-terminal that appears on the right-hand side appears at the end of the production. For that reason, it is a regular grammar and can be transformed directly into a finite automaton.

---

## IMPLEMENTATION

The project is implemented in C++ and is centered around two classes named `GrammarGenerator` and `FiniteAutomaton`. The grammar class stores the non-terminals, terminals, and production rules, while the automaton class stores the states, alphabet, transitions, initial state, and final states.

The structure of the grammar object is shown below:

```cpp
class GrammarGenerator {
public:
    const std::vector<char> non_terminal = { 'S', 'L', 'D' };
    const std::vector<char> terminal = { 'a', 'b', 'c', 'd', 'e', 'f', 'j' };

    const Productions P = {
        { "S", { "aS", "bS", "cD", "dL", "e" } },
        { "L", { "eL", "fL", "jD", "e" } },
        { "D", { "eD", "d" } }
    };

    std::string generate_string() const;
    FiniteAutomaton to_finite_automaton() const;
};
```

The `generate_string()` method starts from the symbol `S` and repeatedly expands the current non-terminal until only terminal characters remain. Because the production choice is performed dynamically, the method can generate multiple valid strings from the same language.

The conversion to finite automaton follows the standard theoretical mapping. Every non-terminal becomes a state, every production of the form `A -> aB` becomes a transition, and every production of the form `A -> a` leads to a special final state. That logic is encapsulated by the constructor:

```cpp
FiniteAutomaton(const Productions& P, char start_symbol);
```

The resulting automaton provides a validation method:

```cpp
bool validate_string(const std::string& input) const;
```

Its task is to consume the input symbol by symbol, follow the transitions defined by the grammar-derived automaton, and decide whether the final configuration is accepting.

---

## EXECUTION

The laboratory driver creates the grammar, generates several sample strings, converts the grammar into a finite automaton, and validates each generated string. It also tests one intentionally invalid word in order to confirm that the recognizer rejects strings outside the language.

```cpp
void solve_lab1() {
    GrammarGenerator grammar_generator;

    std::vector<std::string> grammar_generator_strings(n);
    for (int i = 0; i < n; ++i) {
        grammar_generator_strings[i] = grammar_generator.generate_string();
    }

    std::cout << "Generated strings :" << '\n';

    FiniteAutomaton fa =
        grammar_generator.to_finite_automaton();

    for (int i = 0; i < n; ++i) {
        std::cout << grammar_generator_strings[i]
                  << ' '
                  << (fa.validate_string(grammar_generator_strings[i]) ? "YES" : "NO")
                  << '\n';
    }

    std::cout << "abcdefgabcdefggg"
              << ' '
              << (fa.validate_string("abcdefgabcdefggg") ? "YES" : "NO")
              << '\n';
}
```

---

## RESULTS

The program generates strings such as `cd`, `aadeefe`, `badefjd`, `ade`, and `e`, and then checks each of them with the automaton. All generated examples are accepted, which confirms that the automaton correctly recognizes strings produced by the grammar. The additional test string `abcdefgabcdefggg` is rejected, which shows that the implementation also distinguishes invalid inputs.

The experiment therefore validates both sides of the equivalence. The grammar acts as a producer of words, while the finite automaton acts as a recognizer for the same language.

---

## CONCLUSION

This laboratory work demonstrates the practical equivalence between regular grammars and finite automata. The grammar was represented explicitly, used to generate valid strings, and then transformed into a finite automaton capable of recognizing those strings. The result satisfies the objectives of the task and also provides a solid foundation for the later laboratories developed in the same repository.

---

## REFERENCES

[1] https://en.wikipedia.org/wiki/Formal_grammar
[2] https://en.wikipedia.org/wiki/Finite-state_machine

## CONCLUSION

The laboratory work confirms the equivalence between:

- Regular grammars
- Finite automata

The implemented system:

- Generates valid strings using production rules;
- Converts grammar into an NFA;
- Validates strings through state transitions.

The design also supports further development for future laboratories (determinization, grammar conversion, etc.).
