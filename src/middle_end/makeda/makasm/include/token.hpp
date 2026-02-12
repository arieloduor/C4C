#ifndef C4_MAKASM_LEXER_H
#define C4_MAKASM_LEXER_H




#ifndef C4C_TOKENS_H
#define C4C_TOKENS_H

#include "../../../../utils/include/utils.hpp"



enum class TokenType
{

//   ARITHMETIC OPERATORS

    TOKEN_EXP = 0,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_ADD,
    TOKEN_SUB,
/*

//  COMPARISON OPERATORS

    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS_EQUAL,

//  EQUALITY OPERATORS

    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

//  BITWISE OPERATORS

    TOKEN_BITWISE_AND,
    TOKEN_BITWISE_OR,
    TOKEN_BITWISE_XOR,
    TOKEN_BITWISE_NOT,
    TOKEN_LEFT_SHIFT,
    TOKEN_RIGHT_SHIFT,

//  LOGICAL OPERATORS

    TOKEN_AND,
    TOKEN_OR ,
    TOKEN_NOT,

*/

    TOKEN_COMMA,


//  LITERALS


    TOKEN_LITERAL_INT,
    TOKEN_LITERAL_FLOAT,
    TOKEN_LITERAL_STRING,

//  KEYWORDS

    TOKEN_PUSH,
    TOKEN_POP,
    TOKEN_PRINT,

    TOKEN_JMP,

    TOKEN_L,
    TOKEN_LE,
    TOKEN_G,
    TOKEN_GE,
    TOKEN_EQ,
    TOKEN_NE,

    TOKEN_JT,
    TOKEN_JF,
    
    TOKEN_EOF,

};



class Tokens
{
public:
	TokenType type;
	std::string string;
	int start;
	int end;
	int row;
	int col;
	int line;

public:
	Tokens(TokenType type,std::string string,int start,int end,int row,int col,int line = 0)
	{
		this->type = type;
		this->string = string;
		this->start = start;
		this->end = end;
		this->line = line;
		this->row = row;
		this->col = col;
	}

};

#endif






#endif