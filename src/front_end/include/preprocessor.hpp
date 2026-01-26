/****************************************************************************************************\
 * FILE: Preprocessor.hpp                                                                           *
 * AUTHOR: Michael Kamau                                                                            *
 *                                                                                                  *
 * PURPOSE: The program removes comments and expands macros and include statements,                 *
 *          the output is then fed to the                                                           *
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

#define C4_SYSTEM_FOLDER "../../system_folder"
#define PATH_MAX 1024

class Preprocessor
{
private:
    const std::string &file_name;
    const std::string &file_source;
    std::unordered_map<std::string, std::string> defines; /* We use this map to track the defined macros */
    std::unordered_map<std::string, bool> included_files; /* We use this map to track the already included files */
    std::vector<std::string> track_circular_dependency;   /* This is used to check for circular dependency */
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


    /* This is a helper function to check if the next 9 characters equal `@include `*/
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


    /**
     * This helper function helps with checking whether the included filepath is
     * an absolute or a relative path, currently, it supports unix based paths format
     * forward slash (/) and not backslash (\)
     */
    static bool is_absolute_path(const char *path)
    {
        if (!path)
        {
            return false;
        }

        return path[0] == '/';
    }


    /**
     * This function, helps in determining the base directory of the 
     * source code, 
     * For example, if you are compiling a file in the directory
     * home/.../dev/tests/test.rs,
     * 
     * the dirname_of should return home/.../dev/tests as the base directory name
     * 
     */

     /********************************************************************************************\
      * NOTE: However, due to using recursion, this becomes a disadvantage because i try to find *
      * the base using the current file being processed, as a result the base will always be     *
      * based on the location of the file being currently processed byt the preprocessor.        *
     \********************************************************************************************/

    static std::string dirname_of(const std::string &path)
    {
        size_t pos = path.find_last_of("/\\");
        if (pos == std::string::npos)
        {
            DEBUG_PRINT("Dirname wasnt found","");
            return "."; /* current directory fallback */
        }
        DEBUG_PRINT("Dirname is: ", path.substr(0, pos));

        return path.substr(0, pos);
    }


    /**
     * This function builds the canonical path for the file being included
     * the idea is, after getting the full directory, and checking whether the file is an absolute path or not,
     * it returns the correct path that will lead to the file.
     * 
     * If the file is an absolute path, it is taken as is, because the file is absolute to the file importing it
     * If it is not absolute, we get the base directory and build the path correctly.
     * Remember, the base directory is built from the file being currently processed by the preprocessor
     * 
     * The reason for getting the canonical path is to use it to prevent circular dependencies and multiple imports,
     * a canonical path will always be the same! 
     */
    std::string get_canonical_path(std::string file_name, const char *path)
    {

        std::string possible_path;

        if (is_absolute_path(path))
        {
            possible_path = path;
        }
        else
        {

            std::string base = dirname_of(file_name);
            possible_path = base + "/" + path;
        }
        char res[PATH_MAX];

        DEBUG_PRINT("path this time: ", possible_path);
        if (!realpath(possible_path.c_str(), res))
        {
            perror("Real path failed");
            return NULL;
        }
        return std::string(res);
    }


    /**
     * This is a helper function to help figure out if a the path is already in the track_circular_dependancy vector
     * I try to use little C++ as possible so I didnt use std::find for this one.
     * 
     * The reason for using a linear search O(n) is because of such a case:
     * 
     * A -> B -> C -> A
     * 
     * If the above case should happen, where A,B,C are files, we see that C trys to include A which
     * would lead to a circular dependency
     */
    bool in_stack(const std::string &path)
    {
        DEBUG_PRINT("In here: ", "");
        for (size_t i = 0; i < track_circular_dependency.size(); i++)
        {
            DEBUG_PRINT("In here: ", track_circular_dependency[i]);

            if (track_circular_dependency[i] == path)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * This function is used to updat the row, the row should be nice during error handling 
     * for the preprocessor
     */

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

                std::string full_path = get_canonical_path(file_name, include_file.c_str());

                if (!track_circular_dependency.empty() && in_stack(full_path))
                {
                    DEBUG_PANIC("Detected circular dependency");
                }
                if (!included_files.count(full_path))
                {
                    track_circular_dependency.push_back(full_path);

                    FileToString fs(full_path);
                    std::string file_contents = fs.read();

                    
                    
                    /*********************************************************************************************************\
                     *                                                                                                       *
                     * NOTE: Having to reinstantiate the preprocessor recursively has made it a little                       *
                     * difficult to handle absolute and relative paths well.                                                 *
                     * In this approach, the first file that is being compiled say `test.rs`, and is including               *
                     * another file using its absolute path e.g include_test/test1.rs, will work                             *
                     * since during the first preprocessor.run(), the path build is say                                      *
                     * home/../dev/include_test/test1.rs, because the base will be                                           *
                     * home/.../dev, where test.rs is                                                                        *
                     *                                                                                                       *
                     * but inside test1.rs, should it try to import tests/test.rs, since its in the                          *
                     * recursed preprocessor, the base path becomes the new  file_name passed in the constructor             *
                     * e.g home/.../dev/include_test/ , building the path gives home/.../dev/include_test/tests/test.rs      *
                     * which wont be found.                                                                                  *
                     *                                                                                                       *
                     * As of the documenting of this program, I havent fixed this yet.                                       *
                     *                                                                                                       *
                     * I resorted to using recursion and sharing  parents states to keep ownership of                        *
                     * the track_circular_dependency strictly to preprocessor.hpp, the other approach                        *
                     * that I thought of at the time would have been to let driver.cpp own track_circular_dependency,        *
                     * which wouldnt be good for design nor scale.                                                           *
                     \********************************************************************************************************/

                     /* Share the parent state to avoid reprocessing already-included files */
                     
                     Preprocessor included_prep(full_path, file_contents);
                     included_prep.included_files = this->included_files;
                     included_prep.track_circular_dependency = this->track_circular_dependency;
                     
                    std::string processed_output = included_prep.run();

                    /* Update parent's internal state after recursion */ 
                    this->included_files = included_prep.included_files;
                    track_circular_dependency.pop_back();
                    included_files[full_path] = true;

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
