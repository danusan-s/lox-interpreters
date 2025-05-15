package lox;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;

/**
 * Main class for the Lox interpreter implementation.
 * Handles script execution, REPL interaction, and error reporting.
 */
public class Lox {
    // Tracks whether a syntax error occurred during parsing
    private static boolean hadError = false;
    
    // Tracks whether a runtime error occurred during interpretation
    private static boolean hadRuntimeError = false;

    // The interpreter instance used to execute Lox code
    private static final Interpreter interpreter = new Interpreter();

    /**
     * Entry point for the Lox interpreter.
     * Handles both script file execution and interactive REPL mode.
     *
     * @param args Command line arguments. Accepts optional script file path
     * @throws IOException If there's an error reading the script file or REPL input
     */
    public static void main(String[] args) throws IOException {
        if (args.length > 1) {
            System.out.println("Usage: jlox [script]");
            System.exit(64);
        } else if (args.length == 1) {
            runFile(args[0]);
        } else {
            runPrompt();
        }
    }

    /**
     * Executes a Lox script file.
     * 
     * @param path Path to the script file to execute
     * @throws IOException If the file cannot be read
     */
    private static void runFile(String path) throws IOException {
        byte[] bytes = Files.readAllBytes(Paths.get(path));
        run(new String(bytes, Charset.defaultCharset()));
        if (Lox.hadError) System.exit(65);
        if (Lox.hadRuntimeError) System.exit(70);
    }

    /**
     * Starts an interactive REPL (Read-Eval-Print Loop) session.
     * Allows users to enter and execute Lox code line by line.
     *
     * @throws IOException If there's an error reading from standard input
     */
    private static void runPrompt() throws IOException {
        InputStreamReader input = new InputStreamReader(System.in);
        BufferedReader reader = new BufferedReader(input);

        for (;;) {
            System.out.print("> ");
            String line = reader.readLine();
            if (line == null) break;
            run(line);
            hadError = false;
        }
    }

    /**
     * Core method that processes and executes Lox source code.
     * Handles scanning, parsing, resolving, and interpreting the code.
     *
     * @param source The Lox source code to execute
     */
    private static void run(String source) {
        Scanner scanner = new Scanner(source);
        List<Token> tokens = scanner.scanTokens();

        Parser parser = new Parser(tokens);
        List<Stmt> statements = parser.parse();

        if (hadError) return;

        Resolver resolver = new Resolver(interpreter);
        resolver.resolve(statements);

        if (hadError) return;

        interpreter.interpret(statements);
    }

    /**
     * Reports a syntax error at a specific line.
     *
     * @param line Line number where the error occurred
     * @param message Error message describing the problem
     */
    static void error(int line, String message) {
        report(line, "", message);
    }

    /**
     * Reports a syntax error at a specific token.
     *
     * @param token The token where the error occurred
     * @param message Error message describing the problem
     */
    static void error(Token token, String message) {
        if (token.type == TokenType.EOF) {
            report(token.line, " at end", message);
        } else {
            report(token.line, " at '" + token.lexeme + "'", message);
        }
    }

    /**
     * Reports a runtime error that occurred during interpretation.
     *
     * @param error The runtime error that occurred
     */
    static void runtimeError(RuntimeError error) {
        System.err.println(error.getMessage() +
                "\n[line " + error.token.line + "]");
        hadRuntimeError = true;
    }

    /**
     * Generic error reporting method used by other error handling methods.
     *
     * @param line Line number where the error occurred
     * @param where Additional context about where in the line the error occurred
     * @param message The error message
     */
    static void report(int line, String where, String message) {
        System.err.println(
                "[line " + line + "] Error" + where + ": " + message);
        hadError = true;
    }
}