/****************************************************************************************************\
 * FILE: toml_token.hpp                                                                             *
 * AUTHOR: Michael Kamau                                                                            *
 *                                                                                                  *
 * PURPOSE: This file defines the token types and token representation used by the TOML lexer.      *
 *                                                                                                  *
 * The file provides:                                                                               *
 * - TomlTokenType enum: An enumeration of all possible token types in TOML                         *
 * - TOML_TOKEN_NAME array: String representations of each token type for debugging/display         *
 * - TomlTokens class: A container holding information about a single lexed token                   *
 *                                                                                                  *
 * Token types include:                                                                             *
 * - Special tokens: EOF (end of file), ERROR (lexical error)                                       *
 * - Identifiers and strings: bare keys and quoted string literals                                  *
 * - Literals: integers, floats, booleans, and various datetime formats                             *
 * - Operators: =, ., ,                                                                             *
 * - Brackets: [ ] [[ ]] { }                                                                        *
 * - Structural: newlines                                                                           *
 *                                                                                                  *
 * USAGE: The lexer produces a vector of TomlTokens objects. Each token stores:                     *
 *        - type: The kind of token (TomlTokenType enum value)                                      *
 *        - lexeme: The actual text from the source                                                 *
 *        - line/col: Position information for error reporting                                      *
 *        - start/end: Byte offsets in the source document                                          *
 *                                                                                                  *
 * DESIGN NOTES:                                                                                    *
 * - Token types are zero-indexed and must align with TOML_TOKEN_NAME array                         *
 * - The get_name() method maps enum values to their string representations                         *
 * - The print() method outputs token information in a human-readable format                        *
 *                                                                                                  *
 ****************************************************************************************************/

#ifndef C4C_TOML_TOKENS_HPP
#define C4C_TOML_TOKENS_HPP

#include <string>

std::string TOML_TOKEN_NAME[] = {

    /* Special */
    "TOK_EOF",
    "TOK_ERROR",
    "TOK_NAN",
    "TOK_INF",

    /* Identifiers */
    "TOK_IDENTIFIER", // bare keys: foo, bar_baz, hello123
    "TOK_STRING",     // "value" or 'value'

    /* Literals */
    "TOK_INTEGER", // 42, -7
    "TOK_FLOAT",   // 3.14, 1e10
    "TOK_BOOLEAN", // true, false

    "TOK_DATETIME",   // 1979-05-27T07:32:00Z
    "TOK_LOCAL_DATE", // 1979-05-27
    "TOK_LOCAL_TIME",
    "TOK_LOCAL_DATETIME",
    "TOK_OFFSET_DATETIME",

    /* Symbols */
    "TOK_EQUAL", // =
    "TOK_DOT",   // .
    "TOK_COMMA", // ,

    /* Brackets */
    "TOK_LBRACKET",  // [
    "TOK_RBRACKET",  // ]
    "TOK_LDBRACKET", // [[
    "TOK_RDBRACKET", // ]]
    "TOK_LBRACE",    // {
    "TOK_RBRACE",    // }

    /* Newlines */
    "TOK_NEWLINE"};

/**
 * TomlTokenType Enumeration
 * 
 * Defines all possible token types that can appear in a TOML document.
 * Each value corresponds to an index in the TOML_TOKEN_NAME array for
 * efficient string lookup and debugging output.
 * 
 * Special tokens mark end of file or lexical errors.
 * Identifier tokens represent bare keys and string literals.
 * Literal tokens represent values: integers, floats, booleans, and various datetime formats.
 * Symbol tokens represent operators: =, ., ,
 * Bracket tokens represent structural delimiters: [ ] [[ ]] { }
 * Newline tokens mark line breaks in the source.
 */
enum class TomlTokenType
{
    /* Special */
    TOK_EOF,
    TOK_ERROR,
    TOK_NAN,
    TOK_INF,

    /* Identifiers */
    TOK_IDENTIFIER, // bare keys: foo, bar_baz, hello123
    TOK_STRING,     // "value" or 'value'

    /* Literals */
    TOK_INTEGER,    // 42, -7
    TOK_FLOAT,      // 3.14, 1e10
    TOK_BOOLEAN,    // true, false
    TOK_DATETIME,   // 1979-05-27T07:32:00Z 
    TOK_LOCAL_DATE, // 1979-05-27
    TOK_LOCAL_TIME,
    TOK_LOCAL_DATETIME,
    TOK_OFFSET_DATETIME,

    /* Symbols */
    TOK_EQUAL, // =
    TOK_DOT,   // .
    TOK_COMMA, // ,

    /* Brackets */
    TOK_LBRACKET,  // [
    TOK_RBRACKET,  // ]
    TOK_LDBRACKET, // [[
    TOK_RDBRACKET, // ]]
    TOK_LBRACE,    // {
    TOK_RBRACE,    // }

    /* Newlines */
    TOK_NEWLINE
};

/**
 * TomlTokens Class
 * 
 * Represents a single lexical token extracted from a TOML document.
 * This class encapsulates all information about a token including its type,
 * actual text content, and source location for error reporting.
 *
 * Members:
 * - type: The category of token (from TomlTokenType enum)
 * - lexeme: The exact text from the source document
 * - length: The length of the lexeme (currently unused)
 * - line: Source line number (1-based)
 * - col: Source column number (1-based)
 * - start: Byte offset of token start in source
 * - end: Byte offset of token end in source
 *
 * Methods:
 * - Constructor: Initializes token with type, text, and position info
 * - get_name(): Maps token type to its string representation
 * - print(): Outputs token in format: "lexeme ==> TOKEN_TYPE"
 */
class TomlTokens
{

public:
    TomlTokenType type;
    std::string lexeme;
    size_t length;
    size_t line;
    size_t col;
    size_t start;
    size_t end;

    /**
     * Constructs a TomlTokens object with complete token information.
     * 
     * Parameters:
     * - type: The token type (TomlTokenType enum value)
     * - string: The lexeme text from the source
     * - start: Byte offset where token starts in source
     * - end: Byte offset where token ends in source
     * - col: Column number in the source line
     * - line: Line number in the source (default 0)
     */
    TomlTokens(TomlTokenType type, std::string string, int start, int end, int col, int line = 0)
    {
        this->type = type;
        this->lexeme = string;
        this->start = start;
        this->end = end;
        this->line = line;
        this->col = col;
    }

    /**
     * Default constructor. Initializes all members to zero/empty.
     */
    TomlTokens() = default;

    /**
     * Returns the string representation of this token's type.
     * 
     * Maps the type enum value to its corresponding string from the
     * TOML_TOKEN_NAME array. Used for debugging and error messages.
     * 
     * Returns: String name of the token type (e.g., "TOK_STRING")
     */
    std::string get_name()
    {
        int type = static_cast<int>(this->type);
        return TOML_TOKEN_NAME[type];
    }

    /**
     * Prints the token information to standard output.
     * 
     * Output format: "  [lexeme]    ==>    [TOKEN_TYPE]"
     * This is useful for token stream inspection and debugging during lexing.
     */
    void print()
    {
        int type = static_cast<int>(this->type);
        std::cout << "  " << this->lexeme << "    ==>    " << TOML_TOKEN_NAME[type] << std::endl;
        return;
    }
};

#endif // C4C_TOML_TOKENS_HPP