# LEXER & SCANNER  
## LAB 3

Course: Formal Languages & Finite Automata  
Author: Chiril Caț  

---

## INTRODUCTION

This laboratory work focuses on **lexical analysis**, one of the first stages in the implementation of a compiler or interpreter.

The objective is to:

- Understand how a lexer/tokenizer works;
- Design token categories for a small domain-specific language;
- Implement a working lexer in C++;
- Demonstrate token extraction on a non-trivial input file.

The implementation follows the requirements described in the laboratory task.

---

## THEORY

### Lexical Analysis

Lexical analysis is the process of converting a sequence of characters into a sequence of tokens.

- A **lexeme** is the raw substring extracted from the source.
- A **token** is a categorized representation of that lexeme.

Example:

```
return res
```

Lexemes:
```
"return", "res"
```

Tokens:
```
Return, Identifier
```

The lexer does not evaluate expressions or understand grammar structure - it only categorizes input symbols.

---

## DESIGN

Instead of implementing a simple calculator, a small DSL-like structure was used.  
The input file supports:

- Variable declarations with types
- Function calls
- String literals
- Integers and floats
- Return statement

Example input:

```
A: VecF64
B: VecF64
C: VecF64

input: VecF64 = loadCSV("./some_csv.csv")

a = SumVec(A)
b = SumVec(B)
c = SumVec(C)

z = SumVals(3.3, 4.5)

temp = SumVals(a, b)
res = SumVals(temp, c)

return res
```

---

## TOKEN TYPES

The following token types were defined:

```cpp
enum class TokenType {
    Identifier,
    Equals,
    LParen,
    RParen,
    Comma,
    NewLine,
    Colon,
    StringLiteral,
    Integer,
    Float,
    Return,
    Type,
    End
};
```

This allows classification of:

- Identifiers
- Keywords (`return`)
- Types (`VecF64`)
- Numbers (int / float)
- String literals
- Structural symbols

---

## LEXER STRUCTURE

The `Lexer` class operates on a source string and produces a vector of tokens.

Core interface:

```cpp
class Lexer {
public:
    Lexer(const std::string src);
    std::vector<Token> lex();
    void debug_log(const std::vector<Token>& tokens);
};
```

---

## TOKENIZATION LOGIC

The lexer processes the input sequentially using an index pointer.

### 1. Single-character tokens

Recognized via helper function:

```cpp
static const std::optional<TokenType> single_char_token(char c);
```

Handles:
```
( ) , = :
```

---

### 2. Identifiers & Keywords

Identifiers begin with a letter or underscore and continue with alphanumeric characters.

```cpp
void Lexer::scan_identifier();
```

After reading the lexeme, it is checked against a keyword map:

```cpp
const std::unordered_map<std::string, TokenType> identifier_token = {
    {"return", TokenType::Return},
    {"VecF64", TokenType::Type},
};
```

This allows differentiation between:

```
Identifier("res")
Return("return")
Type("VecF64")
```

---

### 3. Numbers (Integer & Float)

Numbers are parsed as:

- Integer if no decimal point
- Float if decimal point exists

```cpp
void Lexer::scan_number();
```

Malformed floats trigger runtime errors.

Example recognized tokens:

```
Float('3.3')
Float('4.5')
Integer('42')
```

---

### 4. String Literals

Strings are enclosed in double quotes.

```cpp
void Lexer::scan_string_literal();
```

The lexer ensures:

- No newline inside a string
- Proper termination with closing quote

Example:

```
StringLiteral('./some_csv.csv')
```

---

### 5. Newlines & Whitespace

- Newlines are preserved as tokens.
- Other whitespace is ignored.
- End-of-file produces an `End` token.

---

## EXECUTION

The lab function:

```cpp
void solve_lab3(const std::string& path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();

    Lexer lexer(buffer.str());
    auto tokens = lexer.lex();
    lexer.debug_log(tokens);
}
```

---

## OUTPUT

For the provided sample file, the lexer produces:

```
Identifier('A')
Colon
Type('VecF64')
NewLine
Identifier('B')
Colon
Type('VecF64')
NewLine
Identifier('C')
Colon
Type('VecF64')
NewLine
NewLine
Identifier('input')
Colon
Type('VecF64')
Equals
Identifier('loadCSV')
LParen
StringLiteral('./some_csv.csv')
RParen
NewLine
...
Return('return')
Identifier('res')
NewLine
End
```

All constructs are correctly tokenized:

- Declarations
- Assignments
- Function calls
- Floats
- String literals
- Return statement

---

## ERROR HANDLING

The lexer throws runtime errors in case of:

- Unexpected characters
- Unterminated string literals
- Malformed floating point numbers

This prevents silent failure and ensures correctness of tokenization.

---

## CONCLUSION

This laboratory demonstrates:

- Practical implementation of lexical analysis;
- Differentiation between lexemes and tokens;
- Handling of identifiers, keywords, numbers, and strings;
- Proper token stream generation for a small DSL-like language.

The lexer is structured in a modular way and can be extended with:

- Additional keywords
- Operators
- Comments
- More complex token rules

This implementation provides a solid base for further compiler/interpreter stages such as parsing and AST construction.
