# REGULAR GRAMMAR AND FINITE AUTOMATON  
## VARIANT 4

Course: Formal Languages & Finite Automata  
Author: Chiril Caț

---

## INTRODUCTION

This laboratory work presents the implementation of a **regular grammar** and its transformation into a **finite automaton**.

The objective is to:

- Implement a grammar structure;
- Generate valid strings from the grammar;
- Convert the grammar into a finite automaton;
- Validate strings using the automaton.

This demonstrates the theoretical equivalence between **regular grammars (Type 3)** and **finite automata**.

---

## THEORY

A formal language is defined as a set of strings over a finite alphabet.

A regular grammar has production rules of the form:

- **A → aB**
- **A → a**

Each non-terminal corresponds to a state in a finite automaton.

A finite automaton is defined as:

- **Q** – set of states  
- **Σ** – alphabet  
- **δ** – transition function  
- **q₀** – initial state  
- **F** – set of final states  

There exists a one-to-one correspondence between right-linear grammars and finite automata.

---

## GRAMMAR DEFINITION

Non-terminals:  
VN = { S, L, D }

Terminals:  
VT = { a, b, c, d, e, f, j }

Start symbol:  
S

Productions:

S → aS  
S → bS  
S → cD  
S → dL  
S → e  

L → eL  
L → fL  
L → jD  
L → e  

D → eD  
D → d  

This is a **right-linear grammar**, therefore it is regular.

---

## IMPLEMENTATION

The project is implemented in **C++**.

Two main classes are used:

- `GrammarGenerator`
- `FiniteAutomaton`

---

### GrammarGenerator Class

The class stores:

- Non-terminals
- Terminals
- Production rules

```cpp
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
};
```

### String Generation

The `generate_string()` method:

1. Starts from the start symbol `S`;
2. Randomly selects one production rule;
3. Replaces non-terminals until only terminals remain;
4. Returns a valid string from the language.

---

### Conversion to Finite Automaton

The conversion follows the theoretical mapping:

- Each non-terminal → state
- Each production `A → aB` → transition δ(A, a) = B
- Each production `A → a` → transition δ(A, a) = F (final state)

Constructor used:

```cpp
FiniteAutomaton(const Productions& P, char start_symbol);
```

A special final state `"F"` is added.

---

### FiniteAutomaton Class

The automaton stores:

- States
- Alphabet
- Transitions
- Initial state
- Final states

Validation method:

```cpp
bool validate_string(const std::string& input) const;
```

Algorithm:

1. Start from initial state.
2. For each input symbol:
   - Compute next states using δ.
3. After processing input:
   - If at least one current state is final → ACCEPT.
   - Otherwise → REJECT.

The automaton supports:

- NFA representation
- Determinism check
- Subset construction to DFA
- Conversion back to regular grammar

Only validation is required for Lab 1, but the structure supports later labs.

---

## EXECUTION (LAB 1)

Client function:

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

Example output:
```
Generated the following strings :
------------------------
cd
aadeefe
badefjd
ade
e


Validating the strings: 
------------------------
cd YES
aadeefe YES
badefjd YES
ade YES
e YES
abcdefgabcdefggg NO
```

Generated strings are accepted by the automaton.

Invalid test strings are correctly rejected.

---

## CONCLUSION

The laboratory work confirms the equivalence between:

- Regular grammars
- Finite automata

The implemented system:

- Generates valid strings using production rules;
- Converts grammar into an NFA;
- Validates strings through state transitions.

The design also supports further development for future laboratories (determinization, grammar conversion, etc.).
