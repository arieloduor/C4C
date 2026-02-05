#include <iostream>
#include <fstream>
#include <sstream>
#include "toml_lexer.hpp"
#include "toml_parser.hpp"

// Function to read file content
std::string read_file(const char *filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char *argv[])
{
    // Read the TOML file
    std::string toml_content = read_file("test.toml");

    if (toml_content.empty())
    {
        std::cerr << "Error: TOML file is empty or could not be read" << std::endl;
        return 1;
    }

    std::cout << "=== TOML Lexer Test ===" << std::endl;
    std::cout << "\nInput TOML:" << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << toml_content << std::endl;
    std::cout << "-----------" << std::endl;

    // Create lexer and scan tokens
    TomlLexer lexer(toml_content);
    std::vector<TomlTokens> tokens = lexer.scan_tokens();

    // // Print all tokens
    // std::cout << "\nTokens Generated:" << std::endl;
    // std::cout << "-----------------" << std::endl;

    // for (size_t i = 0; i < tokens.size(); i++)
    // {
    //     std::cout << "[" << i << "] ";
    //     tokens[i].print();
    // }

    // std::cout << "\nTotal tokens: " << tokens.size() << std::endl;


    TomlParser parser("test.toml", tokens);
    parser.parse_toml();

    return 0;
}
