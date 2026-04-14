#include <stdio.h>
#include <fstream>
#include <sstream>
#include <memory>

#include "Token.h"
#include "AST.h"
#include "Codegen.h"

int main(int argc, char* argv[]) {
    printf("MAIN START\n");
    fflush(stdout);
    
    if (argc < 2) {
        printf("Usage: compiler <input.c> [-o output.asm]\n");
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = "output.asm";

    std::ifstream in(inputFile);
    if (!in.is_open()) {
        printf("Error: Cannot open file %s\n", inputFile.c_str());
        return 1;
    }

    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string source = buffer.str();
    in.close();

    printf("Source size: %zu\n", source.size());
    fflush(stdout);
    
    printf("Creating lexer...\n");
    fflush(stdout);
    
    Lexer lexer(source);
    printf("Tokenizing...\n");
    fflush(stdout);
    
    try {
        lexer.tokenize();
    } catch (...) {
        printf("Lexer error\n");
        return 1;
    }

    printf("Tokens: %zu\n", lexer.tokens.size());
    fflush(stdout);
    
    printf("Creating parser...\n");
    fflush(stdout);
    
    Parser parser(lexer.tokens);
    printf("Parsing...\n");
    fflush(stdout);
    
    std::unique_ptr<ProgramNode> program;
    try {
        program = parser.parse();
    } catch (...) {
        printf("Parser error\n");
        return 1;
    }

    printf("Parsed, funcs: %zu\n", program->functions.size());
    fflush(stdout);
    
    printf("Creating codegen...\n");
    fflush(stdout);
    
    CodeGen codegen;
    printf("Generating...\n");
    fflush(stdout);
    
    codegen.generate(program.get());

    printf("Codegen output: %zu\n", codegen.output.size());
    fflush(stdout);

    printf("Writing output...\n");
    std::ofstream out(outputFile);
    out << codegen.output;
    out.close();

    printf("Done: %s\n", outputFile.c_str());
    return 0;
}