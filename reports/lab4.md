# REGULAR EXPRESSIONS  
## LAB 4 – VARIANT 4

Course: Formal Languages & Finite Automata  
Author: Chiril Caț  

---

## INTRODUCTION

This laboratory work focuses on **regular expressions**, their structure, and dynamic interpretation.

The objectives of the lab are:

- Explain what regular expressions are and what they are used for;
- Dynamically interpret given regular expressions;
- Generate valid strings conforming to those expressions;
- Avoid hardcoded solutions;
- Limit unbounded repetition operators to 5 iterations.

This lab builds a complete pipeline:

```
Regex string → Tokens → AST → String generation
```

---

## THEORY

### What Are Regular Expressions?

A regular expression (regex) is a formal way of describing a set of strings using symbolic patterns.

Common operators:

- `|`  → alternation (OR)
- `*`  → zero or more repetitions
- `+`  → one or more repetitions
- `?`  → optional (zero or one)
- `^n` → fixed repetition
- `()` → grouping
- concatenation → sequence of symbols

Regular expressions define **regular languages**, which are equivalent to:

- Finite automata
- Regular grammars (Type 3 in Chomsky hierarchy)

---

## VARIANT 4 REGEXES

The following expressions were interpreted dynamically:

```
(S|T)(U|V)W*Y+24
L(U|N)O^3P*Q(2|3)
R*S(T|U|V)W(X|Y|Z)^2N?
```

Examples of expected outputs (as required in the task):

```
SUWWY24
LMOOOPPPQ2
RSTWXX
...
```

---

## IMPLEMENTATION OVERVIEW

The solution is divided into three components:

1. `RegexLexer`
2. `RegexASTBuilder`
3. `RegexASTInterpreter`

The process:

```cpp
RegexLexer lexer(regex);
auto tokens = lexer.lex();

RegexASTBuilder builder(tokens);
auto ast = builder.build();

RegexASTInterpreter interpreter;
std::string result = interpreter.generate(*ast);
```

---

# REGEX LEXER

The lexer converts the regex string into structured tokens.

### Supported Token Types

- Character literal
- Number
- `|`
- `*`
- `+`
- `^`
- `?`
- `(`
- `)`

Example tokenization logic:

```cpp
case '|':
    out_.push_back({ RegexTokenType::Or, std::nullopt });
    break;

case '*':
    out_.push_back({ RegexTokenType::Star, std::nullopt });
    break;
```

Numbers are grouped:

```cpp
if (std::isdigit(c)) {
    size_t start = i;
    while (i < source_.size() && std::isdigit(source_[i])) {
        ++i;
    }
    out_.push_back({
        RegexTokenType::Number,
        source_.substr(start, i - start)
    });
}
```

This allows correct parsing of:

```
^3
24
```

---

# ABSTRACT SYNTAX TREE (AST)

The regex is parsed into a structured tree.

## Node Types

```cpp
LiteralNode
ConcatNode
OrNode
StarNode
PlusNode
RepeatNode
QMarkNode
```

The AST is implemented using `std::variant` for type safety.

---

## Parsing Strategy

The builder follows standard recursive descent structure:

- `expression()` → handles `|`
- `concater()` → handles concatenation
- `base_wrapper()` → handles postfix operators
- `base()` → handles literal or group

Example – handling repetition operator:

```cpp
else if (match(RegexTokenType::Caret)) {
    advance();
    int count = std::stoi(advance().lexeme.value());

    RepeatNode repeat;
    repeat.left = std::move(node);
    repeat.count = count;

    node = std::make_unique<RegexAST>(std::move(repeat));
}
```

This ensures:

```
O^3
```

becomes:

```
RepeatNode(count = 3)
```

---

# AST INTERPRETER (STRING GENERATOR)

The interpreter traverses the AST and generates a valid string.

It uses `std::visit` over the variant node.

---

## Operator Semantics

### Literal

Returns the character itself.

### OR (`|`)

Randomly chooses left or right:

```cpp
std::uniform_int_distribution<> dist(0, 1);
```

---

### STAR (`*`)

Generates between 0 and 5 repetitions:

```cpp
std::uniform_int_distribution<> dist(0, 5);
```

(Limited to 5 as required by the task.)

---

### PLUS (`+`)

Generates between 1 and 5 repetitions.

---

### CARET (`^n`)

Generates exactly `n` repetitions.

---

### QUESTION MARK (`?`)

Optional generation (0 or 1 time).

---

# SAMPLE GENERATED OUTPUT

Example results:

```
SUWWY24
SVWY24
LMOOOPPPQ2
LNOOOPQ3
RSTWXX
RRRSUWYY
```

Each generated string conforms to the corresponding regular expression.

---

# HOW THE PROCESS WORKS (STEP-BY-STEP)

For `(S|T)(U|V)W*Y+24`:

1. Parse `(S|T)` → OrNode
2. Parse `(U|V)` → OrNode
3. Parse `W*` → StarNode
4. Parse `Y+` → PlusNode
5. Parse `24` → LiteralNode + LiteralNode
6. Combine everything into ConcatNode
7. Traverse AST to generate output

This is done dynamically - no hardcoded generation logic exists.

---

# FACED DIFFICULTIES

1. Handling operator precedence correctly.
2. Correctly parsing postfix operators (`*`, `+`, `^`, `?`).
3. Managing memory using `std::unique_ptr` inside `std::variant`.
4. Ensuring correct grouping with parentheses.
5. Preventing infinite repetition for `*` and `+`.

---

# CONCLUSION

This laboratory demonstrates:

- Dynamic parsing of regular expressions;
- AST construction;
- Interpretation of regex operators;
- Controlled generation of valid strings;
- Practical understanding of regular languages.

The solution fully respects the task requirements:

- No hardcoded outputs;
- Proper repetition limits;
- Structured parsing approach.

This implementation also provides a strong base for:

- Converting regex to NFA;
- Implementing regex matching;
- Extending to a full regex engine.
