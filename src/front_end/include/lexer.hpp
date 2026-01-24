/****************************************************************************************************\
 * FILE: lexer.hpp 		                                                                            *
 * AUTHOR: Ariel Oduor                                                                              *
 *                                                                                                  *
 * PURPOSE: The program scans through the source and builds tokens from the characters,             *
 *          tokens produced by this program are fed into the parser for further actions.             *
 *                                                                                                  *
 *  USAGE: To instantiate the program call its constructor and pass two arguments:                  *
 *                -file_name: the name of the file being preprocessed                               *
 *                -file_source: the contents of the file                                            *
 *                                                                                                  *
 *                 `Lexer s(file_name, file_source);`   	                                        *
 *                                                                                                  *
 *                  to run the program,                                                             *
 *                  `l.scan_tokens                                                                  *
 *                                                                                                  *
 \***************************************************************************************************/



#ifndef C4C_LEXER_H
#define C4C_LEXER_H

#include "tokens.hpp"

class Lexer
{
	std::string file_name;
	std::string file_source;
	int file_length;
	int index;
	int current;
	std::vector<Tokens> tokens;
	bool has_errors;
	int start;
	int end;
	int line;
	int row;
	int col;

public:
	/**
	 * Initializes the lexer with the source code to be tokenized.
	 *	It takes two parameters:
	 *  -file_name ->  Name of the source file (used for diagnostics).
	 *  -file_source -> Full contents of the source file.
	 *
	 * Sets up internal cursor positions, line/column tracking,
	 * and prepares the lexer for scanning.
	 */

	Lexer(std::string file_name,std::string file_source)
	{
		this->file_name = file_name;
		this->file_source = file_source;
		this->file_length = this->file_source.length();
		this->index = 0;
		this->current = 0;
		this->has_errors = false;
		this->start = 0;
		this->end = 0;
		this->line = 0;
		this->row = 0;
		this->col = 0;
	}
private:

	/**
	 * Checks whether the lexer has reached the end of the source input.
	 *
	 * it returns true if the current index is at or beyond the file length,
	 * false otherwise.
	 */

	inline bool at_end()
	{
		if (this->index >= this->file_length)
		{
			return true;
		}

		return false;
	}

	/**
	 * Peeks at the current character or a future character without consuming it.
	 *
	 * It takes one parameter:
	 * -lookahead -> Number of characters ahead of the current index to inspect, if not provided a default of 0 is used.
	 * 
	 * The function returns the character at the given lookahead position, or '\0' if out of bounds.
	 *
	 * Used to make decisions about multi-character tokens and lookahead logic.
	 */

	inline char peek(int lookahead = 0)
	{
		if ( (this->index + lookahead) >= this->file_length)
		{
			return '\0';
		}

		return this->file_source[this->index + lookahead];
	}

	/**
	 * Consumes the current character and advances the lexer cursor.
	 *
	 * The function returns The consumed character.
	 *
	 * This is the primary method used to advance through the source input.
	 */

	inline char consume()
	{
		return this->file_source[this->index++];
	}

	/**
	 * Checks whether a specific character matches the current or lookahead character.
	 *
	 * The function takes two parameters:
	 * -token   	-> The character to match.
	 * -lookahead 	-> Number of characters ahead to check, if not provided a default of 0 is used.
	 * 
	 * and returns true if the character matches, false otherwise.
	 */

	inline bool is_token(char token,int lookahead = 0)
	{
		if (this->peek(lookahead) == token)
		{
			return true;
		}

		return false;
	}

	/**
	 * Alias for is_token(), used for semantic clarity when matching tokens.
	 *
	 * The function takes two parameters:
	 * -token     -> The character to match.
	 * -lookahead -> Number of characters ahead to check, if none is given, a default of 0 is used.
	 * 
	 * it returns true if the character matches, false otherwise.
	 */

	inline bool match_token(char token,int lookahead = 0)
	{
		return is_token(token,lookahead);
	}

	/**
	 * Ensures that the next character matches an expected token.
	 *
	 * Intended for enforcing grammar rules and reporting errors
	 * when an expected token is not found.
	 */

	 /***********************************\
	 * NOTE: Currently not implemented. *
	 \***********************************/

	inline void expect_token(char token)
	{
		// not implemented
	}


	/**
	 * Checks whether the current or lookahead character is a digit.
	 *
	 * The function takes one parameter:
	 * -lookahead -> Number of characters ahead to inspect, if none is given, a default of 0 is used.
	 * 
	 * it returns true if the character is between '0' and '9', false otherwise.
	 */

	inline bool is_digit(int lookahead = 0)
	{
		char token = peek(lookahead);
		if ( (token >= '0') and (token <= '9'))
		{
			return true;
		}

		return false;
	}

	/**
	 * Lexes an integer or floating point numeric literal.
	 *
	 * Consumes consecutive digits and optionally parses a fractional
	 * component if a decimal point is followed by digits.
	 *
	 * Emits either an integer or float literal token.
	 */

	inline void make_decimal()
	{
		std::string buf;

		while ( is_digit())
		{
			buf += consume();
		}

		if ( match_token('.') and is_digit(1) )
		{
			buf += consume();
			while (is_digit())
			{
				buf += consume();
			}

			/* To check for cases such as 12.2.2 */
			if( match_token('.') and is_digit(1) )
			{
				if(!this->has_errors)
					this->has_errors = true;
				
				add_token(TokenType::TOKEN_ERROR_MALFORMED_NUMBER);
			}
			else
				add_token(TokenType::TOKEN_LITERAL_FLOAT,buf);
		}
		else
		{
			add_token(TokenType::TOKEN_LITERAL_INT,buf);
		}

		update_col(buf.length());
	}

	/**
	 * Entry point for lexing numeric literals.
	 *
	 * It forwards directly to make_decimal()
	 */

	inline void make_number()
	{
		make_decimal();
	}

	/**
	 * Checks whether the current character is a valid alphabetic identifier character.
	 *
	 * Accepts uppercase letters, lowercase letters, and underscores.
	 *
	 * The function returns true if the character is alphabetic or '_', false otherwise.
	 */

	inline bool is_alpha()
	{
		char token = peek(0);
		if ( (token >= 'A' and token <= 'Z') or (token >= 'a' and token <= 'z') or (token == '_') )
		{
			return true;
		}

		return false;
	}

	/**
	 * Checks whether the current character is alphanumeric.
	 *
	 * Accepts digits, letters, and underscores.
	 *
	 * The function returns true if the character is alphanumeric, false otherwise.
	 */

	inline bool is_alphanum()
	{
		return is_digit() or is_alpha();
	}

	/**
	 * Lexes a string literal enclosed in double quotes.
	 *
	 * Consumes characters until a closing quote is found.
	 * If the end of file is reached before termination,
	 * an unterminated string error token is generated.
	 */

	inline void make_string()
	{
		bool error = false;
		std::string buf;
		consume();

		while ( not match_token('\"'))
		{
			if (at_end())
			{
				error = true;
				break;
			}

			buf += consume();
		}

		if (error == true)
		{
			add_token(TokenType::TOKEN_ERROR_UNTERMINATED_STRING,buf);
			this->has_errors = true;
		}
		else
		{
			consume();
			add_token(TokenType::TOKEN_LITERAL_STRING,buf);
		}

		update_col(buf.length() +  2);
	}


	/**
	 * Lexes an identifier or keyword.
	 *
	 * Consumes alphanumeric characters and underscores,
	 * then determines whether the resulting lexeme matches
	 * a reserved keyword or should be treated as an identifier.
	 */

	inline void make_identifier()
	{
		//DEBUG_PRINT("make identifier ","found");
		std::string buf;

		while (is_alphanum())
		{
			buf += consume();
		}

		update_col(buf.length());
		add_keywords(buf);
	}

	/**
	 * Compares two strings to determine keyword equality.
	 *
	 * The function takes two parameters:
	 * -val1 -> Lexed identifier string.
	 * -val2 -> Keyword string to compare against.
	 * 
	 * it returns true if both strings are equal, false otherwise.
	 */

	inline bool match_keyword(std::string val1,std::string val2)
	{
		return val1 == val2;
	}

	/**
	 * Determines whether a lexed identifier is a reserved keyword.
	 *
	 * Matches the identifier against all known language keywords
	 * and emits the appropriate token. Defaults to an identifier
	 * token if no keyword match is found.
	 *
	 * The function takes one parameter:
	 * -buf -> The lexed identifier string.
	 */

	inline void add_keywords(std::string buf)
	{
		if (match_keyword(buf,"break"))
		{
			add_token(TokenType::TOKEN_KEYWORD_BREAK,buf);
		}
		else if (match_keyword(buf,"cast"))
		{
			add_token(TokenType::TOKEN_KEYWORD_CAST,buf);
		}
		else if (match_keyword(buf,"char"))
		{
			add_token(TokenType::TOKEN_KEYWORD_CHAR,buf);
		}
		else if (match_keyword(buf,"const"))
		{
			add_token(TokenType::TOKEN_KEYWORD_CONST,buf);
		}
		else if (match_keyword(buf,"continue"))
		{
			add_token(TokenType::TOKEN_KEYWORD_CONTINUE,buf);
		}
		else if (match_keyword(buf,"else"))
		{
			add_token(TokenType::TOKEN_KEYWORD_ELSE,buf);
		}
		else if (match_keyword(buf,"elif"))
		{
			add_token(TokenType::TOKEN_KEYWORD_ELIF,buf);
		}
		else if (match_keyword(buf,"extern"))
		{
			add_token(TokenType::TOKEN_KEYWORD_EXTERN,buf);
		}
		else if (match_keyword(buf,"fn"))
		{
			//DEBUG_PRINT("fn " ," found");
			add_token(TokenType::TOKEN_KEYWORD_FN,buf);
		}
		else if (match_keyword(buf,"i8"))
		{
			add_token(TokenType::TOKEN_KEYWORD_I8,buf);
		}
		else if (match_keyword(buf,"i16"))
		{
			add_token(TokenType::TOKEN_KEYWORD_I16,buf);
		}
		else if (match_keyword(buf,"i32"))
		{
			add_token(TokenType::TOKEN_KEYWORD_I32,buf);
		}
		else if (match_keyword(buf,"i64"))
		{
			add_token(TokenType::TOKEN_KEYWORD_I64,buf);
		}
		else if (match_keyword(buf,"if"))
		{
			add_token(TokenType::TOKEN_KEYWORD_IF,buf);
		}
		else if (match_keyword(buf,"loop"))
		{
			add_token(TokenType::TOKEN_KEYWORD_LOOP,buf);
		}
		else if (match_keyword(buf,"native"))
		{
			add_token(TokenType::TOKEN_KEYWORD_NATIVE,buf);
		}
		else if (match_keyword(buf,"pub"))
		{
			add_token(TokenType::TOKEN_KEYWORD_PUB,buf);
		}
		else if (match_keyword(buf,"return"))
		{
			add_token(TokenType::TOKEN_KEYWORD_RETURN,buf);
		}
		else if (match_keyword(buf,"while"))
		{
			add_token(TokenType::TOKEN_KEYWORD_WHILE,buf);
		}
		else
		{
			add_token(TokenType::TOKEN_IDENT,buf);
		}
	}

	/**
	 * Updates the column position after consuming characters.
	 *
	 * The function takes one parameter:
	 * -col_dx -> Number of columns to advance.
	 *
	 * Also updates internal tracking variables related to token boundaries.
	 */

	inline void update_col(int col_dx)
	{
		this->col += col_dx;
		this->current += col_dx;
		this->end = this->index;
	}

	/**
	 * Updates the row (line) position when a newline is encountered.
	 *
	 * The function takes one parameter:
	 * -row_dx -> Number of rows to advance.
	 *
	 * Resets the column counter and updates line tracking information.
	 */

	inline void update_row(int row_dx)
	{
		this->row += row_dx;
		this->line = this->index;
		this->col = 0;
	}

	/**
	 * Adds a token composed of two characters.
	 *
	 * Consumes both characters, emits the token,
	 * and advances the column counter accordingly.
	 *
	 * The function takes two parameter:
	 * -type   -> Token type to emit.
	 * -string -> Optional string representation of the token.
	 */

	inline void add_token_double(TokenType type, std::string string = "")
	{
		consume();
		consume();
		add_token(type,string);
		update_col(2);
	}

	/**
	 * Adds a token composed of a single character.
	 *
	 * Consumes the character, emits the token,
	 * and advances the column counter.
	 *
	 * The function takes two parameters:
	 * -type   -> Token type to emit.
	 * -string -> Optional string representation of the token.
	 */

	inline void add_token_single(TokenType type,std::string string = "")
	{
		consume();
		add_token(type,string);
		update_col(1);
	}

	/**
	 * Appends a token to the token stream.
	 *
	 * Stores token metadata including source range
	 * and positional information for diagnostics.
	 *
	 * This function takes two parameters:
	 * -type   -> Token type.
	 * -string -> Optional token lexeme.
	 */

	inline void add_token(TokenType type, std::string string = "")
	{
		this->tokens.push_back(Tokens(type,string,this->start,this->end,this->row,this->col,this->line));
	}

	/**
	 * Scans and emits the next token from the source input.
	 *
	 * Determines the token type based on the current character,
	 * dispatching to specialized lexing functions or directly
	 * emitting punctuation and operator tokens.
	 *
	 * Whitespace is consumed but not emitted as tokens.
	 */

	inline void scan_token()
	{
		char token = peek();
		//DEBUG_PRINT("scan_token ","found");

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
			 case '_':
				 make_identifier();
				 break;
			 case '0':
			 case '1':
			 case '2':
			 case '3':
			 case '4':
			 case '5':
			 case '6':
			 case '7':
			 case '8':
			 case '9':
				 make_number();
				 break;
			 case '\"':
				 make_string();
				 break;
			 case ':':
				add_token_single(TokenType::TOKEN_COLON);
				break;
			 case '~':
				add_token_single(TokenType::TOKEN_TILDE);
				break;
			 case ';':
				add_token_single(TokenType::TOKEN_SEMI_COLON);
				break;
			 case '(':
				add_token_single(TokenType::TOKEN_LBRACE);
				break;
			 case ')':
				add_token_single(TokenType::TOKEN_RBRACE);
				break;
			 case '[':
				add_token_single(TokenType::TOKEN_LBRACKET);
				break;
			 case ']':
				add_token_single(TokenType::TOKEN_RBRACKET);
				break;
			 case ',':
				add_token_single(TokenType::TOKEN_COMMA);
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
				update_row(1);
				update_col(1);
				consume();
				break;
			 case '*':
				add_token_single(TokenType::TOKEN_MUL,"*");
				break;
			 case '/':
				add_token_single(TokenType::TOKEN_DIV,"/");
				break;
			 case '%':
				add_token_single(TokenType::TOKEN_MOD,"%");
				break;
			 case '+':
				add_token_single(TokenType::TOKEN_ADD,"+");
				break;
			 case '-':
				if ( match_token('>',1))
				    add_token_double(TokenType::TOKEN_RETPARAM,"->");
				else
				    add_token_single(TokenType::TOKEN_SUB,"-");
				break;
			 case '>':
				if ( match_token('=',1))
				    add_token_double(TokenType::TOKEN_GREATER_EQUAL,">=");
				else
				    add_token_single(TokenType::TOKEN_GREATER,">");
				break;
			 case '<':
				if ( match_token('=',1))
				    add_token_double(TokenType::TOKEN_LESS_EQUAL,">=");
				else
				    add_token_single(TokenType::TOKEN_LESS,">");
				break;
			 case '=':
				if ( match_token('=',1))
				    add_token_double(TokenType::TOKEN_EQUAL,"==");
				else
				    add_token_single(TokenType::TOKEN_ASSIGN,"=");
				break;
			 case '!':
				if ( match_token('=',1))
				    add_token_double(TokenType::TOKEN_NOT_EQUAL,"!=");
				else
				    add_token_single(TokenType::TOKEN_NOT,"!");
				break;
			 case '&':
				if ( match_token('&',1))
				    add_token_double(TokenType::TOKEN_AND,"&&");
				else
				    add_token_single(TokenType::TOKEN_BITWISE_AND,"&");
				break;
			 case '|':
				if ( match_token('|',1))
				    add_token_double(TokenType::TOKEN_OR,"||");
				else
				    add_token_single(TokenType::TOKEN_BITWISE_OR,"|");
				break;
			 default:
				consume(); 
		}
	}

public:

	

	/**
	 * Performs a full lexical scan of the source input.
	 *
	 * Iteratively scans tokens until the end of file is reached,
	 * then appends an explicit EOF token.
	 *
	 * The functino returns a vector containing all generated tokens.
	 */
	/*>>>>>>>>>>>> Main function <<<<<<<<<<<<<<<<*/
	inline std::vector<Tokens> scan_tokens()
	{
		while (not at_end())
		{
			this->start = this->current;
			scan_token();
		}

		add_token(TokenType::TOKEN_EOF,"eof");
		return this->tokens;
	}

	/**
	 * Used together with `print_errors()`, this function prints out the error found,
	 * the line at which its found, the part of the code with the error, and the associated message.
	 */

	/******************************************************************************\
	 * 																			  *
	 * NOTE: The line variable that gets printed corresponds to where the index	  *
	 *       points, which is at the end of the extracted code (the source_part), *
	 * 		 and not the begining, should be improved later.					  *
	 \*****************************************************************************/

	void print_error(Tokens token, std::string msg)
	{
		int start = token.start;
		int end = token.end;
		int line = this->row;
		int col = this->col;
		std::string source_part = this->file_source.substr(start,end);

		DEBUG_PRINT("An error in the lexer at line ", line);
		DEBUG_PRINT("Here :", source_part);
		DEBUG_PRINT("message: ", msg);
	}


	/**
	 * This function first checks if the scanned source has any errors,
	 * if found, it loops through all the available tokens and looks for 
	 * lexical errors ones.
	 * It then prints out the errors with relevant messages.
	 */
	void print_errors()
	{
		if (this->has_errors)
		{
			for (Tokens token : this->tokens)
			{
				TokenType this_token_type = token.type;

				switch (this_token_type)
				{
				case TokenType::TOKEN_ERROR_INVALID_CHARACTER:
					print_error(token, "invalid character in input");
					break;

				case TokenType::TOKEN_ERROR_MALFORMED_NUMBER:
					print_error(token, "malformed numeric literal");
					break;

				case TokenType::TOKEN_ERROR_UNTERMINATED_STRING:
					print_error(token, "unterminated string literal (missing closing \")");
					break;

				case TokenType::TOKEN_ERROR_UNTERMINATED_CHARACTER:
					print_error(token, "unterminated character literal (missing closing ')");
					break;

				case TokenType::TOKEN_ERROR_INVALID_ESCAPE_CHARACTER:
					print_error(token, "invalid escape sequence");
					break;

				default:
					break;
				}
			}
		}
	}

	inline void print()
	{
		for (Tokens token : this->tokens)
		{
			token.print();
		}
	}
};

#endif
