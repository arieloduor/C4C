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
	inline bool at_end()
	{
		if (this->index >= this->file_length)
		{
			return true;
		}

		return false;
	}

	inline char peek(int lookahead = 0)
	{
		if ( (this->index + lookahead) >= this->file_length)
		{
			return '\0';
		}

		return this->file_source[this->index + lookahead];
	}
	

	inline char consume()
	{
		return this->file_source[this->index++];
	}

	inline bool is_token(char token,int lookahead = 0)
	{
		if (this->peek(lookahead) == token)
		{
			return true;
		}

		return false;
	}


	inline bool match_token(char token,int lookahead = 0)
	{
		return is_token(token,lookahead);
	}


	inline void expect_token(char token)
	{
		// not implemented
	}

	inline bool is_digit(int lookahead = 0)
	{
		char token = peek(lookahead);
		if ( (token >= '0') and (token <= '9'))
		{
			return true;
		}

		return false;
	}


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

			add_token(TokenType::TOKEN_LITERAL_FLOAT,buf);
		}
		else
		{
			add_token(TokenType::TOKEN_LITERAL_INT,buf);
		}

		update_col(buf.length());
	}


	inline void make_number()
	{
		make_decimal();
	}

	inline bool is_alpha()
	{
		char token = peek(0);
		if ( (token >= 'A' and token <= 'Z') or (token >= 'a' and token <= 'z') or (token == '_') )
		{
			return true;
		}

		return false;
	}


	inline bool is_alphanum()
	{
		return is_digit() or is_alpha();
	}


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
		}
		else
		{
			consume();
			add_token(TokenType::TOKEN_LITERAL_STRING,buf);
		}

		update_col(buf.length() +  2);
	}

	
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

	inline bool match_keyword(std::string val1,std::string val2)
	{
		return val1 == val2;
	}

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

	inline void update_col(int col_dx)
	{
		this->col += col_dx;
		this->current += col_dx;
		this->end = this->index;
	}


	inline void update_row(int row_dx)
	{
		this->row += row_dx;
		this->line = this->index;
		this->col = 0;
	}


	inline void add_token_double(TokenType type,std::string string = "")
	{
		consume();
		consume();
		add_token(type,string);
		update_col(2);
	}

	inline void add_token_single(TokenType type,std::string string = "")
	{
		consume();
		add_token(type,string);
		update_col(1);
	}

	inline void add_token(TokenType type,std::string string = "")
	{
		this->tokens.push_back(Tokens(type,string,this->start,this->end,this->row,this->col,this->line));
	}

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

	inline void print()
	{
		for (Tokens token : this->tokens)
		{
			token.print();
		}
	}

};

#endif
