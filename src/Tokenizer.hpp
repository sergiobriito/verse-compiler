#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <iostream>
#include <optional>
#include <vector>

enum class TokenType {
    IDENT = 0,
    NUMBER = 1,
    STRING = 2,
    PLUS = 3,
    MINUS = 4,
    STAR = 5,
    DIVIDE = 6,
    COMMA = 7,
    SEMI_COL = 8,
    OPENPAR = 9,
    CLOSPAR = 10,
    OPENCURL = 11,
    CLOSCURL = 12,
    OPENSQUAR = 13,
    CLOSSQUAR = 14,
    LET = 15,
    IF = 16,
    ELSE = 17,
    EQ = 18,
    EQEQ = 19,
    GT = 20,
    GTEQ = 21,
    LT = 22,
    LTEQ = 23,
    NOTEQ = 24,
    FOR = 25,
    WHILE = 26,
    PRINT = 27,
    INCVALUE = 30,
    DECVALUE = 31
};

struct Token {
    std::string value{};
    TokenType type;
};

class Tokenizer {
private:
    std::string source;
    int idx = 0;

public:
    explicit Tokenizer(std::string source) : source(std::move(source)){};

    std::optional<char> peek(int offset = 0) {
        if (idx + offset >= source.size()) {
            return {};
        };
        return source[idx + offset];
    };

    char consume() { return source[idx++]; };

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        std::string buffer;

        while (peek().has_value()) {
            if (peek().value() == '\"') {
                consume();
                while (peek().has_value() && std::isalnum(peek().value()) || isspace(peek().value()) || peek().value() == '\\') {
                    buffer.push_back(consume());
                };
                if (peek().value() != '\"') {
                    std::cout << "Expected \" " << std::endl;
                    exit(EXIT_FAILURE);
                };
                consume();
                tokens.push_back({.value = buffer, .type = TokenType::STRING});
                buffer.clear();
            } else if (std::isalpha(peek().value())) {
                buffer.push_back(consume());
                if (peek().has_value() && peek().value() == '+' && peek(1).value() =='+'){
                    buffer.push_back(consume());
                    buffer.push_back(consume());
                    tokens.push_back({.value = buffer, .type = TokenType::INCVALUE});
                } else if(peek().has_value() && peek().value() == '-' && peek(1).value() =='-'){
                    buffer.push_back(consume());
                    buffer.push_back(consume());
                    tokens.push_back({.value = buffer, .type = TokenType::DECVALUE});
                } else{
                    while (peek().has_value() && std::isalnum(peek().value())) {
                        buffer.push_back(consume());
                    };
                    if (buffer == ",") {
                        tokens.push_back({.value = buffer, .type = TokenType::COMMA});
                    } else if (buffer == "let") {
                        tokens.push_back({.value = buffer, .type = TokenType::LET});
                    } else if (buffer == "if") {
                        tokens.push_back({.value = buffer, .type = TokenType::IF});
                    } else if (buffer == "else") {
                        tokens.push_back({.value = buffer, .type = TokenType::ELSE});
                    } else if (buffer == "for") {
                        tokens.push_back({.value = buffer, .type = TokenType::FOR});
                    } else if (buffer == "while") {
                        tokens.push_back({.value = buffer, .type = TokenType::WHILE});
                    } else if (buffer == "print") {
                        tokens.push_back({.value = buffer, .type = TokenType::PRINT});
                    } else {
                        tokens.push_back({.value = buffer, .type = TokenType::IDENT});
                    };
                };
                buffer.clear();
            } else if (std::isdigit(peek().value())) {
                buffer.push_back(consume());
                while (peek().has_value() && std::isdigit(peek().value())) {
                    buffer.push_back(consume());
                };

                tokens.push_back({.value = buffer, .type = TokenType::NUMBER});
                buffer.clear();
            } else if (std::isspace(peek().value())) {
                consume();
            } else if (peek().value() == '(') {
                consume();
                tokens.push_back({.type = TokenType::OPENPAR});
            } else if (peek().value() == ')') {
                consume();
                tokens.push_back({.type = TokenType::CLOSPAR});
            } else if (peek().value() == '{') {
                consume();
                tokens.push_back({.type = TokenType::OPENCURL});
            } else if (peek().value() == '}') {
                consume();
                tokens.push_back({.type = TokenType::CLOSCURL});
            } else if (peek().value() == '[') {
                consume();
                tokens.push_back({.type = TokenType::OPENSQUAR});
            } else if (peek().value() == ']') {
                consume();
                tokens.push_back({.type = TokenType::CLOSSQUAR});
            } else if (peek().value() == '+') {
                consume();
                tokens.push_back({.value = "+" , .type = TokenType::PLUS});  
            } else if (peek().value() == '-') {
                consume();
                tokens.push_back({.value = "-" , .type = TokenType::MINUS});
            } else if (peek().value() == '*') {
                consume();
                tokens.push_back({.value = "*", .type = TokenType::STAR});
            } else if (peek().value() == '/') {
                consume();
                tokens.push_back({.value = "/", .type = TokenType::DIVIDE});
            } else if (peek().value() == ';') {
                consume();
                tokens.push_back({.type = TokenType::SEMI_COL});
            } else if (peek().value() == '=') {
                consume();
                if (peek().value() == '=') {
                    consume();
                    tokens.push_back({.value = "==", .type = TokenType::EQEQ});
                } else {
                    tokens.push_back({.value = "=", .type = TokenType::EQ});
                };
            } else if (peek().value() == '>') {
                consume();
                if (peek().value() == '=') {
                    consume();
                    tokens.push_back({.value = ">=" , .type = TokenType::GTEQ});
                } else {
                    tokens.push_back({.value = ">" , .type = TokenType::GT});
                };
            } else if (peek().value() == '<') {
                consume();
                if (peek().value() == '=') {
                    consume();
                    tokens.push_back({.value = "<=" , .type = TokenType::LTEQ});
                } else {
                    tokens.push_back({.value = "<" , .type = TokenType::LT});
                };
            } else if (peek().value() == '!') {
                consume();
                if (peek().value() == '=') {
                    consume();
                    tokens.push_back({.value = "!=" , .type = TokenType::NOTEQ});
                } else {
                    std::cout << "Invalid syntax (" << idx << ") -> " << peek().value()
                              << std::endl;
                    exit(EXIT_FAILURE);
                };
            } else {
                std::cout << "Invalid syntax (" << idx << ") -> " << peek().value()
                          << std::endl;
                exit(EXIT_FAILURE);
            };
        };

        idx = 0;
        return tokens;
    };
};

#endif