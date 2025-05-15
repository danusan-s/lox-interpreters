# Lox interpreter

Interpreters written for the lox scripting language following the book "Crafting Interpreters" by Robert Nystrom.

## JLox:

A lox interpreter written in Java. It is a complete implementation of the lox language, including a REPL when run without arguements.
The Scanner class tokenizes the lox source code, following which the Parser class creates an abstract syntax tree (AST) from the tokens. The Interpreter class then interprets the AST, executing the code.

### Things learnt:

    tokens and lexing,
    abstract syntax trees,
    recursive descent parsing,
    prefix and infix expressions,
    runtime representation of objects,
    interpreting code using the Visitor pattern,
    lexical scope,
    environment chains for storing variables,
    control flow,
    functions with parameters,
    closures,
    static variable resolution and error detection,
    classes,
    constructors,
    fields,
    methods, and finally,
    inheritance.



