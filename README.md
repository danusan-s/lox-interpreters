# Lox interpreter

Interpreters written for the lox scripting language following the book "Crafting Interpreters" by Robert Nystrom.

## JLox:

A lox interpreter written in Java. It is a complete implementation of the lox language, including a REPL when run without arguements.
The Scanner class tokenizes the lox source code, following which the Parser class creates an abstract syntax tree (AST) from the tokens. The Interpreter class then interprets the AST, executing the code.

### Things learnt:
- Tokens and lexing
- Abstract syntax trees (ASTs)
- Recursive descent parsing
- Prefix and infix expressions
- Runtime representation of objects
- Interpreting code using the Visitor pattern
- Lexical scope
- Environment chains for storing variables
- Control flow
- Functions with parameters
- Closures
- Static variable resolution and error detection
- Classes
- Constructors
- Fields
- Methods
- Inheritance

