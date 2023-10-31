#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <utility>
#include <vector>
#include "Tokenizer.hpp"
#include "Visitor.hpp"

struct AstNode {
    virtual void accept(Visitor* visitor) = 0;
};

struct ProgramNode : AstNode {
    std::vector<AstNode *> statements;

    explicit ProgramNode(const std::vector<AstNode *> &statements)
            : statements(statements) {}

    ~ProgramNode() {
        for (AstNode *statement : statements) {
            delete statement;
        }
    }

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct BinaryOpNode : AstNode {
    std::string op;
    AstNode *left;
    AstNode *right;

    explicit BinaryOpNode(std::string op, AstNode *left, AstNode *right)
            : op(std::move(op)), left(left), right(right){};

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct NumberNode : AstNode {
    int value{};

    explicit NumberNode(int value) : value(value) {}

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct IncrementNode : AstNode{
    AstNode* identifier{};
    std::string value{};

    explicit IncrementNode(AstNode* identifier, std::string value) : value(value),identifier(identifier) {}

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct IdentifierNode : AstNode {
    std::string name;

    explicit IdentifierNode(std::string name) : name(std::move(name)) {}

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct StringNode : AstNode {
    std::string name;
    explicit StringNode(std::string name) : name(std::move(name)) {}

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct DeclarationNode : AstNode {
    IdentifierNode *identifier;
    AstNode *value{};

    explicit DeclarationNode(IdentifierNode *identifier, AstNode *value)
            : identifier(identifier), value(value) {};

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct AssignmentNode : AstNode {
    IdentifierNode *identifier{};
    AstNode *value{};

    explicit AssignmentNode(IdentifierNode *identifier, AstNode *value)
            : identifier(identifier), value(value) {};

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct ComparisonNode : AstNode {
    std::string op;
    AstNode *left;
    AstNode *right;

    ComparisonNode(std::string op, AstNode *left, AstNode *right)
            : op(op), left(left), right(right) {};

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct IfStatementNode : AstNode {
    AstNode *condition{};
    AstNode *trueBody{};
    AstNode *falseBody{};

    explicit IfStatementNode(AstNode *condition, AstNode *trueBody,
                             AstNode *falseBody)
            : condition(condition), trueBody(trueBody), falseBody(falseBody) {};

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct ForLoopNode : AstNode {
    AstNode* initialization;
    AstNode* condition;
    AstNode* increment;
    AstNode* body;

    ForLoopNode(AstNode* initialization, AstNode* condition, AstNode* increment, AstNode* body)
            : initialization(initialization), condition(condition), increment(increment), body(body) {};

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct WhileLoopNode : AstNode {
    AstNode *condition{};
    AstNode *body{};

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }

};

struct FunctionCallNode : AstNode {
    IdentifierNode *functionName{};
    std::vector<AstNode *> arguments{};

    FunctionCallNode(IdentifierNode *functionName,
                     std::vector<AstNode *> arguments)
            : functionName(functionName), arguments(std::move(arguments)) {};

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct FunctionDeclarationNode : AstNode {
    IdentifierNode *functionName{};
    std::vector<IdentifierNode *> parameters{};
    AstNode *body;

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

struct PrintNode : AstNode {
    AstNode* identifier;

    explicit  PrintNode(AstNode *identifier) : identifier(identifier){}

    void accept(Visitor* visitor) override {
        visitor->visit(this);
    }
};

class Parser {
private:
    std::vector<Token> tokens;
    int idx = 0;

public:
    explicit Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

    std::optional<Token> peek(int offset = 0) {
        if (idx + offset >= tokens.size()) {
            return std::nullopt;
        }
        return tokens.at(idx + offset);
    };

    Token consume() { return tokens.at(idx++); };

    bool isComparisonOp(Token token) {
        return token.type == TokenType::EQEQ || token.type == TokenType::GT ||
               token.type == TokenType::GTEQ || token.type == TokenType::LT ||
               token.type == TokenType::LTEQ || token.type == TokenType::NOTEQ;
    };

    std::vector<std::string> separateIdAndIncrementSymbols(std::string input) {
        std::vector<std::string> separatedTokens;
        std::string currentToken = "";

        for (size_t i = 0; i < input.length(); i++) {

            if (input[i] == '+' && i < input.length() - 1 && input[i + 1] == '+') {
                if (!currentToken.empty()) {
                    separatedTokens.push_back(currentToken);
                    currentToken = "";
                }
                separatedTokens.push_back("++");
                i++;
            } else if (input[i] == '-' && i < input.length() - 1 && input[i + 1] == '-') {
                if (!currentToken.empty()) {
                    separatedTokens.push_back(currentToken);
                    currentToken = "";
                }
                separatedTokens.push_back("--");
                i++;
            } else {
                currentToken += input[i];
            }
        }

        if (!currentToken.empty()) {
            separatedTokens.push_back(currentToken);
        }

        return separatedTokens;
    }

    AstNode *parseProgram() {
        std::vector<AstNode *> statements;
        while (peek().has_value()) {
            AstNode *statement = parseStatement();
            if (statement) {
                statements.push_back(statement);
            } else {
                std::cerr << "Invalid statement in parse program" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        idx = 0;
        return new ProgramNode(statements);
    };

    AstNode *parseStatement() {
        if (peek().value().type == TokenType::LET) {
            consume();
            return parseDeclaration();
        } else if (peek().value().type == TokenType::IDENT) {
            Token identifier = consume();
            if (peek().has_value() && peek().value().type == TokenType::OPENPAR) {
                consume();
                return parseFunctionCall(identifier);
            } else {
                return parseAssignment(identifier);
            }
        } else if (peek().value().type == TokenType::IF) {
            consume();
            return parseIfStatement();
        } else if (peek().value().type == TokenType::FOR) {
            consume();
            return parseForLoopStatement();
        } else if (peek().value().type == TokenType::PRINT) {
            consume();
            return parsePrintStatement();
        }else {
            std::cerr << static_cast<int>(peek().value().type) << std::endl;
            std::cerr << "Invalid statement in parse statement" << std::endl;
            exit(EXIT_FAILURE);
        }
    };

    AstNode *parseDeclaration() {
        if (peek().value().type != TokenType::IDENT) {
            std::cerr << "Expected an identifier after 'let'" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string identifierName = consume().value;
        AstNode *expression = nullptr;

        if (peek().value().type == TokenType::EQ) {
            consume();
            expression = parseExpression();
        }

        if (!peek().has_value() || peek().value().type != TokenType::SEMI_COL) {
            std::cerr << "Expected ';' after the declaration" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        if (expression) {
            return new DeclarationNode(new IdentifierNode(identifierName), expression);
        } else {
            return new DeclarationNode(new IdentifierNode(identifierName) , new NumberNode(0));
        }
    };

    AstNode *parseAssignment(Token identifier) {
        if (!peek().has_value() || peek().value().type != TokenType::EQ) {
            std::cerr << "Expected '='" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        AstNode *expression = parseExpression();

        if (!peek().has_value() || peek().value().type != TokenType::SEMI_COL) {
                std::cerr << "Expected ';' after the assignment" << std::endl;
                exit(EXIT_FAILURE);
        }
        consume();

        return new AssignmentNode(new IdentifierNode(identifier.value), expression);
    };

    AstNode *parseExpression() {
        AstNode *left = parseTerm();
        while (peek().has_value() && peek().value().type == TokenType::PLUS ||
               peek().has_value() && peek().value().type == TokenType::MINUS) {
            Token operatorToken = consume();
            AstNode *right = parseTerm();
            left = new BinaryOpNode(operatorToken.value, left, right);
        }
        return left;
    };

    AstNode *parseTerm() {
        AstNode *left = parseFactor();
        while (peek().has_value() && peek().value().type == TokenType::STAR ||
               peek().has_value() && peek().value().type == TokenType::DIVIDE) {
            Token operatorToken = consume();
            AstNode *right = parseFactor();
            left = new BinaryOpNode(operatorToken.value, left, right);
        }
        return left;
    };

    AstNode *parseFactor() {
        if (peek().value().type == TokenType::NUMBER) {
            int value = std::stoi(consume().value);
            return new NumberNode(value);
        } else if (peek().value().type == TokenType::STRING) {
            std::string name = consume().value;
            return new StringNode(name);
        } else if (peek().value().type == TokenType::PRINT) {
            consume();
            if (peek().value().type == TokenType::OPENPAR) {
                consume();
                AstNode* id = new IdentifierNode(consume().value);
                consume();
                return new PrintNode(id);
            } else {
                std::cerr << "Expected '('" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (peek().value().type == TokenType::IDENT) {
            Token name = consume();
            if (peek().value().type == TokenType::OPENPAR) {
                consume();
                return parseFunctionCall(name);
            } else {
                return new IdentifierNode(name.value);
            }
        } else if (peek().value().type == TokenType::OPENPAR) {
            consume();
            AstNode *expression = parseExpression();
            if (peek().value().type == TokenType::CLOSPAR) {
                consume();
                return expression;
            } else {
                std::cerr << "Expected ')'" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else {
            std::cerr << "Unexpected token in factor: type -> "
                      << static_cast<int>(peek().value().type) << std::endl;
            exit(EXIT_FAILURE);
        }
    };

    AstNode *parseComparison() {
        AstNode *left = parseExpression();
        while (peek().has_value() && isComparisonOp(peek().value())) {
            Token operatorToken = consume();
            AstNode *right = parseExpression();
            left = new ComparisonNode(operatorToken.value, left, right);
        }
        return left;
    };

    AstNode *parseIfStatement() {
        if (!peek().has_value() || peek().value().type != TokenType::OPENPAR) {
            std::cerr << "Expected '('" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        AstNode* left;
        if (peek().value().type == TokenType::IDENT) {
            left = new IdentifierNode(consume().value);
        } else if (peek().value().type == TokenType::NUMBER) {
            left = new NumberNode(std::stoi(consume().value));
        } else {
            std::cerr << "Invalid condition in 'if'" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (!peek().has_value() || !isComparisonOp(peek().value())) {
            std::cerr << "Expected a comparison operator in 'if'" << std::endl;
            exit(EXIT_FAILURE);
        }

        Token compOp = consume();

        AstNode* right;
        if (peek().value().type == TokenType::IDENT) {
            right = new IdentifierNode(consume().value);
        } else if (peek().value().type == TokenType::NUMBER) {
            right = new NumberNode(std::stoi(consume().value));
        } else {
            std::cerr << "Invalid condition in 'if'" << std::endl;
            exit(EXIT_FAILURE);
        }

        AstNode *condition = new ComparisonNode(compOp.value, left, right);
        AstNode *trueBody = nullptr;
        AstNode *falseBody = nullptr;

        if (!peek().has_value() || peek().value().type != TokenType::CLOSPAR) {
            std::cerr << "Expected ')'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        if (!peek().has_value() || peek().value().type != TokenType::OPENCURL) {
            std::cerr << "Expected '{' " << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        if (peek().has_value() && peek().value().type != TokenType::CLOSCURL) {
            trueBody = parseIfElseProgram();
        }else{
            std::cerr << "Expected expression" << std::endl;
            exit(EXIT_FAILURE);
        };

        if (peek().has_value() && peek().value().type == TokenType::ELSE) {
            consume();
            if (!peek().has_value() || peek().value().type != TokenType::OPENCURL) {
                std::cerr << "Expected '{'" << std::endl;
                exit(EXIT_FAILURE);
            }
            consume();

            if (peek().has_value() && peek().value().type != TokenType::CLOSCURL) {
                falseBody = parseIfElseProgram();
            }else{
                std::cerr << "Expected expression" << std::endl;
                exit(EXIT_FAILURE);
            };

        }


        if (!peek().has_value() || peek().value().type != TokenType::SEMI_COL) {
            std::cerr << "Expected ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        return new IfStatementNode(condition, trueBody, falseBody);
    };

    AstNode *parseIfElseProgram() {
        std::vector<AstNode *> statements;
        while (peek().has_value() && peek().value().type != TokenType::CLOSCURL) {
            AstNode *statement = parseStatement();
            if (statement) {
                statements.push_back(statement);
            } else {
                std::cerr << "Invalid statement in parse if program" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        consume();
        return new ProgramNode(statements);
    };

    AstNode *parsePrintStatement(){
        if (!peek().has_value() || peek().value().type != TokenType::OPENPAR) {
            std::cerr << "Expected '('" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        if (!peek().has_value() || peek().value().type != TokenType::IDENT) {
            std::cerr << "Expected an identifier" << std::endl;
            exit(EXIT_FAILURE);
        }

        IdentifierNode *identifierNode = new IdentifierNode(consume().value);

        if (!peek().has_value() || peek().value().type != TokenType::CLOSPAR) {
            std::cerr << "Expected ')'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        if (!peek().has_value() || peek().value().type != TokenType::SEMI_COL) {
            std::cerr << "Expected ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        return new PrintNode(identifierNode);
    };

    AstNode *parseLoopProgram() {
        std::vector<AstNode *> statements;
        while (peek().has_value() && peek().value().type != TokenType::CLOSCURL) {
            AstNode *statement = parseStatement();
            if (statement) {
                statements.push_back(statement);
            } else {
                std::cerr << "Invalid statement in parse if program" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        consume();
        return new ProgramNode(statements);
    };

    AstNode *parseIncrement(){
        std::vector<std::string> increment = separateIdAndIncrementSymbols(consume().value);
        if (!std::isalnum(increment.at(0)[0])){
            std::cerr << "Expected an identifier" << std::endl;
            exit(EXIT_FAILURE);
        };
        return new IncrementNode(new IdentifierNode(increment.at(0)),increment.at(1));
    };

    AstNode *parseForLoopStatement(){
        if (!peek().has_value() || peek().value().type != TokenType::OPENPAR) {
            std::cerr << "Expected '('" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        AstNode* initialization = parseAssignment(consume());
        AstNode* condition = parseComparison();

        if (!peek().has_value() || peek().value().type != TokenType::SEMI_COL) {
            std::cerr << "Expected ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        AstNode* incrementNode = parseIncrement();

        if (!peek().has_value() || peek().value().type != TokenType::CLOSPAR) {
            std::cerr << "Expected ')'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        if (!peek().has_value() || peek().value().type != TokenType::OPENCURL) {
            std::cerr << "Expected '{'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        AstNode* body = nullptr;
        if (peek().has_value() && peek().value().type != TokenType::CLOSCURL) {
            body = parseLoopProgram();
        };

        if (!peek().has_value() || peek().value().type != TokenType::SEMI_COL) {
            std::cerr << "Expected ';'" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        return new ForLoopNode(initialization,condition, incrementNode,body);
    };

    AstNode *parseFunctionCall(Token id) {
        std::vector<AstNode *> arguments;
        while (peek().value().type != TokenType::CLOSPAR) {
            AstNode *argument = parseExpression();
            if (!argument) {
                std::cerr << "Invalid function call argument" << std::endl;
                exit(EXIT_FAILURE);
            }
            arguments.push_back(argument);
            if (peek().value().type == TokenType::COMMA) {
                consume();
            } else if (peek().value().type != TokenType::CLOSPAR) {
                std::cerr << "Expected ')' in function call" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        consume();

        if (!peek().has_value() || peek().value().type != TokenType::SEMI_COL) {
            std::cerr << "Expected ';' in function call" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        return new FunctionCallNode(new IdentifierNode(id.value), arguments);
    }

};

#endif