package lox;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static lox.TokenType.*;

/**
 * Lexical scanner for the Lox programming language.
 * Converts source code into a sequence of tokens for parsing.
 * Handles lexical analysis including keywords, identifiers, literals, and operators.
 */
public class Scanner {
    // The source code to be scanned
    private final String source;
    
    // List of tokens produced by the scanner
    private final List<Token> tokens = new ArrayList<>();

    // Lookup table for reserved keywords
    private static final Map<String, TokenType> keywords;

    static {
        keywords = new HashMap<>();
        keywords.put("and", AND);
        keywords.put("class", CLASS);
        keywords.put("else", ELSE);
        keywords.put("false", FALSE);
        keywords.put("for", FOR);
        keywords.put("fun", FUN);
        keywords.put("if", IF);
        keywords.put("nil", NIL);
        keywords.put("or", OR);
        keywords.put("print", PRINT);
        keywords.put("return", RETURN);
        keywords.put("super", SUPER);
        keywords.put("this", THIS);
        keywords.put("true", TRUE);
        keywords.put("var", VAR);
        keywords.put("while", WHILE);
    }

    // Start position of the current lexeme being scanned
    private int start = 0;
    
    // Current position in the source code
    private int current = 0;
    
    // Current line number in the source code
    private int line = 1;

    /**
     * Checks if the scanner has reached the end of the source code.
     *
     * @return true if at end of source, false otherwise
     */
    private boolean isAtEnd() {
        return current >= source.length();
    }

    /**
     * Creates a new Scanner with the given source code.
     *
     * @param source The source code to scan
     */
    Scanner(String source) {
        this.source = source;
    }

    /**
     * Scans the entire source code and produces a list of tokens.
     * This is the main entry point for lexical analysis.
     *
     * @return List of tokens found in the source code
     */
    public List<Token> scanTokens() {
        while (!isAtEnd()) {
            start = current;
            scanToken();
        }
        tokens.add(new Token(EOF, "", null, line));
        return tokens;
    }

    /**
     * Scans a single token from the source code.
     * Handles all token types including operators, keywords, identifiers, and literals.
     */
    private void scanToken() {
        char c = advance();

        switch (c) {
            case '(':
                addToken(LEFT_PAREN);
                break;
            case ')':
                addToken(RIGHT_PAREN);
                break;
            case '{':
                addToken(LEFT_BRACE);
                break;
            case '}':
                addToken(RIGHT_BRACE);
                break;
            case ',':
                addToken(COMMA);
                break;
            case '.':
                addToken(DOT);
                break;
            case '-':
                addToken(MINUS);
                break;
            case '+':
                addToken(PLUS);
                break;
            case ';':
                addToken(SEMICOLON);
                break;
            case '*':
                addToken(STAR);
                break;

            case '!':
                addToken(match('=') ? BANG_EQUAL : BANG);
                break;
            case '=':
                addToken(match('=') ? EQUAL_EQUAL : EQUAL);
                break;
            case '<':
                addToken(match('=') ? LESS_EQUAL : LESS);
                break;
            case '>':
                addToken(match('=') ? GREATER_EQUAL : GREATER);
                break;
            case '/':
                if (match('/')) {
                    // A comment goes until the end of the line.
                    // No need to add comments to the token list.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (match('*')) {
                    while (peek() != '*' && peekNext() != '/' && !isAtEnd()) {
                        if (peek() == '\n') line++;
                        advance();
    }
                } else {
                    addToken(SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                line++;
                break;

            case '"':
                string();
                break;

            default:
                if (isDigit(c)) {
                    number();
                } else if (isAlpha(c)) {
                    identifier();
                } else {
                    Lox.error(line, "Unexpected character.");
                }
                break;
        }
    }

    /**
     * Consumes the next character in the source and returns it.
     *
     * @return The next character in the source
     */

    private char advance() {
        return source.charAt(current++);
    }

    /**
     * Checks if the next character matches the expected character.
     * Used for two-character tokens like '!=', '==', etc.
     *
     * @param expected The character to match against
     * @return true if the next character matches, false otherwise
     */
    private boolean match(char expected) {
        if (isAtEnd()) return false;
        if (source.charAt(current) != expected) return false;

        current++;
        return true;
    }

    /**
     * Returns the next character without consuming it.
     *
     * @return The next character in the source, or '\0' if at end
     */
    private char peek() {
        if (isAtEnd()) return '\0';
        return source.charAt(current);
    }

    /**
     * Returns the character after the next character without consuming either.
     *
     * @return The character after next, or '\0' if at end
     */
    private char peekNext() {
        if (current + 1 >= source.length()) return '\0';
        return source.charAt(current + 1);
    }

    /**
     * Processes a string literal.
     * Handles multi-line strings and reports unterminated strings.
     */
    private void string() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line++;
            advance();
        }

        if (isAtEnd()) {
            Lox.error(line, "Unterminated string.");
            return;
    }

        // The closing ".
        advance();

        // Trim the surrounding quotes.
        String value = source.substring(start + 1, current - 1);
        addToken(STRING, value);
    }

    /**
     * Checks if a character is a digit (0-9).
     *
     * @param c Character to check
     * @return true if the character is a digit
     */

    private boolean isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    /**
     * Processes a number literal.
     * Handles both integer and decimal numbers.
     */
    private void number() {
        while (isDigit(peek())) advance();
        if (peek() == '.' && isDigit(peekNext())) {
            advance();
            while (isDigit(peek())) advance();
        }
        addToken(NUMBER, Double.parseDouble(source.substring(start, current)));
    }

    /**
     * Checks if a character is a letter or underscore.
     *
     * @param c Character to check
     * @return true if the character is a letter or underscore
     */
    private boolean isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    /**
     * Checks if a character is alphanumeric or underscore.
     *
     * @param c Character to check
     * @return true if the character is alphanumeric or underscore
     */
    private boolean isAlphaNumeric(char c) {
        return isAlpha(c) || isDigit(c);
    }

    /**
     * Processes an identifier or keyword.
     * Determines if the lexeme is a reserved keyword or user-defined identifier.
     */
    private void identifier() {
        while (isAlphaNumeric(peek())) advance();

        String text = source.substring(start, current);
        TokenType type = keywords.get(text);
        if (type == null) type = IDENTIFIER;
        addToken(type);
    }

    /**
     * Adds a token with no literal value.
     *
     * @param type The type of token to add
     */
    private void addToken(TokenType type) {
        addToken(type, null);
    }

    /**
     * Adds a token with a literal value.
     *
     * @param type The type of token to add
     * @param literal The literal value associated with the token
     */
    private void addToken(TokenType type, Object literal) {
        String text = source.substring(start, current);
        tokens.add(new Token(type, text, literal, line));
    }
}