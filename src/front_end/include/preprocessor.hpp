/****************************************************************************************************\
 * FILE: Preprocessor.hpp                                                                           *
 * AUTHOR: Michael Kamau                                                                            *
 *                                                                                                  *
 * PURPOSE: The program removes comments and expands macros, the output is then fed to the          *
 *          lexer to continue with the lexical analysis                                             *
 *                                                                                                  *
 *  USAGE: To instantiate the program call its constructor and pass two arguments:                  *
 *                -file_name: the name of the file being preprocessed                               *
 *                -file_source: the contents of the file                                            *
 *                                                                                                  *
 *                 `Preprocessor p(file_name, file_source);`                                        *
 *                                                                                                  *
 *                  to run the program,                                                             *
 *                  `p.run()`                                                                       *
 *                                                                                                  *
 \***************************************************************************************************/

#ifndef C4C_PREPROCESSOR_H
#define C4C_PREPROCESSOR_H

#include "../../utils/include/utils.hpp"

class Preprocessor
{
private:
    const std::string &file_name;
    const std::string &file_source;
    std::unordered_map<std::string, std::string> defines; /* We use this map to track the defined macros */
    std::unordered_map<std::string, bool> included_files; /* We use this map to track the already included files */
    std::vector<std::string> track_circular_dependency;
    bool has_errors;
    std::vector<std::string> errors; /* To store the errors encountered*/
    int row;

    /* Preprocessor internal states */

    enum class State
    {
        NORMAL,       /* Just normal characters*/
        LINECOMMENT,  /* // */
        HASHCOMMENT,  /* # */
        BLOCKCOMMENT, /* /*....*/
        DEFINE,       /* @define */
        INCLUDE       /* @include */
    };

public:
    Preprocessor(std::string &filename, std::string &source)
        : file_name(filename), file_source(source)
    {
        this->has_errors = false;
        this->row = 0;
    }

    /*>>>>>>>>>>>> Helper functions <<<<<<<<<<<<<<<<*/
    /**
     * `is_inline_comment(size_t curr)` -> this function takes the current position of the index
     * as parameter and uses to check whether the next character is a forward slash, which would indicate
     * that the line is a comment.
     *
     * `is_block_comment(size_t curr)` -> this function takes the current position of the index
     * as parameter and uses it to check whether the next character is an asterisk (*)
     * inidicating the beginning of a block comment.
     *
     * `is_block_comment_closed(size_t curr)` -> takes in the current position of the index
     * as parameter and uses it to check whether the current character is an asterisk(*) and whether
     * the next is a forward slash. If these two conditions are met, then that means its the closing of
     * a block comment.
     *
     *
     * `is_define(size_t curr)` -> this funtion takes in the current position of the index
     * as parameter and uses it to check whether the next 8 characters equal to the word
     * "@define ", take note of the space after define, if they are equal it returns true,
     * meaning its the begining of a macro definition.
     *
     * `update_row()` -> this increaments the value of row, row is basically the number of lines
     * in the file.
     *
     * `print_errors` -> it does as the name suggests, it prints out the errors encounterd in the
     * preprocessor.
     */

    inline bool is_inline_comment(size_t curr) const
    {
        return curr + 1 < file_source.length() && file_source[curr + 1] == '/';
    }

    inline bool is_block_comment(size_t curr) const
    {
        return curr + 1 < file_source.length() && file_source[curr + 1] == '*';
    }

    inline bool is_block_comment_closed(size_t curr) const
    {
        return curr + 1 < file_source.length() &&
               file_source[curr] == '*' &&
               file_source[curr + 1] == '/';
    }

    inline bool is_define(size_t curr) const
    {
        if (curr + 8 >= file_source.length())
            return false; /* avoid overflow */

        char buff[9];
        for (size_t j = 0; j < 8; j++)
        {
            buff[j] = file_source[curr + j];
        }
        buff[9] = '\0';

        return strcmp(buff, "@define ") == 0;
    }

    inline bool is_include(size_t curr) const
    {
        if (curr + 9 >= file_source.length())
        {
            return false; /* avoid overflow */
        }

        char buff[10];
        for (size_t j = 0; j < 9; j++)
        {
            buff[j] = file_source[curr + j];
        }
        buff[9] = '\0';
        return strcmp(buff, "@include ") == 0;
    }

    static std::string dirname_of(const std::string &path)
    {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos)
            return "."; /* current directory fallback */
        return path.substr(0, pos);
    }

    char *get_canonical_path(std::string file_name, const char *path)
    {
        std::string base = dirname_of(file_name);
        std::string combined = base + "/" + path;
        char *res = realpath(combined.c_str(), NULL);

        if (!res)
        {
            perror("Real path failed");
            return NULL;
        }
        return res;
    }

    void updateRow()
    {
        this->row++;
    }

    void print_errors()
    {
        if (this->has_errors)
        {
            for (std::string error : errors)
            {
                DEBUG_PRINT("", error);
            }
        }
    }
    /*>>>>>>>>>>>> Major processing function <<<<<<<<<<<<<<<<*/
    /**
     *
     * This is the main function, it scans through the source (contents of the file)
     * to look for comments and macro definitions.
     * It uses the 5 states of the `State` enum:
     *
     * NORMAL       : just a normal character
     * LINECOMMENT  : a line comment (//)
     * HASHCOMMENT  : a hash comment (#)
     * BLOCKCOMMENT : a block comment (/** */
    /*)
     * DEFINE       : a "@define" declaration
     *
     * The function uses a switch to detect states;
     * -if the current character is a forward slash (/) and the current state is NORMAL,
     *  we use the helper functions `is_inline_comment()` and `is_block_comment()` to check
     *  whether this is the begining of a comment
     *
     * -if the current state is LINECOMMENT or BLOCKCOMMENT,
     *  all the characters are ignored until a new line character is reached for LINECOMMENT's
     *  case or a closing block comment is found for BLOCKCOMMENT's case.
     *
     * -if the current character is '@' and the current state is NORMAL,
     *  we use the `is_define()` helper function to check whether it is the beginning of a macro
     *
     * -if the current state is DEFINE, we detect the 'name' of the macro and the 'value'
     *  and store them in the `defines` map.
     *
     * The function start at the NORMAL state and checks for the above conditions, if the current character
     * is not a character that could lead to a comment (/, #), or a macro definition (@),
     * we handle the macro substitution if there were any defined.
     * The core idea is, if a word matches the key to the `defines` map, it is replaced by the value of the key
     *
     */
    std::string run()
    {
        std::string output;
        output.reserve(file_source.size());

        State state = State::NORMAL;
        size_t len = file_source.length();

        for (size_t i = 0; i < len; i++)
        {
            char c = file_source[i];

            switch (state)
            {
            case State::NORMAL:
            {

                if (c == '@' && is_define(i))
                {
                    state = State::DEFINE;
                    i += 7; /* we add 7 to skip "@define" */
                }
                else if (c == '@' && is_include(i))
                {
                    state = State::INCLUDE;
                    i += 8; /* we add 8 to skip "@include" */
                }
                else if (c == '/' && is_inline_comment(i))
                {
                    state = State::LINECOMMENT;
                }
                else if (c == '#')
                {
                    state = State::HASHCOMMENT;
                }
                else if (c == '/' && is_block_comment(i))
                {
                    state = State::BLOCKCOMMENT;
                }
                else if (c == '\n')
                {
                    output += '\n';
                    updateRow();
                }
                else
                {
                    /* Handle normal code and macro substitution */

                    if (isalnum(c))
                    {
                        std::string word;
                        while (i < len && isalnum(file_source[i]))
                        {
                            word += file_source[i++];
                        }
                        i--; /* adjust because for loop will increment */

                        /* Substitute macros if defined */

                        if (defines.count(word))
                        {
                            output += defines[word];
                        }
                        else
                        {
                            output += word;
                        }
                    }
                    else
                    {
                        output += c;
                    }
                }
            }
            break;

            case State::LINECOMMENT:
            case State::HASHCOMMENT:
                while (i < len && file_source[i] != '\n')
                    i++;
                output += "\n"; /* We add this because the loop consumes the new line character at the end*/
                updateRow();    /* We update because of the new line added above*/
                state = State::NORMAL;
                break;

            case State::DEFINE:
            {
                /* Skip any leading whitespace */
                while (i < len && isspace(file_source[i]))
                {
                    i++;
                }

                /* Read macro name */
                std::string macro;
                while (i < len && !isspace(file_source[i]))
                {
                    macro += file_source[i++];
                }

                /* Skip whitespace before value */
                while (i < len && isspace(file_source[i]))
                {
                    i++;
                }

                /* Read macro value until newline */
                std::string value;
                while (i < len && file_source[i] != '\n')
                {
                    value += file_source[i++];
                }

                defines[macro] = value;

                state = State::NORMAL;
                updateRow();
            }
            break;

            case State::INCLUDE:
            {
                /* We skip any preciding spaces*/
                while (i < len && isspace(file_source[i]))
                {
                    i++;
                }

                std::string include_file;
                if (file_source[i] == '"')
                {
                    i++;
                }
                else
                {
                    this->has_errors = true;
                    errors.push_back("Expected \" but got none");
                }

                while (i < len && file_source[i] != '"')
                {
                    include_file += file_source[i++];
                }

                if (i == len)
                {
                    has_errors = true;
                    errors.push_back("Expected closing \" but found none");
                }

                char *full_path = get_canonical_path(file_name, include_file.c_str());
                DEBUG_PRINT("Checking for: ", include_file);

                DEBUG_PRINT("The path is: ", full_path);
                FileToString fs(full_path);
                std::string file_contents = fs.read();

                if (!included_files.count(full_path))
                {
                    included_files[full_path] = true;
                    std::string real_path = full_path;

                    Preprocessor p(real_path, file_contents);
                    std::string processed_output = p.run();

                    output += processed_output;
                    output += "\n";
                    updateRow();
                }
                else
                {
                    DEBUG_PRINT("Handled multiple imports gracefully for file: ", full_path);
                }

                state = State::NORMAL;
                DEBUG_PRINT("File to be included is ", include_file);
            }
            break;
            case State::BLOCKCOMMENT:
                while (i < len - 1)
                {
                    if (c == '\n')
                        updateRow();
                    if (is_block_comment_closed(i))
                    {
                        i += 1; /* skip the closing '/' */
                        state = State::NORMAL;
                        break;
                    }
                    else
                        i++;
                }

                /* If the state is normal means a closing block comment was not found*/
                if (state == State::BLOCKCOMMENT)
                {
                    this->has_errors = true;
                    this->errors.push_back("No closing */ for block comment was found, please add (*/)");
                }
                break;

            default:
                break;
            }
        }

        return output;
    }
};

#endif // C4C_PREPROCESSOR_H
