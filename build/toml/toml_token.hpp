#ifndef C4C_TOML_TOKENS_H
#define C4C_TOML_TOKENS_H

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
    TOK_DATETIME,   // 1979-05-27T07:32:00Z (optional early)
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

    TomlTokens(TomlTokenType type, std::string string, int start, int end, int col, int line = 0)
    {
        this->type = type;
        this->lexeme = string;
        this->start = start;
        this->end = end;
        this->line = line;
        this->col = col;
    }

    TomlTokens() = default;

    std::string get_name()
    {
        int type = static_cast<int>(this->type);
        return TOML_TOKEN_NAME[type];
    }

    void print()
    {
        int type = static_cast<int>(this->type);
        std::cout << "  " << this->lexeme << "    ==>    " << TOML_TOKEN_NAME[type] << std::endl;
        return;
    }
};

#endif // C4C_TOML_TOKENS_H