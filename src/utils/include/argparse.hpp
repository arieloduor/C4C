#ifndef C4_ARGPARSE_H
#define C4_ARGPARSE_H

#include "utils.hpp"
#include <stdlib.h>

enum class ArgumentType
{
    FLAG,
    CHAR,
    I8,
    I16,
    I32,
    I64,
    U8,
    U16,
    U32,
    U64,
    F32,
    F64,
    STRING,
};


class Argument
{
public:
    std::string short_name;
    std::string long_name;
    std::string description;
    std::string help;
    ArgumentType type;
    bool flag = false;
    bool is_active = false;
    
    // unions are a pain in c++

    std::string value_string;
    i64 value_i64;



    Argument(std::string short_name,std::string long_name,std::string description,std::string help,ArgumentType type)
    {
        this->short_name = short_name;
        this->long_name = long_name;
        this->description = description;
        this->help = help;
        this->type = type;
    }
    
};


// SHOULD USE A HASHMAP BUT WHAT THE HELL(I HATE C++ ANYWAY)


class Argparser
{
public:
    std::string name;
    std::string description;
    std::string help;
    std::string version;
    std::vector<Argument> arguments;

    Argparser(std::string name,std::string description,std::string help,std::string version)
    {
        this->name = name;
        this->description = description;
        this->help = help;
        this->version = version;
    }


    std::string get_value_string(std::string long_name)
    {
        for(Argument argument : this->arguments)
        {
            if(argument.long_name == long_name)
            {
                return argument.value_string;
            }
        }

        return "";
    }


    std::string get_description(std::string long_name)
    {
        for(Argument argument : this->arguments)
        {
            if(argument.long_name == long_name)
            {
                return argument.description;
            }
        }

        return "";
    }


    std::string get_help(std::string long_name)
    {
        for(Argument argument : this->arguments)
        {
            if(argument.long_name == long_name)
            {
                return argument.help;
            }
        }

        return "ERROR : NO HELP";
    }


    bool get_flag(std::string long_name)
    {
        for(Argument argument : this->arguments)
        {
            if(argument.long_name == long_name)
            {
                return argument.flag;
            }
        }

        return false;
    }


    i64 get_value_i64(std::string long_name)
    {
        for(Argument argument : this->arguments)
        {
            if(argument.long_name == long_name)
            {
                return argument.value_i64;
            }
        }

        return -1;
    }


    void parse(int argc,char **argv)
    {
        for(int i = 1; i < argc; i++)
        {
            char *arg = argv[i];
            char *name;


            if( arg[0] == '-')
            {
                if(arg[1] == '-')
                {
                    arg += 2;
                    name = arg;

                    for( int j = 0; j < this->arguments.size(); j++)
                    {
                        Argument argument = this->arguments[j];
                    
                        if(argument.long_name == name)
                        {
                            switch(argument.type)
                            {
                                case ArgumentType::STRING:
                                {
                                    std::string value_string(argv[++i]);
                                    this->arguments[j].value_string = value_string;
                                    this->arguments[j].is_active = true;
                                    break;
                                }
                                case ArgumentType::FLAG:
                                {
                                    this->arguments[i].flag = true;
                                    this->arguments[j].is_active = true;
                                    break;
                                }
                                default:
                                {
                                    fatal("unsupported argument type");
                                }
                            }
                        }
                    }
                }
                else
                {
                    arg += 1;
                    name = arg;

                    for( int j = 0; j < this->arguments.size(); j++)
                    {
                        Argument argument = this->arguments[j];
                    
                        if(argument.short_name == name)
                        {
                            switch(argument.type)
                            {
                                case ArgumentType::STRING:
                                {
                                    std::string value_string(argv[++i]);
                                    this->arguments[j].value_string = value_string;
                                    this->arguments[j].is_active = true;
                                    break;
                                }
                                case ArgumentType::FLAG:
                                {
                                    this->arguments[i].flag = true;
                                    this->arguments[j].is_active = true;
                                    break;
                                }
                                default:
                                {
                                    fatal("unsupported argument type");
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    void fatal(std::string str)
    {
        std::cout << str <<std::endl;
    }


    void add_argument(Argument argument)
    {
        this->arguments.push_back(argument);
    }


};




#endif