# PARSER & ABSTRACT SYNTAX TREE  
## LAB 6

Course: Formal Languages & Finite Automata  
Author: Chiril Caț  

---

## INTRODUCTION

This laboratory work extends the lexer from the previous DSL task with a parser and an Abstract Syntax Tree. The purpose of the implementation is to move from simple token extraction to syntactic analysis, so that the structure of the source program can be represented explicitly and checked against a grammar.

The same input language from the previous laboratory work is preserved. The lexer is reused and improved, while the new parser consumes the generated token sequence and builds a tree representation of the program. In this way, the project now contains both the lexical and syntactic stages of a small compiler front end.

---

## THEORY

Parsing is the process of analyzing a sequence of tokens according to the rules of a grammar. If lexical analysis answers the question of what the individual pieces of text are, parsing answers the question of how those pieces are combined and what role they have inside the language.

An Abstract Syntax Tree is a hierarchical representation of the program structure. It is called abstract because it keeps only the information that matters for syntax and later processing. Parentheses, separators, and similar concrete details are used during parsing, but the final tree stores only the essential constructs such as declarations, assignments, function calls, literals, and return statements.

For a language of this size, a recursive descent parser is a natural choice. Each parsing function corresponds to a grammar rule, which makes the implementation readable and easy to extend.

---

## LANGUAGE MODEL

The parser targets the same DSL used in `example.dsl`. The source file contains typed declarations, assignments, function calls, numeric literals, string literals, and a final return statement. A simplified form of the grammar is the following:

```text
program          ::= { statement }
statement        ::= declaration | assignment | return_statement
declaration      ::= IDENTIFIER ":" TYPE [ "=" expression ] NEWLINE
assignment       ::= IDENTIFIER "=" expression NEWLINE
return_statement ::= "return" expression NEWLINE
expression       ::= function_call | IDENTIFIER | STRING | INTEGER | FLOAT
function_call    ::= IDENTIFIER "(" [ expression { "," expression } ] ")"
```

This grammar is sufficient for the sample input already present in the repository:

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

---

## LEXER ADAPTATION

The previous lexer already defined token categories through `TokenType`, but for this laboratory work the token recognition step was refactored to rely on regular expressions, as required by the task. The lexer now uses an ordered list of regex rules to distinguish identifiers, keywords, numeric literals, string literals, punctuation symbols, and line breaks.

The most important categories are `Identifier`, `Return`, `Type`, `Integer`, `Float`, `StringLiteral`, `Equals`, `Colon`, `Comma`, `LParen`, `RParen`, `NewLine`, and `End`. Because the parser must report syntax errors precisely, each token now also carries line and column information.

This preserves the tokenizer from the previous work, but makes it more suitable for parsing and error reporting.

---

## AST DESIGN

The AST is implemented with a small hierarchy of node types. The root node is `Program`, which stores the list of statements in the source file. Statement nodes are represented through `TypeDeclarationStmt`, `AssignmentStmt`, and `ReturnStmt`. A declaration may also carry an initialization expression when the source contains both a type annotation and a value. Expression nodes are represented through `IdentifierExpr`, `LiteralExpr`, and `CallExpr`.

This structure mirrors the logical entities of the DSL instead of the surface syntax. For example, the assignment

```text
z = SumVals(3.3, 4.5)
```

is represented as an `AssignmentStmt` whose value is a `CallExpr` with two literal arguments. The comma and parentheses are needed by the parser, but they do not appear as separate nodes in the final tree.

---

## PARSER IMPLEMENTATION

The parser is implemented in `Parser` and uses recursive descent. It reads the token stream from left to right and decides which statement rule applies by inspecting the current token and one lookahead token. If the current token is an identifier followed by a colon, the parser reads a declaration. If the identifier is followed by an equals sign, the parser reads an assignment. If the current token is `return`, the parser reads a return statement.

Expressions are intentionally simple because the language itself is simple. The parser supports identifiers, literals, and function calls. A function call may contain multiple arguments, and each argument is parsed again as an expression, which allows nested calls if needed later.

Whenever the current token does not match the expected rule, the parser throws a syntax error that includes the exact line and column where parsing failed. This makes the implementation practical for testing and further extensions.

---

## PROGRAM FLOW

The complete processing pipeline for lab 6 is now:

```text
source file -> lexer -> token sequence -> parser -> abstract syntax tree
```

The entry point for this laboratory work reads the source file, runs the lexer, prints the generated tokens, then runs the parser and prints the resulting AST. This makes it easy to verify both stages during evaluation.

---

## RESULT

The implementation demonstrates that the DSL can now be processed at two levels. First, the input text is transformed into a sequence of categorized tokens. Second, those tokens are assembled into a structural representation of the program. The result is no longer just a flat token list, but a tree that exposes declarations, assignments, function invocations, and the final returned value.

This is an important step toward later compilation stages such as semantic analysis, interpretation, or code generation, because those stages operate naturally on an AST rather than on raw text.

---

## CONCLUSION

This laboratory work completes the transition from lexical analysis to syntactic analysis for the small DSL used in the project. The lexer from the previous work was reused and improved with regex-based token classification, and a recursive descent parser was added on top of it. The final result is a working Abstract Syntax Tree builder that satisfies the objectives of the task and provides a solid base for future language-processing stages.

---

## REFERENCES

[1] https://en.wikipedia.org/wiki/Parsing  
[2] https://en.wikipedia.org/wiki/Abstract_syntax_tree
