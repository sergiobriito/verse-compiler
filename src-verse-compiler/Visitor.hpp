#ifndef VISITOR_HPP
#define VISITOR_HPP

struct ProgramNode;
struct BinaryOpNode;
struct NumberNode;
struct IdentifierNode;
struct StringNode;
struct DeclarationNode;
struct AssignmentNode;
struct ComparisonNode;
struct IfStatementNode;
struct ForLoopNode;
struct WhileLoopNode;
struct FunctionCallNode;
struct FunctionDeclarationNode;
struct PrintNode;
struct IncrementNode;

class Visitor {
public:
    virtual void visit(ProgramNode* node) = 0;
    virtual void visit(BinaryOpNode* node) = 0;
    virtual void visit(NumberNode* node) = 0;
    virtual void visit(IdentifierNode* node) = 0;
    virtual void visit(StringNode* node) = 0;
    virtual void visit(DeclarationNode* node) = 0;
    virtual void visit(AssignmentNode* node) = 0;
    virtual void visit(ComparisonNode* node) = 0;
    virtual void visit(IfStatementNode* node) = 0;
    virtual void visit(ForLoopNode* node) = 0;
    virtual void visit(WhileLoopNode* node) = 0;
    virtual void visit(FunctionCallNode* node) = 0;
    virtual void visit(FunctionDeclarationNode* node) = 0;
    virtual void visit(PrintNode* node) = 0;
    virtual void visit(IncrementNode* node) = 0;
};

#endif