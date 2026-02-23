# DETERMINISM IN FINITE AUTOMATA. NDFA → DFA. CHOMSKY HIERARCHY  
## VARIANT 4

Course: Formal Languages & Finite Automata  
Author: Chiril Caț  

---

## INTRODUCTION

This laboratory work extends the previous implementation by focusing on:

- Grammar classification based on the Chomsky hierarchy;
- Determinism in finite automata;
- Conversion of a finite automaton to a regular grammar;
- Conversion of an NDFA to a DFA using subset construction.

The work continues in the same repository and builds on the previously implemented grammar and automaton structures, as required in the task :contentReference[oaicite:0]{index=0}.

---

## THEORY

### Chomsky Hierarchy

Grammars are classified into four types:

- Type 3 — Regular grammars  
- Type 2 — Context-free grammars  
- Type 1 — Context-sensitive grammars  
- Type 0 — Unrestricted grammars  

A grammar is **Type 3 (regular)** if productions are of the form:

- A → aB  
- A → a  

and the linearity (left or right) is consistent.

---

### Deterministic vs Non-Deterministic Finite Automata

A finite automaton is deterministic (DFA) if:

- For each state and each symbol in the alphabet,
- There is at most one transition.

If a transition leads to multiple states for the same symbol, the automaton is non-deterministic (NDFA).

Every NDFA can be converted into an equivalent DFA using **subset construction**.

---

## PART I — GRAMMAR CLASSIFICATION

The grammar from Lab 1 is reused:

```
S → aS | bS | cD | dL | e  
L → eL | fL | jD | e  
D → eD | d  
```

To classify the grammar, a `GrammarClassifier` class was implemented.

### Classification Strategy

The classification checks grammar properties in descending order:

1. Check Type 3 (regular)
2. Check Type 2 (context-free)
3. Check Type 1 (context-sensitive)
4. Otherwise → Type 0

Core method:

```cpp
int GrammarClassifier::classify_grammar() {
    if (check_3()) return 3;
    if (check_2()) return 2;
    if (check_1()) return 1;
    return 0;
}
```

The `check_3()` function verifies:

- Left-hand side contains exactly one non-terminal;
- Right-hand side length ≤ 2;
- Form is consistently left-linear or right-linear.

Result:

```
Lab 1 grammar is of type 3
```

Therefore, the grammar is **regular (Type 3)**.

---

## PART II — FINITE AUTOMATON ANALYSIS (VARIANT 4)

Given automaton:

```
Q = {q0,q1,q2,q3}
Σ = {a,b}
F = {q3}
```

Transitions:

```
δ(q0,a) = q1
δ(q0,a) = q2
δ(q1,b) = q1
δ(q1,a) = q2
δ(q2,a) = q1
δ(q2,b) = q3
```

Implementation snippet:

```cpp
transitions[{ "q0", 'a' }].insert("q1");
transitions[{ "q0", 'a' }].insert("q2");
```

Since δ(q0, a) leads to two states, the automaton is **non-deterministic**.

Determinism check:

```cpp
bool FiniteAutomaton::is_deterministic() const {
    for (const auto& [key, next_states] : transitions_) {
        if (next_states.size() > 1)
            return false;
    }
    return true;
}
```

Result:

```
Variant 4 FA is NOT deterministic
```

---

## PART III — CONVERSION FA → REGULAR GRAMMAR

Each transition:

```
δ(qi, a) = qj
```

becomes production:

```
A_i → aA_j
```

If `qj` is final, then:

```
A_i → a
```

Snippet:

```cpp
grammar[state_nt].push_back(std::string(1, c) + next_state_nt);

if (final_states_.contains(next_state)) {
    grammar[state_nt].push_back(std::string(1,c));
}
```

Resulting grammar:

```
B -> aC
B -> bB
A -> aC
A -> aB
C -> bF1
C -> b
C -> aB
```

This confirms the equivalence between finite automata and regular grammars.

---

## PART IV — NDFA → DFA CONVERSION

The conversion uses **subset construction**:

- Each DFA state represents a set of NDFA states.
- Start state = {q0}.
- BFS is used to explore reachable subsets.

Core logic:

```cpp
States start = { initial_state_ };
bfs.push(start);
visited.insert(encode(start));
```

For each symbol:

```cpp
for (char a : alphabet_) {
    States next_union;
    ...
}
```

Resulting DFA:

```
Q = {q0,q1,q1|q2,q1|q3,q2,q3}
Σ = {a,b}
F = {q1|q3,q3}
```

Transitions:

```
δ(q0,a) = q1|q2
δ(q1,a) = q2
δ(q1,b) = q1
δ(q1|q2,a) = q1|q2
δ(q1|q2,b) = q1|q3
...
```

Now every transition leads to exactly one state — the automaton is deterministic.

---

## RESULTS

Program output:

```
Lab 1 grammar is of type 3
------------------------

Variant 4 FA: 
------------------------
Q = {q0,q1,q2,q3},
Σ = {a,b},
F = {q3},
δ(q0,a) = q1,
δ(q0,a) = q2,
δ(q1,a) = q2,
δ(q1,b) = q1,
δ(q2,a) = q1,
δ(q2,b) = q3,


Variant 4 FA is NOT deterministic
------------------------

Regular grammar from variant 4 FA: 
------------------------
B -> aC
B -> bB
A -> aC
A -> aB
C -> bF1
C -> b
C -> aB


NDFA to DFA conversion for variant 4: 
------------------------
Q = {q0,q1,q1|q2,q1|q3,q2,q3},
Σ = {a,b},
F = {q1|q3,q3},
δ(q0,a) = q1|q2,
δ(q1,a) = q2,
δ(q1,b) = q1,
δ(q1|q2,a) = q1|q2,
δ(q1|q2,b) = q1|q3,
δ(q1|q3,a) = q2,
δ(q1|q3,b) = q1,
δ(q2,a) = q1,
δ(q2,b) = q3,
```

All required tasks from the laboratory were completed:

- Grammar classification;
- FA → regular grammar conversion;
- Determinism detection;
- NDFA → DFA conversion.

---

## CONCLUSION

This laboratory demonstrates:

- Formal classification of grammars using Chomsky hierarchy;
- Identification of non-determinism in finite automata;
- Equivalence between regular grammars and finite automata;
- Subset construction algorithm for determinization.

The implementation is generic and reusable for other grammars and automata, not limited to the specific variant.
