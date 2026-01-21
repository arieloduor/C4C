#ifndef C4C_PREPROCESSOR_H
#define C4C_PREPROCESSOR_H

#include "../../utils/include/utils.hpp"

class Preprocessor
{
private:
    const std::string &file_name;
    const std::string &file_source;
    std::unordered_map<std::string, std::string> defines;

    /* Preprocessor internal states */

    enum class State
    {
        NORMAL,
        LINECOMMENT,
        HASHCOMMENT,
        BLOCKCOMMENT,
        DEFINE
    };

public:
    Preprocessor(std::string &filename, std::string &source)
        : file_name(filename), file_source(source)
    {
    }



    /*>>>>>>>>>>>> Helper functions <<<<<<<<<<<<<<<<*/

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
        buff[8] = '\0';

        return strcmp(buff, "@define ") == 0;
    }



    /*>>>>>>>>>>>> Major processing function <<<<<<<<<<<<<<<<*/

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
                    i += 7; /* skip "@define" */ 
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
                        i--; /* adjust because for-loop will increment */

                        /* Substitute macros if defined */
                        if (defines.count(word))
                            output += defines[word];
                        else
                            output += word;
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
                state = State::NORMAL;
                break;


            case State::DEFINE:
            {
                /* Skip any leading whitespace */
                while (i < len && isspace(file_source[i]))
                    i++;

                /* Read macro name */
                std::string macro;
                while (i < len && !isspace(file_source[i]))
                    macro += file_source[i++];

                /* Skip whitespace before value */
                while (i < len && isspace(file_source[i]))
                    i++;

                /* Read macro value until newline */
                std::string value;
                while (i < len && file_source[i] != '\n')
                    value += file_source[i++];

                defines[macro] = value;

                state = State::NORMAL;
            }
            break;

            
            case State::BLOCKCOMMENT:
                while (i < len - 1)
                {
                    if (is_block_comment_closed(i))
                    {
                        i += 1; /* skip the closing '/' */
                        state = State::NORMAL;
                        break;
                    }
                    else
                        i++;
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
