#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

inline std::string ast_indent(int depth) {
    return std::string(static_cast<size_t>(depth) * 2, ' ');
}

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(std::ostream& out, int depth) const = 0;
};

struct Expression : ASTNode {
    ~Expression() override = default;
};

struct IdentifierExpr final : Expression {
    explicit IdentifierExpr(std::string name)
    : name(std::move(name)) {}

    void print(std::ostream& out, int depth) const override {
        out << ast_indent(depth) << "Identifier(" << name << ")\n";
    }

    std::string name;
};

struct LiteralExpr final : Expression {
    LiteralExpr(std::string kind, std::string value)
    : kind(std::move(kind)), value(std::move(value)) {}

    void print(std::ostream& out, int depth) const override {
        out << ast_indent(depth) << kind << "(" << value << ")\n";
    }

    std::string kind;
    std::string value;
};

struct CallExpr final : Expression {
    explicit CallExpr(std::string callee)
    : callee(std::move(callee)) {}

    void print(std::ostream& out, int depth) const override {
        out << ast_indent(depth) << "Call(" << callee << ")\n";
        for (const auto& argument : arguments) {
            argument->print(out, depth + 1);
        }
    }

    std::string callee;
    std::vector<std::unique_ptr<Expression>> arguments;
};

struct Statement : ASTNode {
    ~Statement() override = default;
};

struct TypeDeclarationStmt final : Statement {
    TypeDeclarationStmt(std::string name, std::string type_name)
    : name(std::move(name)), type_name(std::move(type_name)) {}

    void print(std::ostream& out, int depth) const override {
        out << ast_indent(depth) << "TypeDeclaration(" << name << ": " << type_name << ")\n";
        if (value) {
            value->print(out, depth + 1);
        }
    }

    std::string name;
    std::string type_name;
    std::unique_ptr<Expression> value;
};

struct AssignmentStmt final : Statement {
    explicit AssignmentStmt(std::string target)
    : target(std::move(target)) {}

    void print(std::ostream& out, int depth) const override {
        out << ast_indent(depth) << "Assignment(" << target << ")\n";
        value->print(out, depth + 1);
    }

    std::string target;
    std::unique_ptr<Expression> value;
};

struct ReturnStmt final : Statement {
    void print(std::ostream& out, int depth) const override {
        out << ast_indent(depth) << "Return\n";
        value->print(out, depth + 1);
    }

    std::unique_ptr<Expression> value;
};

struct Program final : ASTNode {
    void print(std::ostream& out, int depth) const override {
        out << ast_indent(depth) << "Program\n";
        for (const auto& statement : statements) {
            statement->print(out, depth + 1);
        }
    }

    std::vector<std::unique_ptr<Statement>> statements;
};
