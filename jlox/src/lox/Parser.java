package lox;

import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;

import static lox.TokenType.*;

/**
 * Recursive descent parser for the Lox programming language.
 * Converts a flat sequence of tokens into a tree structure of expressions and statements.
 * Implements precedence rules and handles syntax error recovery.
 */
public class Parser {
    
    // Exception class to indicate error during parse
    private static class ParseError extends RuntimeException {}

    // The list of tokens to be parsed
    private final List<Token> tokens;

    // Current position in the token list
    private int current = 0;

    /**
     * Constructs a new Parser with the given list of tokens.
     *
     * @param tokens The list of tokens to parse
     */
    Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    /**
     * Checks if the current token matches any of the given types.
     * Advances to the next token if there's a match.
     *
     * @param types Variable number of TokenType to match against
     * @return true if current token matches any of the types
     */
    private boolean match(TokenType... types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if the current token is of the given type without consuming it.
     *
     * @param type The TokenType to check for
     * @return true if current token is of the given type
     */
    private boolean check(TokenType type) {
        if (isAtEnd()) {
            return false;
        }
        return peek().type == type;
    }

    /**
     * Returns the current token without consuming it.
     *
     * @return The current Token
     */
    private Token peek() {
        return tokens.get(current);
    }

    /**
     * Returns the most recently consumed token.
     *
     * @return The previous Token
     */
    private Token previous() {
        return tokens.get(current - 1);
    }

    /**
     * Checks if we've reached the end of the token list.
     *
     * @return true if at end of input
     */
    private boolean isAtEnd() {
        return peek().type == EOF;
    }

    /**
     * Consumes and returns the current token.
     *
     * @return The current Token before advancing
     */
    private Token advance() {
        if (!isAtEnd()) {
            current++;
        }
        return previous();
    }

    /**
     * Parses the tokens into a list of statements.
     * Entry point for the parsing process.
     *
     * @return List of parsed statements
     */
    public List<Stmt> parse() {
        List<Stmt> statements = new ArrayList<>();
        while (!isAtEnd()) {
            statements.add(declaration());
        }
        return statements;
    }

    /**
     * Parses a declaration (variable, function, or class declaration).
     *
     * @return The parsed declaration statement
     */
    private Stmt declaration() {
        try {
            if (match(FUN)) {
                return function("function");
            }
            if (match(VAR)) {
                return varDeclaration();
            }
            return statement();
        } catch (ParseError error) {
            synchronize();
            return null;
        }
    }

    /**
     * Parses a function declaration.
     *
     * @param kind The kind of function being declared (e.g., "function", "method")
     * @return The parsed function declaration statement
     */
    private Stmt function(String kind) {
        Token name = consume(IDENTIFIER, "Expect " + kind + " name.");
        consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
        List<Token> parameters = new ArrayList<>();
        if (!check(RIGHT_PAREN)) {
            do {
                if (parameters.size() >= 255) {
                    throw error(peek(), "Can't have more than 255 parameters.");
                }
                parameters.add(consume(IDENTIFIER, "Expect parameter name."));
            } while (match(COMMA));
        }
        consume(RIGHT_PAREN, "Expect ')' after parameters.");
        consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
        List<Stmt> body = block();
        return new Stmt.Function(name, parameters, body);
    }

    /**
     * Parses a variable declaration.
     *
     * @return The parsed variable declaration statement
     */
    private Stmt varDeclaration() {
        Token name = consume(IDENTIFIER, "Expect variable name.");
        Expr initializer = null;
        if (match(EQUAL)) {
            initializer = expression();
        }
        consume(SEMICOLON, "Expect ';' after variable declaration.");
        return new Stmt.Var(name, initializer);
    }

    /**
     * Parses a statement.
     *
     * @return The parsed statement
     */
    private Stmt statement() {
        if (match(IF)) {
            return ifStatement();
        }
        if (match(CLASS)) {
            return classDeclaration();
        }
        if (match(FOR)) {
            return forStatement();
        }
        if (match(PRINT)) {
            return printStatement();
        }
        if (match(RETURN)) {
            return returnStatement();
        }
        if (match(WHILE)) {
            return whileStatement();
        }
        if (match(LEFT_BRACE)) {
            return new Stmt.Block(block());
        }
        return expressionStatement();
    }

    /**
     * Parses a block of statements.
     *
     * @return List of statements in the block
     */
    private List<Stmt> block() {
        List<Stmt> statements = new ArrayList<>();
        while (!check(RIGHT_BRACE) && !isAtEnd()) {
            statements.add(declaration());
        }
        consume(RIGHT_BRACE, "Expect '}' after block.");
        return statements;
    }

    /**
     * Parses a class declaration.
     *
     * @return The parsed class declaration statement
     */
    private Stmt classDeclaration() {
        Token name = consume(IDENTIFIER, "Expect class name.");

        Expr.Variable superclass = null;
        if (match(LESS)) {
            consume(IDENTIFIER, "Expect superclass name.");
            superclass = new Expr.Variable(previous());
        }

        consume(LEFT_BRACE, "Expect '{' before class body.");
        List<Stmt.Function> methods = new ArrayList<>();
        while (!check(RIGHT_BRACE) && !isAtEnd()) {
            methods.add((Stmt.Function) function("method"));
        }
        consume(RIGHT_BRACE, "Expect '}' after class body.");
        return new Stmt.Class(name, superclass, methods);
    }

    /**
     * Parses an if statement.
     *
     * @return The parsed if statement
     */
    private Stmt ifStatement() {
        consume(LEFT_PAREN, "Expect '(' after 'if'.");
        Expr condition = expression();
        consume(RIGHT_PAREN, "Expect ')' after condition.");

        Stmt thenBranch = statement();
        Stmt elseBranch = null;
        if (match(ELSE)) {
            elseBranch = statement();
        }

        return new Stmt.If(condition, thenBranch, elseBranch);
    }

    /**
     * Parses a while statement.
     *
     * @return The parsed while statement
     */
    private Stmt whileStatement() {
        consume(LEFT_PAREN, "Expect '(' after 'while'.");
        Expr condition = expression();
        consume(RIGHT_PAREN, "Expect ')' after condition.");
        Stmt body = statement();

        return new Stmt.While(condition, body);
    }

    /**
     * Parses a for statement.
     *
     * @return The parsed for statement
     */
    private Stmt forStatement() {
        consume(LEFT_PAREN, "Expect '(' after 'for'.");

        Stmt initializer;
        if (match(SEMICOLON)) {
            initializer = null;
        } else if (match(VAR)) {
            initializer = varDeclaration();
        } else {
            initializer = expressionStatement();
        }

        Expr condition = null;
        if (!check(SEMICOLON)) {
            condition = expression();
        }
        consume(SEMICOLON, "Expect ';' after loop condition.");

        Expr increment = null;
        if (!check(RIGHT_PAREN)) {
            increment = expression();
        }
        consume(RIGHT_PAREN, "Expect ')' after for clauses.");

        Stmt body = statement();

        if (increment != null) {
            body = new Stmt.Block(Arrays.asList(body, new Stmt.Expression(increment)));
        }

        if (condition == null) {
            condition = new Expr.Literal(true);
        }

        body = new Stmt.While(condition, body);

        if (initializer != null) {
            body = new Stmt.Block(Arrays.asList(initializer, body));
        }

        return body;
    }

    /**
     * Parses a print statement.
     *
     * @return The parsed print statement
     */
    private Stmt printStatement() {
        Expr value = expression();
        consume(SEMICOLON, "Expect ';' after value.");
        return new Stmt.Print(value);
    }

    /**
     * Parses a return statement.
     *
     * @return The parsed return statement
     */
    private Stmt returnStatement() {
        Token keyword = previous();
        Expr value = null;
        if (!check(SEMICOLON)) {
            value = expression();
        }
        consume(SEMICOLON, "Expect ';' after return value.");
        return new Stmt.Return(keyword, value);
    }

    /**
     * Parses an expression statement.
     *
     * @return The parsed expression statement
     */
    private Stmt expressionStatement() {
        Expr expression = expression();
        consume(SEMICOLON, "Expect ';' after value.");
        return new Stmt.Expression(expression);
    }

    /**
     * Parses an expression.
     *
     * @return The parsed expression
     */
    private Expr expression() {
        return assignment();
    }

    /**
     * Parses an assignment expression.
     *
     * @return The parsed assignment expression
     */
    private Expr assignment() {
        Expr expr = or();

        if (match(EQUAL)) {
            Token equals = previous();
            Expr value = assignment();

            if (expr instanceof Expr.Variable) {
                Token name = ((Expr.Variable) expr).name;
                return new Expr.Assign(name, value);
            } else if (expr instanceof Expr.Get) {
                Expr.Get get = (Expr.Get) expr;
                return new Expr.Set(get.object, get.name, value);
            }

            throw error(equals, "Invalid assignment target.");
        }

        return expr;
    }

    /**
     * Parses a logical OR expression.
     *
     * @return The parsed logical OR expression
     */
    private Expr or() {
        Expr expr = and();

        if (match(OR)) {
            Token or = previous();
            Expr right = and();
            expr = new Expr.Logical(expr, or, right);
        }

        return expr;
    }

    /**
     * Parses a logical AND expression.
     *
     * @return The parsed logical AND expression
     */
    private Expr and() {
        Expr expr = equality();

        if (match(AND)) {
            Token and = previous();
            Expr right = equality();
            expr = new Expr.Logical(expr, and, right);
        }

        return expr;
    }

    /**
     * Parses an equality expression.
     *
     * @return The parsed equality expression
     */
    private Expr equality() {
        Expr expr = comparison();

        while (match(BANG_EQUAL, EQUAL_EQUAL)) {
            Token operator = previous();
            Expr right = comparison();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    /**
     * Parses a comparison expression.
     *
     * @return The parsed comparison expression
     */
    private Expr comparison() {
        Expr expr = term();

        while (match(LESS, GREATER, LESS_EQUAL, GREATER_EQUAL)) {
            Token operator = previous();
            Expr right = term();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    /**
     * Parses a term (addition/subtraction) expression.
     *
     * @return The parsed term expression
     */
    private Expr term() {
        Expr expr = factor();

        while (match(PLUS, MINUS)) {
            Token operator = previous();
            Expr right = factor();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    /**
     * Parses a factor (multiplication/division) expression.
     *
     * @return The parsed factor expression
     */
    private Expr factor() {
        Expr expr = unary();

        while (match(SLASH, STAR)) {
            Token operator = previous();
            Expr right = unary();
            expr = new Expr.Binary(expr, operator, right);
        }

        return expr;
    }

    /**
     * Parses a unary expression.
     *
     * @return The parsed unary expression
     */
    private Expr unary() {
        if (match(BANG, MINUS)) {
            Token operator = previous();
            Expr right = unary();
            return new Expr.Unary(operator, right);
        }

        return call();
    }

    /**
     * Parses a function call expression.
     *
     * @return The parsed call expression
     */
    private Expr call() {
        Expr expr = primary();

        while (true) {
            if (match(LEFT_PAREN)) {
                expr = finishCall(expr);
            } else if (match(DOT)) {
                Token name = consume(IDENTIFIER, "Expect property name after '.'.");
                expr = new Expr.Get(expr, name);
            } else {
                break;
            }
        }

        return expr;
    }

    /**
     * Completes parsing of a function call after the opening parenthesis.
     *
     * @param callee The expression being called
     * @return The completed call expression
     */
    private Expr finishCall(Expr callee) {
        List<Expr> arguments = new ArrayList<>();
        if (!check(RIGHT_PAREN)) {
            do {
                if (arguments.size() >= 255) {
                    throw error(peek(), "Can't have more than 255 arguments.");
                }
                arguments.add(expression());
            } while (match(COMMA));
        }

        Token paren = consume(RIGHT_PAREN, "Expect ')' after arguments.");
        return new Expr.Call(callee, paren, arguments);
    }

    /**
     * Parses a primary expression (literal, grouping, etc.).
     *
     * @return The parsed primary expression
     */
    private Expr primary() {
        if (match(FALSE)) {
            return new Expr.Literal(false);
        }
        if (match(TRUE)) {
            return new Expr.Literal(true);
        }
        if (match(NIL)) {
            return new Expr.Literal(null);
        }
        if (match(NUMBER, STRING)) {
            return new Expr.Literal(previous().literal);
        }
        if (match(LEFT_PAREN)) {
            Expr expr = expression();
            consume(RIGHT_PAREN, "Expect ')' after expression.");
            return new Expr.Grouping(expr);
        }
        if (match(IDENTIFIER)) {
            return new Expr.Variable(previous());
        }
        if (match(THIS)) {
            return new Expr.This(previous());
        }
        if (match(SUPER)) {
            Token keyword = previous();
            consume(DOT, "Expect '.' after 'super'.");
            Token method = consume(IDENTIFIER, "Expect superclass method name.");
            return new Expr.Super(keyword, method);
        }

        throw error(peek(), "Expect expression.");
    }

    /**
     * Consumes the current token if it matches the expected type.
     * Throws ParseError if it doesn't match.
     *
     * @param type Expected token type
     * @param message Error message if token doesn't match
     * @return The consumed token
     * @throws ParseError if the current token doesn't match the expected type
     */
    private Token consume(TokenType type, String message) {
        if (check(type)) return advance();

        throw error(peek(), message);
    }

    /**
     * Creates a ParseError with the given token and message.
     *
     * @param token The token where the error occurred
     * @param message The error message
     * @return A new ParseError instance
     */
    private ParseError error(Token token, String message) {
        Lox.error(token, message);
        return new ParseError();
    }

    /**
     * Synchronizes the parser state after an error.
     * Discards tokens until it finds a likely statement boundary.
     */
    private void synchronize() {
        advance();

        while (!isAtEnd()) {
            // Explicit boundary
            if (previous().type == SEMICOLON) return;

            // If no semicolon, infer from token as these are common start of statements.
            switch (peek().type) {
                case CLASS:
                case FUN:
                case VAR:
                case FOR:
                case IF:
                case WHILE:
                case PRINT:
                case RETURN:
                    return;
            }

            advance();
        }
    }
}