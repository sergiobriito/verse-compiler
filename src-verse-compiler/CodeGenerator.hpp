#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <map>
#include "Visitor.hpp"
#include "Parser.hpp"

class CodeGenerator : public Visitor {
private:
    std::map<std::string, std::string> variables;
    std::stack<int> stack {};
    std::string buffer {};
    std::string labelBuffer {};
    std::string fileName {};
    std::ofstream file {};
    int labelCount {};

public:
    std::string replaceSubstring(std::string originalString, std::string searchString, std::string replacementString) {
        std::string newString = originalString;
        size_t found = newString.find(searchString);
        if (found != std::string::npos) {
            newString.replace(found, searchString.length(), replacementString);
        }
        return newString;
    }

    bool isAllDigits(std::string str) {
        for (char c : str) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    }

    explicit CodeGenerator (const std::string& outputFileName = "out.asm"){
        this->file = std::ofstream (outputFileName, std::ios::trunc);
        this->fileName = outputFileName;
    };

    void addVariable(const std::string& name, std::string value) {
        variables[name] = std::move(value);
    }

    void generateCode(AstNode* node) {
        file << "section .text" << std::endl;
        file << "global _start" << std::endl;
        file << "extern printf" << std::endl;
        file << "extern exit" << std::endl;
        file << "_start:" << std::endl;
        file << std::endl;

        node->accept(this);

        file << std::endl << "call exit" << std::endl;

        genDataSection();
        file.close();
    }

    void genDataSection() {
        file << std::endl << "section .data" << std::endl;
        if (!variables.empty()) {
            for (const std::pair<const std::basic_string<char>, std::basic_string<char>>& variable : variables) {
                if (variable.first.find("_len") != std::string::npos){
                    file << variable.first << variable.second  << std::endl;
                }else{
                    if (isAllDigits(variable.second)){
                        file << variable.first << " dd " << variable.second  << std::endl;
                    }else{
                        file << variable.first << " db " << "\"" << variable.second << "\"" << ",10,0" << std::endl;
                    };

                }

            }
        }
        file << "fmt db \"%d\", 10, 0" << std::endl;
    }

    void visit(ProgramNode* node) override {
        for (AstNode* statement : node->statements) {
            statement->accept(this);
        }
    }

    void visit(BinaryOpNode* node) override {
        int l;
        int f;

        node->left->accept(this);
        if (!buffer.empty()){
            std::string varValue = replaceSubstring(variables[buffer], "\"", "");
            l = std::stoi(varValue);
            buffer.clear();
        }else{
            l = stack.top();
            stack.pop();
        };

        node->right->accept(this);
        if (!buffer.empty()){
            std::string varValue = replaceSubstring(variables[buffer], "\"", "");
            f = std::stoi(varValue);
            buffer.clear();
        }else{
            f = stack.top();
            stack.pop();
        };

        std::string op = node->op;
        if (op == "+") {
            stack.push(f+l);
        } else if (op == "-") {
            stack.push(f-l);
        } else if (op == "*") {
            stack.push(f*l);
        } else if (op == "/") {
            if (l == 0) {
                std::cerr << "Division by zero" << std::endl;
                std::ofstream (fileName, std::ios::trunc);
                exit(EXIT_FAILURE);
            } else {
                stack.push(f/l);
            }
        } else {
            std::cerr << "Unsupported operator: " << op << std::endl;
            std::ofstream (fileName, std::ios::trunc);
            exit(EXIT_FAILURE);
        }
    }

    void visit(NumberNode* node) override {
        stack.push(node->value);
    }

    void visit(StringNode* node) override {
        std::string string = replaceSubstring(node->name, "\\n", "%c");
        buffer.append(string);
    }

    void visit(IdentifierNode* node) override {
        auto it = variables.find(node->name);
        if (it == variables.end()) {
            std::cerr << "Variable " << node->name << " not declared " << std::endl;
            std::ofstream (fileName, std::ios::trunc);
            exit(EXIT_FAILURE);
        };
        buffer.append(it->first);
    }

    void visit(PrintNode* node) override {
        node->identifier->accept(this);

        if (isAllDigits(variables.at(buffer))){
            file << "push dword [" << buffer << "]" << std::endl;
            file << "push dword fmt" << std::endl;
            file << "call printf" << std::endl;
        }else{
            file << "push dword " << buffer << "" << std::endl;
            file << "call printf" << std::endl;
        };

        buffer.clear();
    }

    void visit(DeclarationNode* node) override {
        auto it = variables.find(node->identifier->name);

        if (it != variables.end()) {
            std::cerr << "Variable " << node->identifier->name << " already declared " << std::endl;
            std::ofstream (fileName, std::ios::trunc);
            exit(EXIT_FAILURE);
        };

        node->value->accept(this);

        if (!stack.empty()){
            addVariable(node->identifier->name, std::to_string(stack.top()));
            stack.pop();
        }else{
            addVariable(node->identifier->name + "_len", " equ $ - " + node->identifier->name);
            addVariable(node->identifier->name, buffer);
        };

        buffer.clear();
    }

    void visit(AssignmentNode* node) override {
        node->value->accept(this);

        bool varExists = false;
        for (auto& it : variables){
            if (it.first == node->identifier->name) {
                varExists = true;
            };
        };

        if (!varExists){
            std::cerr << "Variable " << node->identifier->name << " not declared " << std::endl;
            std::ofstream (fileName, std::ios::trunc);
            exit(EXIT_FAILURE);
        };

        if (!stack.empty()){
            file << "mov byte [" << node->identifier->name << "]," << stack.top() << std::endl;
            stack.pop();
        }else{
            std::cerr << "Cant reassign String" << std::endl;
            std::ofstream (fileName, std::ios::trunc);
            exit(EXIT_FAILURE);
        };
    }

    void visit(ComparisonNode* node) override {
        std::string label = labelBuffer;

        node->left->accept(this);
        std::string leftOp;
        if (!stack.empty()){
            leftOp = std::to_string(stack.top());
            stack.pop();
        }else{
            leftOp = buffer;
            buffer.clear();
        };

        node->right->accept(this);
        std::string rightOp;
        if (!stack.empty()){
            rightOp = std::to_string(stack.top());
            stack.pop();
        }else{
            rightOp = buffer;
            buffer.clear();
        };

        std::string compOp = node->op;

        if (isAllDigits(leftOp)){
            file << "mov eax, " << leftOp << "" << std::endl;
        }else{
            file << "mov eax, [" << leftOp << "]" << std::endl;
        };

        if (isAllDigits(rightOp)){
            file << "mov ebx, " << rightOp << "" << std::endl;
        }else{
            file << "mov ebx, [" << rightOp << "]" << std::endl;
        };

        file << "cmp eax, ebx" << std::endl;

        if (compOp == "<") {
            file << "jl " << label << std::endl;
            file << std::endl;
        } else if (compOp == ">") {
            file << "jg " << label << std::endl;
            file << std::endl;
        } else if (compOp == "==") {
            file << "je " << label << std::endl;
            file << std::endl;
        } else if (compOp == ">=") {
            file << "jge " << label << std::endl;
            file << std::endl;
        } else if (compOp == "<=") {
            file << "jle " << label << std::endl;
            file << std::endl;
        } else {
            std::cerr << "Unsupported comparison operator: " << compOp << std::endl;
            exit(EXIT_FAILURE);
        }

    }

    void visit(IfStatementNode* node) override {
        int label = ++labelCount;
        labelBuffer.append("if_label_"+std::to_string(label));
        node->condition->accept(this);

        if (!node->falseBody) {
            file << "jmp " << "end_if_label_" << label << std::endl;
        }else{
            file << "jmp " << "else_label_" << label << std::endl;
        };

        file << std::endl << "if_label_" << label << ":" << std::endl;
        node->trueBody->accept(this);
        file << "jmp " << "end_if_label_" << label << std::endl;
        file << std::endl;

        if (node->falseBody) {
            file << "else_label_" << label << ":" << std::endl;
            node->falseBody->accept(this);
            file << "jmp " << "end_if_label_" << label << std::endl;
            file << std::endl;
        }

        file << std::endl << "end_if_label_" << label << ":" << std::endl;
        labelBuffer.clear();
    }

    void visit(IncrementNode* node) override {
        node->identifier->accept(this);
        std::string id = buffer;
        buffer.clear();
        if (node->value == "++"){
            file << "add byte ["<< id << "], 1" << std::endl;
        }else{
            file << "sub byte ["<< id << "], 1" << std::endl;
        };
    }

    void visit(ForLoopNode* node) override {
        int label = ++labelCount;
        labelBuffer.append("for_loop_label_"+std::to_string(label));

        node->initialization->accept(this);
        file << std::endl;
        file << "for_loop_label_" << label << ":" << std::endl;

        node->body->accept(this);
        node->increment->accept(this);
        node->condition->accept(this);

        file << "jmp end_for_loop_" << label << std::endl;
        file << std::endl;
        file << "end_for_loop_" << label << ":" << std::endl;

        labelBuffer.clear();
    }

    void visit(WhileLoopNode* node) override {}
    void visit(FunctionCallNode* node) override {}
    void visit(FunctionDeclarationNode* node) override {}

};

