package lox;

import java.util.List;

public abstract class Stmt {

    interface Visitor<R> {
        R visitExpression(Expression stmt);

        R visitPrint(Print stmt);

        R visitVar(Var stmt);

        R visitBlock(Block stmt);
    }

    abstract <R> R accept(Visitor<R> visitor);

    public static class Expression extends Stmt {
        Expression(Expr expression) {
            this.expression = expression;
        }

        final Expr expression;

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitExpression(this);
        }
    }

    public static class Print extends Stmt {
        Print(Expr expression) {
            this.expression = expression;
        }

        final Expr expression;

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitPrint(this);
        }
    }

    public static class Var extends Stmt {
        Var(Token name, Expr initializer) {
            this.name = name;
            this.initializer = initializer;
        }

        final Token name;
        Expr initializer;

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitVar(this);
        }
    }

    public static class Block extends Stmt {
        Block(List<Stmt> statements) {
            this.statements = statements;
        }

        final List<Stmt> statements;

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitBlock(this);
        }
    }
}
