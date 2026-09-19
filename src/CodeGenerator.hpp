#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include "Visitor.hpp"
#include "Parser.hpp"

class CodeGenerator : public Visitor {
private:
    std::map<std::string, std::string> variables;
    std::map<std::string, FunctionNode *> functions;
    std::set<std::string> stringVariables;
    std::vector<std::set<std::string>> scopes;
    std::stack<int> stack {};
    std::string buffer {};
    std::string labelBuffer {};
    std::string fileName {};
    std::ofstream file {};
    int labelCount {};

    bool isVisible(const std::string& name) const {
        for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope) {
            if (scope->find(name) != scope->end()) {
                return true;
            }
        }
        return false;
    }

    void enterScope() {
        scopes.emplace_back();
    }

    void leaveScope() {
        scopes.pop_back();
    }

    struct EvaluationResult {
        bool returned;
        int value;
    };

    int evaluateExpression(AstNode *node, std::map<std::string, int>& environment) {
        if (auto number = dynamic_cast<NumberNode *>(node)) {
            return number->value;
        }
        if (auto identifier = dynamic_cast<IdentifierNode *>(node)) {
            auto value = environment.find(identifier->name);
            if (value == environment.end()) {
                std::cerr << "Variable " << identifier->name << " not declared in function" << std::endl;
                exit(EXIT_FAILURE);
            }
            return value->second;
        }
        if (auto binary = dynamic_cast<BinaryOpNode *>(node)) {
            int left = evaluateExpression(binary->left, environment);
            int right = evaluateExpression(binary->right, environment);
            if (binary->op == "+") return left + right;
            if (binary->op == "-") return right - left;
            if (binary->op == "*") return left * right;
            if (binary->op == "/") {
                if (left == 0) {
                    std::cerr << "Division by zero" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return right / left;
            }
        }
        if (auto comparison = dynamic_cast<ComparisonNode *>(node)) {
            int left = evaluateExpression(comparison->left, environment);
            int right = evaluateExpression(comparison->right, environment);
            if (comparison->op == "==") return left == right;
            if (comparison->op == "!=") return left != right;
            if (comparison->op == "<") return left < right;
            if (comparison->op == "<=") return left <= right;
            if (comparison->op == ">") return left > right;
            if (comparison->op == ">=") return left >= right;
        }
        if (auto call = dynamic_cast<CallNode *>(node)) {
            std::vector<int> arguments;
            for (AstNode *argument : call->arguments) {
                arguments.push_back(evaluateExpression(argument, environment));
            }
            return evaluateFunction(call->name, arguments);
        }

        std::cerr << "Unsupported expression in function" << std::endl;
        exit(EXIT_FAILURE);
    }

    EvaluationResult evaluateStatement(AstNode *node, std::map<std::string, int>& environment) {
        if (auto declaration = dynamic_cast<DeclarationNode *>(node)) {
            environment[declaration->identifier->name] = evaluateExpression(declaration->value, environment);
            return {false, 0};
        }
        if (auto assignment = dynamic_cast<AssignmentNode *>(node)) {
            if (environment.find(assignment->identifier->name) == environment.end()) {
                std::cerr << "Variable " << assignment->identifier->name << " not declared in function" << std::endl;
                exit(EXIT_FAILURE);
            }
            environment[assignment->identifier->name] = evaluateExpression(assignment->value, environment);
            return {false, 0};
        }
        if (auto returnNode = dynamic_cast<ReturnNode *>(node)) {
            return {true, evaluateExpression(returnNode->value, environment)};
        }
        if (auto conditional = dynamic_cast<IfStatementNode *>(node)) {
            int condition = evaluateExpression(conditional->condition, environment);
            if (condition) return evaluateStatements(static_cast<ProgramNode *>(conditional->trueBody), environment);
            if (conditional->falseBody) return evaluateStatements(static_cast<ProgramNode *>(conditional->falseBody), environment);
            return {false, 0};
        }
        if (auto loop = dynamic_cast<ForLoopNode *>(node)) {
            evaluateStatement(loop->initialization, environment);
            while (evaluateExpression(loop->condition, environment)) {
                EvaluationResult result = loop->body
                        ? evaluateStatements(static_cast<ProgramNode *>(loop->body), environment)
                        : EvaluationResult{false, 0};
                if (result.returned) return result;
                if (auto increment = dynamic_cast<IncrementNode *>(loop->increment)) {
                    auto identifier = dynamic_cast<IdentifierNode *>(increment->identifier);
                    int &value = environment[identifier->name];
                    value += increment->value == "++" ? 1 : -1;
                }
            }
            return {false, 0};
        }

        std::cerr << "Unsupported statement in function" << std::endl;
        exit(EXIT_FAILURE);
    }

    EvaluationResult evaluateStatements(ProgramNode *program, std::map<std::string, int>& environment) {
        for (AstNode *statement : program->statements) {
            EvaluationResult result = evaluateStatement(statement, environment);
            if (result.returned) return result;
        }
        return {false, 0};
    }

    int evaluateFunction(const std::string& name, const std::vector<int>& arguments) {
        auto function = functions.find(name);
        if (function == functions.end()) {
            std::cerr << "Function " << name << " not declared" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (function->second->parameters.size() != arguments.size()) {
            std::cerr << "Wrong number of arguments for function " << name << std::endl;
            exit(EXIT_FAILURE);
        }

        std::map<std::string, int> environment;
        for (size_t index = 0; index < arguments.size(); ++index) {
            environment[function->second->parameters[index]] = arguments[index];
        }
        EvaluationResult result = evaluateStatements(function->second->body, environment);
        if (!result.returned) {
            std::cerr << "Function " << name << " did not return a value" << std::endl;
            exit(EXIT_FAILURE);
        }
        return result.value;
    }

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
        if (str.empty()) {
            return false;
        }

        size_t start = str.front() == '-' ? 1 : 0;
        if (start == str.size()) {
            return false;
        }

        for (size_t index = start; index < str.size(); ++index) {
            if (!std::isdigit(static_cast<unsigned char>(str[index]))) {
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
        scopes.clear();
        enterScope();

        file << "section .text" << std::endl;
        file << "global _start" << std::endl;
        file << "extern printf" << std::endl;
        file << "extern exit" << std::endl;
        file << "_start:" << std::endl;
        file << std::endl;

        node->accept(this);

        file << std::endl << "push dword 0" << std::endl;
        file << "call exit" << std::endl;

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
                    if (stringVariables.find(variable.first) == stringVariables.end()){
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
            if (auto function = dynamic_cast<FunctionNode *>(statement)) {
                if (functions.find(function->name) != functions.end()) {
                    std::cerr << "Function " << function->name << " already declared" << std::endl;
                    exit(EXIT_FAILURE);
                }
                functions[function->name] = function;
            }
        }
        for (AstNode* statement : node->statements) {
            if (dynamic_cast<FunctionNode *>(statement) == nullptr) {
                statement->accept(this);
            }
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
        if (it == variables.end() || !isVisible(node->name)) {
            std::cerr << "Variable " << node->name << " not declared " << std::endl;
            std::ofstream (fileName, std::ios::trunc);
            exit(EXIT_FAILURE);
        };
        buffer.append(it->first);
    }

    void visit(PrintNode* node) override {
        node->identifier->accept(this);

        if (stringVariables.find(buffer) == stringVariables.end()){
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
            stringVariables.insert(node->identifier->name);
        };

        scopes.back().insert(node->identifier->name);

        buffer.clear();
    }

    void visit(AssignmentNode* node) override {
        node->value->accept(this);

        if (!isVisible(node->identifier->name)) {
            std::cerr << "Variable " << node->identifier->name << " not declared " << std::endl;
            std::ofstream (fileName, std::ios::trunc);
            exit(EXIT_FAILURE);
        };

        if (!stack.empty()){
            file << "mov dword [" << node->identifier->name << "]," << stack.top() << std::endl;
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
        std::string previousLabel = labelBuffer;
        labelBuffer = "if_label_" + std::to_string(label);
        node->condition->accept(this);

        if (!node->falseBody) {
            file << "jmp " << "end_if_label_" << label << std::endl;
        }else{
            file << "jmp " << "else_label_" << label << std::endl;
        };

        file << std::endl << "if_label_" << label << ":" << std::endl;
        enterScope();
        node->trueBody->accept(this);
        leaveScope();
        file << "jmp " << "end_if_label_" << label << std::endl;
        file << std::endl;

        if (node->falseBody) {
            file << "else_label_" << label << ":" << std::endl;
            enterScope();
            node->falseBody->accept(this);
            leaveScope();
            file << "jmp " << "end_if_label_" << label << std::endl;
            file << std::endl;
        }

        file << std::endl << "end_if_label_" << label << ":" << std::endl;
        labelBuffer = previousLabel;
    }

    void visit(IncrementNode* node) override {
        node->identifier->accept(this);
        std::string id = buffer;
        buffer.clear();
        if (node->value == "++"){
            file << "add dword ["<< id << "], 1" << std::endl;
        }else{
            file << "sub dword ["<< id << "], 1" << std::endl;
        };
    }

    void visit(FunctionNode* /*node*/) override {}

    void visit(CallNode* node) override {
        std::vector<int> arguments;
        for (AstNode *argument : node->arguments) {
            argument->accept(this);
            if (stack.empty()) {
                if (buffer.empty() || stringVariables.find(buffer) != stringVariables.end()) {
                    std::cerr << "Function arguments must be integers" << std::endl;
                    exit(EXIT_FAILURE);
                }
                arguments.push_back(std::stoi(variables.at(buffer)));
                buffer.clear();
                continue;
            }
            arguments.push_back(stack.top());
            stack.pop();
        }
        stack.push(evaluateFunction(node->name, arguments));
    }

    void visit(ReturnNode* /*node*/) override {
        std::cerr << "Return is only valid inside a function" << std::endl;
        exit(EXIT_FAILURE);
    }

    void visit(ForLoopNode* node) override {
        int label = ++labelCount;
        std::string previousLabel = labelBuffer;
        labelBuffer = "for_loop_label_" + std::to_string(label);

        enterScope();
        node->initialization->accept(this);
        file << std::endl;
        file << "for_loop_label_" << label << ":" << std::endl;

        enterScope();
        if (node->body) {
            node->body->accept(this);
        }
        leaveScope();
        node->increment->accept(this);
        node->condition->accept(this);

        file << "jmp end_for_loop_" << label << std::endl;
        file << std::endl;
        file << "end_for_loop_" << label << ":" << std::endl;

        leaveScope();
        labelBuffer = previousLabel;
    }

};

