/****************************************************************************************************\
 * FILE: toml_parser.hpp                                                                            *
 * AUTHOR: Michael Kamau                                                                            *
 *                                                                                                  *
 * PURPOSE: This file implements a recursive-descent parser for TOML documents.                     *
 * The parser consumes a linear stream of lexical tokens produced by the TomlLexer                  *
 * and constructs a hierarchical TomlDocument representing the TOML data structure.                 *
 *                                                                                                  *
 * The parser handles all TOML constructs including:                                                *
 * - Key-value pairs: simple assignments and dotted key paths                                       *
 * - Tables: [table] sections and nested [table.subtable] hierarchies                               *
 * - Array of tables: [[array.of.tables]] declarations                                              *
 * - Arrays: homogeneous collections [...] with comma-separated values                              *
 * - Inline tables: {...} structured data on a single line                                          *
 * - All value types: strings, integers, floats, booleans, and datetime values                      *
 *                                                                                                  *
 * The parser operates in a single pass, maintaining a cursor over the token stream.                *
 *                                                                                                  *
 * USAGE: To parse a TOML document:                                                                 *
 *        1. Tokenize with TomlLexer: `TomlLexer lexer(source); tokens = lexer.scan_tokens();`      *
 *        2. Create parser: `TomlParser parser("file.toml", tokens);`                               *
 *        3. Parse: `TomlDocument doc = parser.parse_toml();`                                       *
 *                                                                                                  *
 * OUTPUT: Returns a TomlDocument with a root TomlValue (always a table) containing                 *
 *         the entire parsed TOML structure                                                         *
 *                                                                                                  *
 * DESIGN NOTES:                                                                                    *
 * - Parsing strategy: recursive descent with lookahead                                             *
 * - Error handling: fatal errors halt parsing immediately                                          *
 * - Token consumption: strict left-to-right without backtracking                                   *
 *                                                                                                  *
 ****************************************************************************************************/

#ifndef C4C_TOML_PARSER_HPP
#define C4C_TOML_PARSER_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include "toml_token.hpp"

#define panic(msg)                           \
    do                                       \
    {                                        \
        printf("An error occured: %s", msg); \
        exit(1);                             \
    } while (0)

#define debug_print(tok) printf("DEBUG: for token %s of token name : %s on line: %ld\n", (tok).lexeme.c_str(), (tok).get_name().c_str(), (tok).line)

enum class TomlType
{
    String,
    Integer,
    Float,
    Boolean,
    LocalDate,
    LocalTime,
    LocalDateTime,
    OffsetDateTime,
    Array,
    Table
};

class TomlValue
{
public:
    TomlType type;

    std::string string_value;
    int64_t int_value;
    double float_value;
    bool bool_value;

    std::vector<TomlValue> array_value;
    std::unordered_map<std::string, TomlValue> table_value;
};

class TomlDocument
{
public:
    TomlValue root; // always a table
};

class TomlParser
{

    inline bool not_end()
    {
        return this->index < this->tokens_length;
    }

    TomlTokens peek(int lookahead = 0)
    {
        int idx = this->index + lookahead;
        if (idx >= this->tokens_length)
        {
            print_error("Out of bound!", peek());
        }
        return this->tokens[idx];
    }

    inline TomlTokens consume()
    {
        if (this->index + 1 > this->tokens_length)
        {
            print_error("Unexpected end of file", peek());
        }
        return this->tokens[this->index++];
    }

    inline bool expect_token(TomlTokenType type, int lookahead = 0)
    {
        TomlTokens token = peek(lookahead);
        return token.type == type;
    }

    void parse_statement(TomlValue *&current_table)
    {
        while (expect_token(TomlTokenType::TOK_NEWLINE))
        {
            consume();
        }

        if (expect_token(TomlTokenType::TOK_LDBRACKET))
        {
            current_table = parse_array_table(current_table);
        }
        else if (expect_token(TomlTokenType::TOK_LBRACKET))
        {
            current_table = parse_table(current_table);
        }
        else if (expect_token(TomlTokenType::TOK_IDENTIFIER))
        {
            parse_key_value(current_table);
        }
        else if (expect_token(TomlTokenType::TOK_EOF))
        {
            consume();
            return;
        }
        else
        {
            print_error("Unexpected token in statement", peek());
        }
    }

    TomlValue *parse_array_table(TomlValue *root)
    {
        consume(); // consume '[['

        auto keys = parse_key_path();

        if (!expect_token(TomlTokenType::TOK_RDBRACKET))
            print_error("Expected ']]' after array table name", peek());

        consume(); // consume ']]'

        TomlValue *current = root;

        // Traverse all keys except the last
        for (size_t i = 0; i < keys.size() - 1; i++)
        {
            auto &key = keys[i];

            if (!current->table_value.count(key))
            {
                current->table_value[key].type = TomlType::Table;
            }

            current = &current->table_value[key];

            if (current->type != TomlType::Table)
                print_error("Key path conflicts with non-table", peek());
        }

        const std::string &array_key = keys.back();

        // Ensure array exists
        if (!current->table_value.count(array_key))
        {
            TomlValue arr;
            arr.type = TomlType::Array;
            current->table_value[array_key] = arr;
        }

        TomlValue &array = current->table_value[array_key];

        if (array.type != TomlType::Array)
            print_error("Array-of-table conflicts with non-array", peek());

        // Create new table and append
        TomlValue new_table;
        new_table.type = TomlType::Table;

        array.array_value.push_back(new_table);

        // Return pointer to the newly created table
        return &array.array_value.back();
    }

    TomlValue parse_ld_array()
    {

        consume();
        TomlValue arr;
        arr.type = TomlType::Array;

        if (expect_token(TomlTokenType::TOK_RDBRACKET, 1))
        {
            consume();
            return arr;
        }

        while (true)
        {
            TomlValue main;

            while (true)
            {
                TomlValue v = parse_value();
                main.array_value.push_back(v);

                if (expect_token(TomlTokenType::TOK_COMMA))
                {
                    consume();
                    continue;
                }
                break;
            }

            if (expect_token(TomlTokenType::TOK_RDBRACKET))
            {
                break;
            }

            if (not expect_token(TomlTokenType::TOK_RBRACKET))
            {
                print_error("Expected ']' but found none", peek());
            }
            consume(); // conusme the left brace;

            arr.array_value.push_back(main);
            if (expect_token(TomlTokenType::TOK_COMMA) and expect_token(TomlTokenType::TOK_LBRACKET, 1))
            {
                consume(); // consume comma
                consume(); // consume [
                continue;
            }
            break;
        }

        if (not expect_token(TomlTokenType::TOK_RDBRACKET))
        {
            print_error("Expected ']]' but found none", peek());
        }
        consume(); // consume

        return arr;
    }

    TomlValue parse_array()
    {
        consume(); // '['
        while (expect_token(TomlTokenType::TOK_NEWLINE))
        {
            consume(); // to support multiline array
        }

        TomlValue arr;
        arr.type = TomlType::Array;

        if (expect_token(TomlTokenType::TOK_RBRACKET))
        {
            consume(); // empty array
            return arr;
        }

        while (true)
        {
            TomlValue v = parse_value();
            arr.array_value.push_back(v);

            if (expect_token(TomlTokenType::TOK_COMMA) and expect_token(TomlTokenType::TOK_NEWLINE, 1))
            {
                consume(); // consume the comma
                while (expect_token(TomlTokenType::TOK_NEWLINE))
                {
                    consume(); // to support multiline array
                }
                continue;
            }
            else if (expect_token(TomlTokenType::TOK_COMMA))
            {
                consume();
                continue;
            }
            break;
        }

        while (expect_token(TomlTokenType::TOK_NEWLINE))
        {
            consume(); // to support multiline array
        }

        if (!expect_token(TomlTokenType::TOK_RBRACKET))
        {
            print_error("Expected ']' at end of array", peek());
        }

        consume();
        return arr;
    }

    TomlValue *parse_table(TomlValue *root)
    {
        consume(); // '['

        auto keys = parse_key_path();

        if (!expect_token(TomlTokenType::TOK_RBRACKET))
            print_error("Expected ']' after table name", peek());

        consume();

        TomlValue *current = root;
        for (auto &key : keys)
        {
            if (!current->table_value.count(key))
            {
                current->table_value[key].type = TomlType::Table;
            }
            current = &current->table_value[key];

            if (current->type != TomlType::Table)
                print_error("Table name conflicts with non-table", peek());
        }

        return current;
    }

    TomlValue parse_inline_table()
    {

        consume();
        while (expect_token(TomlTokenType::TOK_NEWLINE))
        {
            consume(); // to support multiline array
        }

        TomlValue table;
        table.type = TomlType::Table;

        if (expect_token(TomlTokenType::TOK_RBRACE))
        {
            consume(); // consume }
            return table;
        }

        while (true)
        {

            auto keys = parse_key_path();

            if (!expect_token(TomlTokenType::TOK_EQUAL))
            {
                print_error("Expected '=' after identifier", peek());
            }
            consume(); // consume the equal sign

            TomlValue v;
            v = parse_value();
            insert_value(&table, keys, v);

            if (expect_token(TomlTokenType::TOK_COMMA))
            {
                consume();
                continue;
            }

            break;
        }

        if (expect_token(TomlTokenType::TOK_COMMA))
        {
            consume(); // accept trailing comma
        }

        while (expect_token(TomlTokenType::TOK_NEWLINE))
        {
            consume(); // to support multiline inline table
        }

        if (!expect_token(TomlTokenType::TOK_RBRACE))
        {
            print_error("Expected } but got none", peek());
        }
        consume(); // consume the right brace;

        return table;
    }

    TomlValue parse_value()
    {
        TomlTokens tok = peek();

        TomlValue v;

        switch (tok.type)
        {
        case TomlTokenType::TOK_STRING:
            v.type = TomlType::String;
            v.string_value = tok.lexeme;
            consume();
            break;

        case TomlTokenType::TOK_INTEGER:
            v.type = TomlType::Integer;
            v.int_value = std::stoi(tok.lexeme);
            consume();
            break;

        case TomlTokenType::TOK_FLOAT:
            v.type = TomlType::Float;
            v.float_value = std::stof(tok.lexeme);
            consume();
            break;

        case TomlTokenType::TOK_BOOLEAN:
            v.type = TomlType::Boolean;
            v.bool_value = (tok.lexeme == "true");
            consume();
            break;

        case TomlTokenType::TOK_LOCAL_DATE:
            v.type = TomlType::LocalDate;
            v.string_value = tok.lexeme;
            consume();
            break;

        case TomlTokenType::TOK_LOCAL_TIME:
            v.type = TomlType::LocalTime;
            v.string_value = tok.lexeme;
            consume();
            break;

        case TomlTokenType::TOK_LOCAL_DATETIME:
            v.type = TomlType::LocalDateTime;
            v.string_value = tok.lexeme;
            consume();
            break;

        case TomlTokenType::TOK_OFFSET_DATETIME:
            v.type = TomlType::OffsetDateTime;
            v.string_value = tok.lexeme;
            consume();
            break;

        case TomlTokenType::TOK_LBRACKET:
            return parse_array();

        case TomlTokenType::TOK_LBRACE:
            return parse_inline_table();

        case TomlTokenType::TOK_LDBRACKET:
            parse_ld_array();
            break;
        case TomlTokenType::TOK_NEWLINE:
            consume();
            break;
        default:
            print_error("Invalid value", peek());
        }

        return v;
    }

    void parse_key_value(TomlValue *table)
    {
        std::vector<std::string> keys = parse_key_path();

        if (!expect_token(TomlTokenType::TOK_EQUAL))
            print_error("Expected '=' after key", peek());

        consume();

        TomlValue v = parse_value();
        insert_value(table, keys, v);

        if (expect_token(TomlTokenType::TOK_NEWLINE))
            consume();
    }

    std::vector<std::string> parse_key_path()
    {
        std::vector<std::string> keys;
        keys.push_back(consume().lexeme);

        while (expect_token(TomlTokenType::TOK_DOT))
        {
            consume();
            if (!expect_token(TomlTokenType::TOK_IDENTIFIER))
            {
                print_error("Expected an identifier after the dot", peek());
            }

            keys.push_back(consume().lexeme);
        }

        return keys;
    }

    /**
     * Traverses to the end of the last key and inserts the value there
     * Note how the loop goes up to keys-size()-1, we intentionally leave out the last
     * key since that key is not a table but an identifier whose value is to be set.
     */

    void insert_value(TomlValue *table, std::vector<std::string> &keys, TomlValue value)
    {
        TomlValue *current_table = table;
        for (int i = 0; i < keys.size() - 1; i++)
        {
            auto &key = keys[i];
            if (!current_table->table_value.count(key))
            {
                current_table->table_value[key].type = TomlType::Table;
            }

            current_table = &current_table->table_value[key];

            if (current_table->type != TomlType::Table)
            {
                print_error("Key path conflicts with non-table", peek());
            }
        }

        current_table->table_value[keys.back()] = value;
    }

    void indent(int level)
    {
        for (int i = 0; i < level; i++)
            std::cout << "  ";
    }

    void print_value(const TomlValue &v, int level = 0)
    {
        switch (v.type)
        {
        case TomlType::String:
            std::cout << "\"" << v.string_value << "\"";
            break;

        case TomlType::Integer:
            std::cout << v.int_value;
            break;

        case TomlType::Float:
            std::cout << v.float_value;
            break;

        case TomlType::Boolean:
            std::cout << (v.bool_value ? "true" : "false");
            break;

        case TomlType::LocalDate:
            std::cout << v.string_value;
            break;

        case TomlType::LocalTime:
            std::cout << v.string_value;
            break;

        case TomlType::LocalDateTime:
            std::cout << v.string_value;
            break;

        case TomlType::OffsetDateTime:
            std::cout << v.string_value;
            break;

        case TomlType::Array:
            std::cout << "[\n";
            for (const auto &item : v.array_value)
            {
                indent(level + 1);
                print_value(item, level + 1);
                std::cout << ",\n";
            }
            indent(level);
            std::cout << "]";
            break;

        case TomlType::Table:
            std::cout << "{\n";
            for (const auto &[key, value] : v.table_value)
            {
                indent(level + 1);
                std::cout << key << " = ";
                print_value(value, level + 1);
                std::cout << "\n";
            }
            indent(level);
            std::cout << "}";
            break;
        }
    }

    void print_error(std::string message, TomlTokens token)
    {
        int col = token.col;
        int line = token.line;

        std::cout << "Error parsing token: " << message << std::endl;
        token.print();
        std::cout << "near line " << line << " column " << col << std::endl;

        exit(1);
    }

public:
    std::string file_name;
    std::vector<TomlTokens> tokens;
    int tokens_length;
    int index = 0;
    TomlDocument *toml_doc;

    TomlParser(std::string file_name, std::vector<TomlTokens> tokens)
    {
        this->file_name = file_name;
        this->tokens = tokens;
        this->tokens_length = tokens.size();
        this->index = 0;
        this->toml_doc = nullptr;
    }

    TomlDocument parse_toml()
    {
        TomlDocument doc;
        doc.root.type = TomlType::Table;

        TomlValue *current_table = &doc.root;

        while (not_end())
        {
            if (expect_token(TomlTokenType::TOK_NEWLINE))
            {
                consume();
                continue;
            }

            parse_statement(current_table);
        }

        print_document(doc);
        return doc;
    }

    void print_document(const TomlDocument &doc)
    {
        print_value(doc.root, 0);
        std::cout << "\n";
    }
};

#endif // C4C_TOML_PARSER_HPP
