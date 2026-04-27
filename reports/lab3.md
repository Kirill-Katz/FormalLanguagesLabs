# LEXER & SCANNER
## LAB 3

Course: Formal Languages & Finite Automata
Author: Chiril Caț

---

## INTRODUCTION

This laboratory work focuses on lexical analysis, one of the first major stages in the implementation of a compiler or interpreter. The goal is to read a source file, identify the meaningful elements that appear inside it, and convert those elements into a structured token stream that can be used by later stages of the project.

Instead of choosing a very small arithmetic example, the implementation uses a compact DSL-like input language with declarations, assignments, function calls, numbers, string literals, and a final return statement. This makes the scanner more realistic while still keeping the scope manageable for a laboratory exercise.

---

## THEORY

Lexical analysis is the process of converting raw source text into a sequence of tokens. A token is not simply the text fragment itself, but the category assigned to that fragment according to the language rules. In the input `return res`, the lexemes are the substrings `"return"` and `"res"`, while the token categories are `Return` and `Identifier`.

The lexer does not try to understand the full structure of the program. It does not check whether an expression is valid or whether a statement has the correct form. Its task is limited to reading characters, grouping them into lexemes, and assigning a token type to each recognized unit.

---

## INPUT LANGUAGE

The scanner targets the DSL used in the repository. The sample input contains typed declarations, assignments, function calls, floating-point values, string literals, and a final return statement:

```text
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

This example is rich enough to exercise several token categories without introducing unnecessary grammar complexity at the lexical stage.

---

## TOKEN MODEL

The token types defined in the project are:

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

These categories allow the lexer to distinguish identifiers, punctuation, numeric literals, strings, keywords, and type names. The separation between `Integer` and `Float` is especially useful because later compiler stages often need to preserve that difference rather than treating all numeric values the same way.

---

## LEXER STRUCTURE

The scanner is implemented in a `Lexer` class that receives the full source text and produces a vector of tokens:

```cpp
class Lexer {
public:
    Lexer(const std::string src);
    std::vector<Token> lex();
    void debug_log(const std::vector<Token>& tokens);
};
```

The `lex()` method performs the tokenization, while `debug_log()` prints the resulting token sequence for inspection. Internally, the lexer processes the input from left to right using an index pointer and dispatches to dedicated scanning routines depending on the current character.

---

## TOKENIZATION PROCESS

Single-character tokens such as parentheses, commas, colons, and the assignment operator are recognized directly through a helper function:

```cpp
static const std::optional<TokenType> single_char_token(char c);
```

Identifiers begin with a letter or underscore and continue with alphanumeric characters. After the full lexeme is scanned, it is checked against a keyword map so that reserved words and known types can be distinguished from ordinary identifiers:

```cpp
const std::unordered_map<std::string, TokenType> identifier_token = {
    {"return", TokenType::Return},
    {"VecF64", TokenType::Type},
};
```

This makes it possible to separate `Identifier("res")`, `Return("return")`, and `Type("VecF64")`, even though all of them are scanned through the same identifier routine.

Numbers are handled by `scan_number()`. If the lexeme contains no decimal point, the token is classified as `Integer`. If it does contain a decimal point, the token becomes `Float`. Invalid number forms cause runtime errors instead of being accepted silently.

String literals are scanned by `scan_string_literal()`. The lexer verifies that the closing quote exists and that a newline does not appear before the string is terminated. This is enough for the DSL used in the report, where strings are mainly needed for file-path arguments such as `./some_csv.csv`.

Whitespace is treated selectively. Ordinary spaces are ignored, but line breaks are preserved as `NewLine` tokens because the source format is line-oriented. At the end of the input, the lexer appends an `End` token to mark the termination of the stream.

---

## EXECUTION

The laboratory driver reads the source file into memory, creates a `Lexer` instance, runs tokenization, and prints the resulting tokens:

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

This setup is intentionally simple and makes the lexical stage easy to test during evaluation.

---

## RESULT

For the provided sample file, the lexer produces a token stream that begins with entries such as `Identifier('A')`, `Colon`, `Type('VecF64')`, and `NewLine`, and ends with `Return('return')`, `Identifier('res')`, `NewLine`, and `End`. The output shows that declarations, assignments, function calls, floating-point values, string literals, and the final return statement are all recognized correctly.

The produced token stream is no longer just raw text. It is already structured enough to serve as the input for the parser introduced in the next laboratory work.

---

## ERROR HANDLING

The implementation throws runtime errors when it encounters unexpected characters, unterminated string literals, or malformed floating-point numbers. This behavior is important because it prevents silent failure and makes the lexer reliable as a front-end component. A scanner that reports problems immediately is much easier to test and extend than one that accepts invalid input without complaint.

---

## CONCLUSION

This laboratory work demonstrates the practical implementation of lexical analysis for a small DSL. The lexer identifies token categories for identifiers, keywords, numbers, strings, and structural symbols, preserves line boundaries where needed, and produces a clean token stream suitable for later syntactic analysis. As a result, the project now has a solid lexical foundation for parsing and Abstract Syntax Tree construction.

---

## REFERENCES

[1] https://en.wikipedia.org/wiki/Lexical_analysis
[2] https://en.wikipedia.org/wiki/Lexer
