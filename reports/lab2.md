# DETERMINISM IN FINITE AUTOMATA. NDFA -> DFA. CHOMSKY HIERARCHY
## VARIANT 4

Course: Formal Languages & Finite Automata
Author: Chiril Caț

---

## INTRODUCTION

This laboratory work extends the implementation from the previous task by studying grammar classification, determinism in finite automata, and the conversion processes between automata and grammars. The objective is to connect these topics inside the same codebase rather than treat them as isolated theory.

The work continues in the same repository and reuses the structures developed earlier. This makes the task cumulative in a useful way, because the project evolves from a simple generator and recognizer into a broader toolkit for formal language analysis.

---

## THEORY

The Chomsky hierarchy classifies grammars into four major categories: Type 0, Type 1, Type 2, and Type 3. A grammar is regular, or Type 3, when its productions follow a restricted linear form such as `A -> aB` or `A -> a`. Because of that restriction, regular grammars correspond directly to finite automata.

A finite automaton is deterministic when each state has at most one outgoing transition for a given symbol. If the same state and input symbol may lead to multiple next states, then the automaton is non-deterministic. Even in that case, the language recognized by the NDFA can still be recognized by an equivalent DFA obtained through subset construction.

Another important equivalence is the conversion from automata back to regular grammars. If each transition is turned into a production, the behavior of the automaton can be described again in grammar form. This duality is one of the central ideas verified in this laboratory.

---

## GRAMMAR CLASSIFICATION

The grammar reused from Lab 1 is:

```text
S -> aS | bS | cD | dL | e
L -> eL | fL | jD | e
D -> eD | d
```

To classify the grammar, the project introduces a `GrammarClassifier` class. The classification checks the strongest form first and then proceeds downward through the hierarchy:

```cpp
int GrammarClassifier::classify_grammar() {
    if (check_3()) return 3;
    if (check_2()) return 2;
    if (check_1()) return 1;
    return 0;
}
```

The regularity check verifies that the left-hand side always contains exactly one non-terminal, that the right-hand side has the correct restricted length, and that the grammar maintains a consistent linear direction. For the grammar above, the result is Type 3, which confirms that the grammar is regular.

---

## FINITE AUTOMATON ANALYSIS

The automaton for variant 4 has the state set `Q = {q0, q1, q2, q3}`, alphabet `Σ = {a, b}`, and final state set `F = {q3}`. Its transitions include:

```text
δ(q0,a) = q1
δ(q0,a) = q2
δ(q1,b) = q1
δ(q1,a) = q2
δ(q2,a) = q1
δ(q2,b) = q3
```

The critical detail is that `δ(q0, a)` leads to two different states. In the implementation, that appears directly as:

```cpp
transitions[{ "q0", 'a' }].insert("q1");
transitions[{ "q0", 'a' }].insert("q2");
```

Because the same state-symbol pair has more than one target, the automaton is non-deterministic. The project checks this property with:

```cpp
bool FiniteAutomaton::is_deterministic() const {
    for (const auto& [key, next_states] : transitions_) {
        if (next_states.size() > 1)
            return false;
    }
    return true;
}
```

When executed on the given automaton, the program reports that the variant 4 automaton is not deterministic.

---

## CONVERSION FROM AUTOMATON TO REGULAR GRAMMAR

The next step transforms the finite automaton into a regular grammar. Each transition `δ(qi, a) = qj` becomes a production of the form `Ai -> aAj`. If the destination state is final, then a terminal-only production is also introduced.

The core of that logic is:

```cpp
grammar[state_nt].push_back(std::string(1, c) + next_state_nt);

if (final_states_.contains(next_state)) {
    grammar[state_nt].push_back(std::string(1, c));
}
```

For the given automaton, the resulting grammar includes productions such as:

```text
B -> aC
B -> bB
A -> aC
A -> aB
C -> bF1
C -> b
C -> aB
```

This confirms that the language recognized by the automaton can be expressed again as a regular grammar.

---

## NDFA TO DFA CONVERSION

The final part of the laboratory applies subset construction in order to obtain a deterministic automaton. Each DFA state represents a set of original NDFA states. The process starts from the initial subset containing `q0` and explores all reachable subsets using breadth-first search.

The initialization of the algorithm is:

```cpp
States start = { initial_state_ };
bfs.push(start);
visited.insert(encode(start));
```

For each symbol, the algorithm computes the union of all next states reachable from the current subset:

```cpp
for (char a : alphabet_) {
    States next_union;
    ...
}
```

The resulting DFA contains states such as `q1|q2` and `q1|q3`, which encode subsets of the original NDFA states. A representative form of the final automaton is:

```text
Q = {q0,q1,q1|q2,q1|q3,q2,q3}
Σ = {a,b}
F = {q1|q3,q3}
```

After this transformation, every state-symbol pair has exactly one destination, which means the automaton is deterministic.

---

## RESULTS

The execution confirms all required goals of the laboratory. The grammar from Lab 1 is classified as Type 3, the variant 4 automaton is recognized as non-deterministic, the automaton is converted into a regular grammar, and the subset construction produces a valid DFA. The project therefore demonstrates the theoretical relationships through actual program behavior rather than only through definitions.

---

## CONCLUSION

This laboratory work extends the previous implementation into a more complete framework for grammar and automaton analysis. It classifies a grammar inside the Chomsky hierarchy, identifies non-determinism in a finite automaton, converts the automaton into an equivalent regular grammar, and determinizes the NDFA through subset construction. The result is both technically correct and reusable for later experiments with formal language models.

---

## REFERENCES

[1] https://en.wikipedia.org/wiki/Chomsky_hierarchy
[2] https://en.wikipedia.org/wiki/Nondeterministic_finite_automaton
[3] https://en.wikipedia.org/wiki/Powerset_construction
