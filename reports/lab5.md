# CHOMSKY NORMAL FORM (CNF)  
## LAB 5

Course: Formal Languages & Finite Automata  
Author: Chiril Caț  

---

## INTRODUCTION

This laboratory work focuses on **Chomsky Normal Form (CNF)** and the normalization of context-free grammars.

The objectives of the lab are:

- Understand what Chomsky Normal Form is;
- Learn the normalization procedure;
- Implement a transformation method that converts an arbitrary grammar into CNF;
- Test the correctness of the transformation.

The implementation is generic and works for arbitrary grammars, not only for a specific variant.

---

## THEORY

### Chomsky Normal Form

A grammar is in **Chomsky Normal Form** if every production has one of the following forms:

1. **A → BC**  
   (two non-terminals)

2. **A → a**  
   (a single terminal)

3. **S → ε**  
   (only if S is the start symbol and ε is in the language)

Additionally:

- The start symbol must not appear on the right-hand side.
- No unit productions (A → B).
- No useless symbols.
- No mixed terminal/non-terminal RHS of length ≥ 2.

CNF is important because:

- It simplifies parsing algorithms (e.g., CYK);
- It provides structural uniformity;
- It ensures derivations are binary.

---

# IMPLEMENTATION OVERVIEW

The normalization is implemented inside:

```cpp
class ChomskyNormalForm {
public:
    explicit ChomskyNormalForm(Grammar g);
    void normalize();
    Grammar result() const;
};
```

Main driver:

```cpp
ChomskyNormalForm chomsky_normal_form(grammar);
chomsky_normal_form.normalize();
Grammar normalized = chomsky_normal_form.result();
```

---

# NORMALIZATION PIPELINE

The transformation is executed in the following order:

```
START → TERM → BIN → DEL → UNIT → cleanup
```

Full sequence:

```cpp
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
```

This ordering avoids exponential blow-up and keeps the grammar minimal.

---

# STEP-BY-STEP TRANSFORMATIONS

---

## START – New Start Symbol

A fresh start symbol is introduced to ensure:

- The start symbol does not appear on the RHS.

```cpp
void START() {
    std::string start_sym = fresh_non_terminal("S");
    grammar_.productions.insert({ start_sym, { { grammar_.start_symbol } } });
    grammar_.start_symbol = start_sym;
}
```

---

## TERM – Replace Terminals in Long Rules

If a production has length ≥ 2 and contains terminals, each terminal is replaced with a new non-terminal:

```
A → aB  becomes
A → N_a B
N_a → a
```

Implementation idea:

```cpp
if (grammar_.terminals.contains(sym)) {
    sym = processed_terminals.at(sym);
}
```

---

## BIN – Binarization

Rules longer than 2 symbols are broken into binary form:

```
A → B C D E
```

Becomes:

```
A → B X1
X1 → C X2
X2 → D E
```

Implementation creates fresh non-terminals dynamically:

```cpp
std::string prev = fresh_non_terminal("A");
```

---

## DEL – Eliminate ε-Productions

Steps:

1. Compute nullable non-terminals.
2. Generate all combinations where nullable symbols may be removed.
3. Remove ε except possibly for the start symbol.

Core idea:

```cpp
if (rhs.empty()) {
    propagate_nullability(lhs);
}
```

All nullable combinations are generated carefully without using recursion explosion.

---

## UNIT – Eliminate Unit Productions

Unit productions:

```
A → B
```

Are removed by constructing a graph of unit dependencies and propagating reachable non-unit rules.

Approach:

```cpp
if (rhs.size() == 1 && grammar_.non_terminals.contains(rhs[0])) {
    unit_graph[lhs].push_back(rhs[0]);
}
```

Then DFS propagates all reachable productions.

---

## Remove Useless Symbols

Two cleanup passes:

### Inaccessible symbols
Symbols not reachable from the start symbol are removed.

### Non-productive symbols
Symbols that cannot derive terminal strings are removed.

This ensures the resulting grammar is minimal and valid.

---

# CNF VALIDATION

A CNF checker was implemented:

```cpp
static inline bool is_cnf(const Grammar& g);
```

Validation rules:

- Only A → BC
- Only A → a
- Only S → ε
- Start symbol not on RHS

Driver:

```cpp
auto r = check_cnf(normalized_grammar);
if (r.cnf_ok) {
    std::cout << "CNF IS CORRECT\n";
}
```

---

# TESTED GRAMMARS

Two grammars were tested:

1. Custom complex grammar with ε-productions, unit rules, and mixed RHS.
2. Wikipedia example grammar.

Both were successfully transformed into valid CNF.

Example output:

```
Start symbol: S
Non-terminals: {A, B, C, D, S}
Terminals: {a, b}

A -> B | A S | b B A B | b
B -> b | b S | a D | ε
C -> B a
D -> A A
S -> a B | A
Start symbol: S0
Non-terminals: {A, A0, A1, B, D, Na, Nb, S, S0}
Terminals: {a, b}

A -> A S | Na B | Na D | Nb A0 | Nb S | a | b
A0 -> A B | A S | B A1 | Na B | Na D | Nb A0 | Nb S | a | b
A1 -> A B | A S | Na B | Na D | Nb A0 | Nb S | a | b
B -> Na D | Nb S | a | b
D -> A A | A S | Na B | Na D | Nb A0 | Nb S | a | b
Na -> a
Nb -> b
S -> A S | Na B | Na D | Nb A0 | Nb S | a | b
S0 -> ε | A S | Na B | Na D | Nb A0 | Nb S | a | b
=================
CNF IS CORRECT
=================
Start symbol: S0
Non-terminals: {A, A0, B, C, Nb, S0}
Terminals: {a, b, c}

A -> a
A0 -> Nb B | b
B -> A A | A C | a | b | c
C -> b | c
Nb -> b
S0 -> A A0 | Nb B | b | c
=================
CNF IS CORRECT
=================
```

---

# FACED DIFFICULTIES

1. Correct handling of nullable symbol combinations.
2. Avoiding iterator invalidation during rule modification.
3. Preventing exponential growth of productions.
4. Proper ordering of transformation steps.
5. Ensuring start symbol rules remain valid.

The ordering of transformations was crucial to avoid blow-up.

---

# CONCLUSION

This laboratory demonstrates:

- Complete CNF normalization pipeline;
- Removal of ε-productions;
- Removal of unit productions;
- Binarization of long rules;
- Elimination of useless symbols;
- Validation of CNF correctness.

The implementation is:

- Generic (accepts arbitrary grammars);
- Modular;
- Fully tested.

It provides a strong foundation for implementing parsing algorithms such as CYK in future work.
