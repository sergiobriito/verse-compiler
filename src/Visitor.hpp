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
struct PrintNode;
struct IncrementNode;
struct FunctionNode;
struct CallNode;
struct ReturnNode;
struct ArrayNode;
struct IndexNode;

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
    virtual void visit(PrintNode* node) = 0;
    virtual void visit(IncrementNode* node) = 0;
    virtual void visit(FunctionNode* node) = 0;
    virtual void visit(CallNode* node) = 0;
    virtual void visit(ReturnNode* node) = 0;
    virtual void visit(ArrayNode* node) = 0;
    virtual void visit(IndexNode* node) = 0;
};

#endif