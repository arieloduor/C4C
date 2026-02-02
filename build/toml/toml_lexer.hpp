#ifndef C4C_TOML_LEXER_HPP
#define C4C_TOML_LEXER_HPP

#include <string>
#include <vector>
#include <stdint.h>
#include "toml_token.hpp"

enum DateState
{
    YEAR,
    MONTH,
    DAY,

    TIME_HOUR,
    TIME_MIN,
    TIME_SEC,

    FRACTION,

    OFFSET_HOUR,
    OFFSET_MIN,
};

class TomlLexer
{
    int col;
    bool has_errors;
    int line;
    int pos;
    int end;
    int token_start;
    std::string src;
    int length;
    int token_capacity;
    int token_count;
    std::vector<TomlTokens> tokens;

private:
    void make_decimal()
    {
        std::string buff;
        TomlTokenType tok_type = TomlTokenType::TOK_INTEGER;

        while (is_integer() or match_token('+') or match_token('-') or match_token('e') or match_token('E') or (match_token('_') and is_integer(1)))
        {
            if (match_token('e') or match_token('E'))
            {
                tok_type = TomlTokenType::TOK_FLOAT;
            }
            if (match_token('_'))
            {
                consume();
            }
            buff += consume();
        }

        if (match_token('.') and is_integer(1))
        {

            buff += consume();

            while (is_integer() or match_token('e') or match_token('E') or match_token('+') or match_token('-'))
            {
                buff += consume();
            }

            add_token(buff, TomlTokenType::TOK_FLOAT);
        }
        else
        {

            add_token(buff, tok_type);
        }

        update_col(buff.length());
    }

    inline void make_number()
    {
        make_decimal();
    }

    inline void make_binary_number()
    {
        std::string buff;

        buff += consume(); // consume the 0
        buff += consume(); // consume the b

        while (match_token('1') or match_token('0') or match_token('_'))
        {
            if (match_token('_'))
            {
                consume();
            }
            buff += consume();
        }

        add_token(buff, TomlTokenType::TOK_INTEGER);
    }

    inline void make_hex_number()
    {
        std::string buff;
        buff += consume(); // consume the 0
        buff += consume(); // consume the x

        while (is_hex() or is_integer() or match_token('_'))
        {
            if (match_token('_'))
            {
                consume();
            }
            buff += consume();
        }

        add_token(buff, TomlTokenType::TOK_INTEGER);
    }

    inline void make_octal_number()
    {
        std::string buff;
        buff += consume(); // consume 0
        buff += consume(); // consume o

        while (is_integer() or match_token('_'))
        {
            if (match_token('_'))
            {
                consume();
            }

            buff += consume();
        }

        add_token(buff, TomlTokenType::TOK_INTEGER);
    }

    bool looks_like_time()
    {
        return is_integer() && is_integer(1) && peek(2) == ':';
    }

    bool looks_like_date()
    {
        return is_integer() && is_integer(1) && is_integer(2) && is_integer(3) && peek(4) == '-';
    }

    inline bool looks_like_date_time()
    {

        return looks_like_date() || looks_like_time();
    }

    bool consume_digits(std::string &buff, int n)
    {
        for (int i = 0; i < n; i++)
        {
            if (!is_integer())
                return false;
            buff += consume();
        }
        return true;
    }

    inline void make_date()
    {

        std::string buff;
        bool seen_date = false;
        bool is_timeonly = false;

        DateState state;

        if (is_integer() and is_integer(1) and is_integer(2) and is_integer(3) and peek(4) == '-')
        {
            state = YEAR;
        }
        else if (is_integer() and is_integer(1) and peek(2) == ':')
        {
            state = TIME_HOUR;
            is_timeonly = true;
        }
        else
        {
            return;
        }

        while (true)
        {
            switch (state)
            {

            case YEAR:
                if (!consume_digits(buff, 4))
                    goto error;
                if (peek() != '-')
                    goto error;
                buff += consume();
                state = MONTH;
                break;

            case MONTH:
                if (!consume_digits(buff, 2))
                    goto error;
                if (peek() != '-')
                    goto error;
                buff += consume();
                state = DAY;
                break;

            case DAY:
                if (!consume_digits(buff, 2))
                    goto error;
                seen_date = true;

                if (peek() == 'T')
                {
                    buff += consume();
                    state = TIME_HOUR;
                }
                else if (!is_timeonly)
                {
                    add_token(buff, TomlTokenType::TOK_LOCAL_DATE);
                    return;
                }
                else
                {
                    goto error;
                }
                break;

            case TIME_HOUR:
                if (!consume_digits(buff, 2))
                    goto error;
                if (peek() != ':')
                    goto error;
                buff += consume();
                state = TIME_MIN;
                break;

            case TIME_MIN:
                if (!consume_digits(buff, 2))
                    goto error;
                if (peek() != ':')
                    goto error;
                buff += consume();
                state = TIME_SEC;
                break;

            case TIME_SEC:
                if (!consume_digits(buff, 2))
                    goto error;

                if (peek() == '.')
                {
                    buff += consume();
                    state = FRACTION;
                }
                else if (peek() == 'Z')
                {
                    buff += consume();
                    add_token(buff, TomlTokenType::TOK_OFFSET_DATETIME);
                    return;
                }
                else if (peek() == '+' || peek() == '-')
                {
                    buff += consume();
                    state = OFFSET_HOUR;
                }
                else if (seen_date)
                {
                    add_token(buff, TomlTokenType::TOK_LOCAL_DATETIME);
                    return;
                }
                else if (!seen_date)
                {
                    add_token(buff, TomlTokenType::TOK_LOCAL_TIME);
                    return;
                }
                break;

            case FRACTION:
                if (!is_integer())
                    goto error;
                while (is_integer())
                {
                    buff += consume();
                }

                if (peek() == 'Z')
                {
                    buff += consume();
                    add_token(buff, TomlTokenType::TOK_OFFSET_DATETIME);
                    return;
                }
                else if (peek() == '+' || peek() == '-')
                {
                    buff += consume();
                    state = OFFSET_HOUR;
                }
                else
                {
                    add_token(buff, TomlTokenType::TOK_LOCAL_TIME);
                    return;
                }
                break;

            case OFFSET_HOUR:
                if (!consume_digits(buff, 2))
                    goto error;
                if (peek() != ':')
                    goto error;
                buff += consume();
                state = OFFSET_MIN;
                break;

            case OFFSET_MIN:
                if (!consume_digits(buff, 2))
                    goto error;
                add_token(buff, TomlTokenType::TOK_OFFSET_DATETIME);
                return;
            }
        }
    error:
        has_errors = true;
        add_token(buff, TomlTokenType::TOK_ERROR);
        return;
    }

    inline void handle_escape(std::string &buff)
    {
        consume();
        char c = consume();
        switch (c)
        {
        case 'n':
            buff += '\n';
            break;
        case 't':
            buff += '\t';
            break;
        case 'r':
            buff += '\r';
            break;
        case 'b':
            buff += '\b';
            break;
        case 'f':
            buff += '\f';
            break;
        case '"':
            buff += '"';
            break;
        case '\\':
            buff += '\\';
            break;

        case 'u':
            parse_unicode(buff, 4);
            break;

        case 'U':
            parse_unicode(buff, 8);
            break;

        default:
            panic("Unknown escape character");
            break;
        }
    }

    void parse_unicode(std::string &buff, int digits)
    {
        uint32_t codepoint = 0;

        for (int i = 0; i < digits; i++)
        {
            char h = consume();
            if (!is_hex(h))
            {
                panic("Not a hex value");
            }
            codepoint = (codepoint << 4) | hex_value(h);
        }

        append_utf8(buff, codepoint);
    }

    inline uint8_t hex_value(char c)
    {
        if (c >= '0' and c <= '9')
            return c - '0';
        if (c >= 'a' and c <= 'f')
            return c - 'a' + 10;
        if (c >= 'A' and c <= 'F')
            return c - 'A' + 10;
        return 0;
    }

    void append_utf8(std::string &buff, uint32_t cp)
    {
        if (cp <= 0x7F)
        {
            buff.push_back((char)cp);
        }
        else if (cp <= 0x7FF)
        {
            buff.push_back((char)(0xC0 | (cp >> 6)));
            buff.push_back((char)(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0xFFFF)
        {
            buff.push_back((char)(0xE0 | (cp >> 12)));
            buff.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
            buff.push_back((char)(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0x10FFFF)
        {
            buff.push_back((char)(0xF0 | (cp >> 18)));
            buff.push_back((char)(0x80 | ((cp >> 12) & 0x3F)));
            buff.push_back((char)(0x80 | ((cp >> 6) & 0x3F)));
            buff.push_back((char)(0x80 | (cp & 0x3F)));
        }
        else
        {
            panic("invalid Unicode code point");
        }
    }

    inline void add_keyword(std::string buff)
    {
        if (buff == "true" or buff == "false")
        {
            add_token(buff, TomlTokenType::TOK_BOOLEAN);
        }
        else if (buff == "nan" or buff == "+nan" or buff == "-nan")
        {
            add_token(buff, TomlTokenType::TOK_NAN);
        }
        else if (buff == "inf" or buff == "+inf" or buff == "-inf")
        {
            add_token(buff, TomlTokenType::TOK_INF);
        }
        else
        {
            add_token(buff, TomlTokenType::TOK_IDENTIFIER);
        }
    }

    inline void make_identifier()
    {
        std::string buff;
        while (is_alphanumeric() or match_token('_') or match_token('-') or match_token('+'))
        {
            buff += consume();
        }

        update_col(buff.length());
        add_keyword(buff);
    }

    inline void make_multiline_string()
    {
        char quote;
        if (this->src[this->pos] == '\'')
        {
            quote = '\'';
        }
        else
        {
            quote = '\"';
        }
        consume();
        consume();
        consume();
        bool error = false;
        std::string buff;

        while (!(match_token(quote) and match_token(quote, 1) and match_token(quote, 2) and match_token('\n', 3)))
        {
            if (is_end())
            {
                error = true;
                break;
            }

            if (match_token('\\') and match_token('n', 1))
            {
                consume();
                consume();
                update_row(1);
                buff += ' ';
            }
            if (match_token('\\'))
            {
                while (not is_end() and not is_alphanumeric())
                {
                    consume();
                }
            }

            buff += consume();
        }

        if (error == true)
        {
            add_token(buff, TomlTokenType::TOK_ERROR);
            this->has_errors = true;
        }
        else
        {
            add_token(buff, TomlTokenType::TOK_STRING);
        }

        consume();
        consume();
        consume();
        update_col(buff.length() + 2);
    }

    inline void make_string()
    {
        char quote;
        if (this->src[this->pos] == '\'')
        {
            quote = '\'';
        }
        else
        {
            quote = '\"';
        }

        consume();
        bool error = false;
        std::string buff;

        while (not match_token(quote))
        {
            if (is_end())
            {
                error = true;
                break;
            }

            if (match_token('\\') and match_token('n', 1))
            {
                consume();
                consume();
                buff += ' ';
            }
            if (match_token('\\'))
            {
                handle_escape(buff);
            }
            else
            {
                buff += consume();
            }
        }

        consume();
        while (match_token(' ') and not match_token('\n'))
        {
            consume();
        }

        if (error == true)
        {
            add_token(buff, TomlTokenType::TOK_ERROR);
            this->has_errors = true;
        }
        else if (peek() == '=')
        {
            add_token(buff, TomlTokenType::TOK_IDENTIFIER);
        }
        else
        {
            add_token(buff, TomlTokenType::TOK_STRING);
        }

        update_col(buff.length() + 2);
    }

    inline bool is_end()
    {
        int curr = this->pos;
        return curr >= this->length;
    }

    inline void update_row(size_t row_dx)
    {
        this->line += row_dx;
        this->col = 0;
    }

    inline void update_col(size_t len)
    {
        this->col += len;
        this->token_start = this->pos;
        this->end = this->pos;
    }

    inline char consume()
    {
        return this->src[this->pos++];
    }

    inline char peek(size_t lookahead = 0)
    {
        if (this->pos + (int)lookahead >= this->length)
        {
            return '\0';
        }

        return this->src[this->pos + lookahead];
    }

    inline bool is_hex(size_t lookahead = 0)
    {
        int curr_pos = this->pos;
        int idx = curr_pos + lookahead;

        if (idx >= length)
        {
            return false;
        }

        return (this->src[idx] >= 'a' and this->src[idx] <= 'f') or (this->src[idx] >= 'A' and this->src[idx] <= 'F');
    }

    inline bool is_char_hex(char tok)
    {

        return (tok >= 'a' and tok <= 'f') or (tok >= 'A' and tok <= 'F');
    }

    inline bool is_integer(size_t lookahead = 0)
    {
        int curr_pos = this->pos;
        int idx = curr_pos + lookahead;

        if (idx >= this->length)
            return false;
        return (this->src[idx] >= '0' and this->src[idx] <= '9');
    }

    inline bool is_aslphabet(size_t lookahead = 0)
    {
        int curr_pos = this->pos;
        int idx = curr_pos + lookahead;
        if (idx >= this->length)
            return false;

        return (this->src[idx] >= 'a' and this->src[idx] <= 'z') or (this->src[idx] >= 'A' and this->src[idx] <= 'Z');
    }

    inline bool is_alphanumeric(size_t lookahead = 0)
    {
        return is_integer(lookahead) or is_aslphabet(lookahead);
    }

    inline bool match_token(char tok, size_t lookahead = 0)
    {
        int curr_pos = this->pos;
        int idx = curr_pos + lookahead;

        return this->src[idx] == tok;
    }

    inline void add_token(const std::string lexem, TomlTokenType type)
    {
        TomlTokens token(type, lexem, this->token_start, this->col, this->col, this->line);
        this->tokens.push_back(token);
    }

    inline void add_token_single(TomlTokenType type, std::string string = "")
    {
        consume();
        add_token(string, type);
        update_col(1);
    }

    inline void add_token_double(TomlTokenType type, std::string string = "")
    {
        consume();
        consume();
        add_token(string, type);
        update_col(2);
    }

    inline void skip_comment()
    {
        size_t skipped = 0;

        while (not is_end() and peek() != '\n')
        {
            consume();
            skipped++;
        }

        update_col(skipped);
        update_row(1);
    }

    inline void panic(std::string msg)
    {
        printf("An error occured %s", msg.c_str());
        exit(1);
    }

public:
    inline std::vector<TomlTokens> scan_tokens()
    {
        while (not is_end())
        {
            this->token_start = this->pos;
            scan_token();
        }

        add_token("eof", TomlTokenType::TOK_EOF);
        return this->tokens;
    }

    TomlLexer(std::string file_source)
    {
        this->col = 0;
        this->has_errors = false;
        this->line = 0;
        this->pos = 0;
        this->token_start = 0;
        this->end = 0;
        this->src = file_source;
        this->length = file_source.length();
        this->token_capacity = 0;
        this->token_count = 0;
    }

    inline void scan_token()
    {
        char token = peek();

        switch (token)
        {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            make_identifier();
            break;
        case '0':
            if (peek(1) == 'x')
            {
                make_hex_number();
            }
            else if (peek(1) == 'o')
            {
                make_octal_number();
            }
            else if (peek(1) == 'b')
            {
                make_binary_number();
            }
            else if (looks_like_date_time())
            {
                make_date();
            }
            else
            {
                make_number();
            }
            break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            if (looks_like_date_time())
            {
                make_date();
            }
            else
            {
                make_number();
            }
            break;
        case '+':
        case '-':
            if (is_integer(1))
            {
                make_number();
            }
            else if (is_aslphabet(1))
            {
                make_identifier();
            }
            break;
        case '\"':
        case '\'':
            if ((peek() == '\"' and peek(1) == '\"') or ((peek() == '\'' and peek(1) == '\'')))
            {
                make_multiline_string();
            }
            else
            {
                make_string();
            }
            break;
        case '[':
            if (match_token('[', 1))
            {
                add_token_double(TomlTokenType::TOK_LDBRACKET, "[[");
            }
            else
            {
                add_token_single(TomlTokenType::TOK_LBRACKET, "[");
            }
            break;
        case ']':
            if (match_token(']', 1))
            {
                add_token_double(TomlTokenType::TOK_RDBRACKET, "]]");
            }
            else
            {
                add_token_single(TomlTokenType::TOK_RBRACKET, "]");
            }
            break;
        case '{':
            add_token_single(TomlTokenType::TOK_LBRACE, "{");
            break;
        case '}':
            add_token_single(TomlTokenType::TOK_RBRACE, "}");
            break;
        case ',':
            add_token_single(TomlTokenType::TOK_COMMA, ",");
            break;
        case '\t':
            update_col(4);
            consume();
            break;
        case ' ':
            update_col(1);
            consume();
            break;
        case '\r':
            update_col(1);
            consume();
            break;
        case '\n':
            add_token_single(TomlTokenType::TOK_NEWLINE, "\\n");
            update_row(1);
            update_col(1);
            break;
        case '.':
            add_token_single(TomlTokenType::TOK_DOT, ".");
            break;
        case '=':
            add_token_single(TomlTokenType::TOK_EQUAL, "=");
            break;
        case '#':
            skip_comment();
            break;
        default:
            consume();
            this->has_errors = true;
            break;
        }
    }
};

#endif // C4C_TOML_PARSER_HPP
