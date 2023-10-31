#include <fstream>
#include <sstream>

#include "Parser.hpp"
#include "CodeGenerator.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Input error" << std::endl;
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream inputFile(filename);
    std::string line;
    std::string source;

    while (getline(inputFile, line)) {
        source += line;
    }
    inputFile.close();

    Tokenizer tokenizer(source);
    std::vector<Token> tokens = tokenizer.tokenize();

    Parser parser(tokens);
    AstNode* AST = parser.parseProgram();

    CodeGenerator codeGenerator;
    codeGenerator.generateCode(AST);

    return 0;
}