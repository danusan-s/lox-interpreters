package lox;

import java.util.List;

public abstract class Stmt {

    interface Visitor<R> {
        R visitExpression(Expression stmt);

        R visitPrint(Print stmt);

        R visitVar(Var stmt);

        R visitBlock(Block stmt);

        R visitIf(If stmt);

        R visitWhile(While stmt);

        R visitFunction(Function stmt);
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

    public static class If extends Stmt {
        If(Expr condition, Stmt thenBranch, Stmt elseBranch) {
            this.condition = condition;
            this.thenBranch = thenBranch;
            this.elseBranch = elseBranch;
        }

        final Expr condition;
        final Stmt thenBranch;
        final Stmt elseBranch;

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitIf(this);
        }
    }

    public static class While extends Stmt {
        While(Expr condition, Stmt body) {
            this.condition = condition;
            this.body = body;
        }

        final Expr condition;
        final Stmt body;

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitWhile(this);
        }
    }

    public static class Function extends Stmt {
        Function(Token name, List<Token> params, List<Stmt> body) {
            this.name = name;
            this.params = params;
            this.body = body;
        }

        final Token name;
        final List<Token> params;
        final List<Stmt> body;

        @Override
        <R> R accept(Visitor<R> visitor) {
            return visitor.visitFunction(this);
        }
    }
}
