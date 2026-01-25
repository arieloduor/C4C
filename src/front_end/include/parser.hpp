/****************************************************************************************************\
 * FILE: Parser.hpp																					*
 * AUTHOR: Ariel Oduor																				*
 *																									*
 * PUROPOSE: This file implements a recursive-descent parser for the C4C language.					*
 * The parser consumes a linear stream of lexical tokens produced by the lexer						*
 * and constructs a fully-formed Abstract Syntax Tree (AST) representing the						*
 * entire source file.																				*
 * The parser operates in a single pass and uses arena allocation for all AST						*
 * nodes, ensuring fast allocation and stable memory throughout compilation.						*
 *																									*
 * The root of the produced AST is an ASTProgram node, which contains a list						*
 * of top-level declarations such as functions, variables, and native bindings.						*
 *																									*
 * Parse responsibilities are:																		*
 * - Validate syntactic correctness of the token stream												*
 * - Enforce grammar structure and operator precedence												*
 * - Construct AST nodes for declarations, statements, and expressions								*
 * - Report fatal syntax errors and halt compilation on invalid input								*
 *																									*
 *																									*
 * USAGE: To use the program, first instantiate passing it three arguments:							*
 * 			-file_name																				*
 * 			-tokens																					*
 * 			-&arena																					*
 * 																									*
 * 			`Parser parser(file_name,tokens,&arena);`												*
 * 																									*
 * 			 then 																					*
 * 				`parser.parse_program()`															*
 *																									*
 * OUTPUT:																							*
 *   ASTProgram* stored in Parser::program															*
 *																									*
 * DESIGN NOTES:																					*
 * - Parsing strategy: recursive descent + precedence climbing for expressions						*
 * - Memory management: arena allocation (no manual deallocation)									*
 * - Grammar style: declaration-based, block-delimited by ':' tokens								*
 *																									*
 ****************************************************************************************************/


#ifndef C4C_PARSER_H
#define C4C_PARSER_H

#include "ast.hpp"
#include "../../utils/include/arena.hpp"



/**
 * The Parser class consumes a sequence of lexical tokens and produces an
 * Abstract Syntax Tree (AST) representing the structure of a source file.
 *
 * Parsing is performed using recursive-descent techniques, with explicit
 * handling of operator precedence for expressions.
 *
 * The parser maintains an internal cursor over the token stream and advances
 * strictly forward; backtracking is not supported.
 */

class Parser
{
public:
	std::string file_name;
	std::vector<Tokens> tokens;
	int tokens_length;
	int index = 0;
	ASTProgram *program;
	Arena *arena;

	Parser(std::string file_name,std::vector<Tokens> tokens,Arena *arena)
	{
		this->file_name = file_name;
		this->tokens = tokens;
		this->tokens_length = tokens.size();
		this->index = 0;
		this->program = nullptr;
		this->arena = arena;
	}

	/**
	 * Converts a keyword string into its corresponding token type.
	 * Returns TOKEN_EOF if the keyword is not recognized.
	 */

	TokenType keyword_to_token(std::string keyword)
	{
		if (keyword == "fn")
		{
			return TokenType::TOKEN_KEYWORD_FN;
		}
		else if (keyword == "return")
		{
			return TokenType::TOKEN_KEYWORD_RETURN;
		}
		else if (keyword == "while")
		{
			return TokenType::TOKEN_KEYWORD_WHILE;
		}
		else if (keyword == "break")
		{
			return TokenType::TOKEN_KEYWORD_BREAK;
		}
		else if (keyword == "cast")
		{
			return TokenType::TOKEN_KEYWORD_CAST;
		}
		else if (keyword == "continue")
		{
			return TokenType::TOKEN_KEYWORD_CONTINUE;
		}
		else if (keyword == "if")
		{
			return TokenType::TOKEN_KEYWORD_IF;
		}
		else if (keyword == "elif")
		{
			return TokenType::TOKEN_KEYWORD_ELIF;
		}
		else if (keyword == "else")
		{
			return TokenType::TOKEN_KEYWORD_ELSE;
		}
		else if (keyword == "enum")
		{
			return TokenType::TOKEN_KEYWORD_ENUM;
		}
		else if (keyword == "i32")
		{
			return TokenType::TOKEN_KEYWORD_I32;
		}
		else if (keyword == "i64")
		{
			return TokenType::TOKEN_KEYWORD_I64;
		}
		else if (keyword == "u32")
		{
			return TokenType::TOKEN_KEYWORD_U32;
		}
		else if (keyword == "u64")
		{
			return TokenType::TOKEN_KEYWORD_U64;
		}
		else if (keyword == "native")
		{
			return TokenType::TOKEN_KEYWORD_NATIVE;
		}
		else if (keyword == "pub")
		{
			return TokenType::TOKEN_KEYWORD_PUB;
		}
		else if (keyword == "loop")
		{
			return TokenType::TOKEN_KEYWORD_LOOP;
		}

		return TokenType::TOKEN_EOF;
	}

	/**
	 * Converts a symbol string into its corresponding token type.
	 * Returns TOKEN_EOF if the symbol is not recognized.
	 */

	TokenType symbol_to_token(std::string symbol)
	{
		if (symbol == "(")
		{
			return TokenType::TOKEN_LBRACE;
		}
		else if (symbol == ")")
		{
			return TokenType::TOKEN_RBRACE;
		}
		else if (symbol == "[")
		{
			return TokenType::TOKEN_LBRACKET;
		}
		else if (symbol == "]")
		{
			return TokenType::TOKEN_RBRACKET;
		}
		else if (symbol == "->")
		{
			return TokenType::TOKEN_RETPARAM;
		}
		else if (symbol == "*")
		{
			return TokenType::TOKEN_MUL;
		}
		else if (symbol == ":")
		{
			return TokenType::TOKEN_COLON;
		}
		else if (symbol == "+")
		{
			return TokenType::TOKEN_ADD;
		}
		else if (symbol == "-")
		{
			return TokenType::TOKEN_SUB;
		}
		else if (symbol == "~")
		{
			return TokenType::TOKEN_TILDE;
		}
		else if (symbol == ">")
		{
			return TokenType::TOKEN_GREATER;
		}
		else if (symbol == ">=")
		{
			return TokenType::TOKEN_GREATER_EQUAL;
		}
		else if (symbol == "<")
		{
			return TokenType::TOKEN_LESS;
		}
		else if (symbol == "<=")
		{
			return TokenType::TOKEN_LESS_EQUAL;
		}
		else if (symbol == "==")
		{
			return TokenType::TOKEN_EQUAL;
		}
		else if (symbol == "!=")
		{
			return TokenType::TOKEN_NOT_EQUAL;
		}
		else if (symbol == "&&")
		{
			return TokenType::TOKEN_AND;
		}
		else if (symbol == "||")
		{
			return TokenType::TOKEN_OR;
		}
		else if (symbol == "!")
		{
			return TokenType::TOKEN_NOT;
		}
		else if (symbol == "::")
		{
			return TokenType::TOKEN_RESOLUTION;
		}
		else if (symbol == "=")
		{
			return TokenType::TOKEN_ASSIGN;
		}
		else if (symbol == ",")
		{
			return TokenType::TOKEN_COMMA;
		}

		return TokenType::TOKEN_EOF;
	}


	/**
	 * Converts a raw string into a token type.
	 * Currently delegates to symbol_to_token().
	 */

	TokenType string_to_token(std::string string)
	{
		return symbol_to_token(string);
	}

	/**
	 * Returns a pointer to the current token or a future token without consuming it.
	 * The function takes one parameter:
	 * -look_ahead  -> Offset from the current token
	 * 
	 * It returns Pointer to token, or nullptr if out of bounds
	 */

	Tokens *peek(int look_ahead = 0)
	{
		if ( (this->index + look_ahead) >= this->tokens_length)
		{
			return nullptr;
		}

		return &this->tokens[this->index + look_ahead];
	}


	/**
	 * Consumes and returns the current token, advancing the parser cursor.
	 */

	Tokens consume()
	{
		return this->tokens[this->index++];
	}


	/**
	 * Checks whether the current or lookahead token matches a symbol.
	 */

	bool is_token_type(TokenType type, int look_ahead = 0)
	{
		Tokens *token = peek(look_ahead);
		if (token == nullptr)
		{
			return false;
		}
		return token->type == type;
	}


	/**
	 * Checks whether the current or lookahead token matches a symbol.
	 */

	bool is_token(std::string string,int look_ahead = 0)
	{
		return is_token_type(string_to_token(string),look_ahead);
	}

	/**
	 * Checks whether the current or lookahead token matches a keyword.
	 */

	bool is_token_string(std::string string,int look_ahead = 0)
	{
		return is_token_type(keyword_to_token(string),look_ahead);
	}


	/**
	 * Reports a fatal parsing error and terminates compilation.
	 * This function does not return.
	 */

	void fatal(std::string string)
	{
		DEBUG_PANIC(string);
	}


	/**
	 * Checks whether the current token represents a supported data type.
	 * More types to be added later
	 */

	bool match_type()
	{

		Tokens token = *(peek());

		if (token.type == TokenType::TOKEN_KEYWORD_I32)
		{
			return true;
		}
		else if (token.type == TokenType::TOKEN_KEYWORD_I64)
		{
			return true;
		}
		else if (token.type == TokenType::TOKEN_KEYWORD_U32)
		{
			return true;
		}
		else if (token.type == TokenType::TOKEN_KEYWORD_U64)
		{
			return true;
		}

		return false;
	}


	/**
	 * Checks whether the token at the lookahead position is similar
	 * to the one specified
	 * The function takes three parameters:
	 * - symbol 	-> The symbol to check
	 * - string 	->
	 * - look_ahead -> The number of positions ahead of the index to check,
	 *   if none is given a default of zero is used
	 */

	 /********************************************************\
	  * NOTE: As of the documentation of this function,		 *
	  * The string parameter is not in use and was probably  *
	  * used initially but the designer of the program		 *
	  * found a better way but forgot to remove it			 *
	 \********************************************************/

	bool is_symbol(std::string symbol,std::string string = "",int look_ahead = 0)
	{
		return is_token_type(symbol_to_token(symbol),look_ahead);
	}

	bool match_symbol(std::string symbol,std::string string = "",int look_ahead = 0)
	{
		if (is_token_type(symbol_to_token(symbol),look_ahead))
		{
			return true;
		}

		fatal("expected " + symbol + " but got " + (*(peek())).get_type() );
		return false;
	}

	/**
	 * Ensures that the current token matches the given symbol.
	 * Advances the token stream or reports a fatal error.
	 */

	bool expect_symbol(std::string symbol,std::string string = "")
	{
		bool status = false;
		if (is_token_type(symbol_to_token(symbol)))
		{
			status = true;
		}
		else
		{
			fatal("expected " + symbol + " but got " + (*(peek())).get_type() );
		}
		
		consume();
		return status;
	}



	/**
	 * Ensures that the current token matches the given symbol.
	 * Advances the token stream or reports a fatal error.
	 */

	bool match_keyword(std::string keyword,std::string string = "")
	{
		bool status = false;
		if (is_token_type(keyword_to_token(keyword)))
		{
			status = true;
		}
		
		return status;
	}

	/**
	 * Ensures that the current token matches the given keyword.
	 * Advances the token stream or reports a fatal error.
	 */

	bool expect_keyword(std::string keyword,std::string string = "")
	{
		bool status = false;
		if (is_token_type(keyword_to_token(keyword)))
		{
			status = true;
		}
		else
		{
			fatal("expected " + keyword + " but got " + (*(peek())).get_type() );
		}
		
		consume();
		return status;
	}

	/**
	 * Ensures that the current token's string matches the expected literal.
	 */

	bool expect_string_literal(std::string keyword,std::string string = "")
	{
		bool status = false;
		if ((*(peek())).string == keyword)
		{
			status = true;
		}
		else
		{
			fatal("expected " + keyword + " but got " + (*(peek())).get_type() );
		}
		
		consume();
		return status;
	}


	/**
	 * Checks whether the current token is an indentifier
	 */

	bool is_identifier()
	{
		return is_token_type(TokenType::TOKEN_IDENT);
	}



	/**
	 * This function just wraps the `is_identifier` function,
	 * if an identifier doesnt match, it calls the fatal function, which in tunr
	 * halts the program and prints an error message
	 */
	
	bool match_identifier()
	{
		if (is_identifier())
		{
			return true;
		}

		fatal("expected an identifier but got " + (*(peek())).get_type() );
		return true;
	}

	/**
	 * Checks whether the index has reached the end of the token's
	 * length.
	 */

	bool is_at_end()
	{
		return this->index >= this->tokens_length;
	}

	/**
	 * The function uses an arena to allocate memory of
	 * size `size`.
	 * The function takes one parameter:
	 * -size -> the size of memroy to be allocated
	 */

	void *alloc(int size)
	{
		return this->arena->alloc(size);
	}

	/**
	 * Parses the entire source file.
	 *
	 * Grammar:
	 *   Program -> Declaration
	 *
	 * Produces an ASTProgram node stored in Parser::program.
	 */

	void parse_program()
	{
		void *mem = alloc(sizeof(ASTProgram));
		this->program = new(mem) ASTProgram();

		while ( not is_at_end())
		{
			ASTDeclaration *decl = parse_decl();
			this->program->add_decl(decl);
		}

	}

	/**
	 * Parses a single top-level declaration.
	 *
	 * Supported declarations:
	 * - Function declarations
	 * - Variable declarations
	 * - Native (extern) declarations
	 * - enums
	 */

	ASTDeclaration *parse_decl()
	{
		Tokens token = (*(peek()));
		ASTDeclaration *decl = nullptr;
		void *mem = alloc(sizeof(ASTDeclaration));

		switch (token.type)
		{
			case TokenType::TOKEN_KEYWORD_EXTERN:
			{
				consume();
				if (match_type())
				{
					ASTVarDecl *decl_val = parse_vardecl(false,false,true);
					decl = new(mem) ASTDeclaration(ASTDeclarationType::VARDECL,decl_val);
				}
				else
				{
					fatal(" invalid use of 'pub' in " + (*(peek())).get_type());
				}
				break;
			}
			case TokenType::TOKEN_KEYWORD_PUB:
			{
				consume();
				if (match_keyword("fn"))
				{
					ASTFunctionDecl *decl_val = parse_fn_decl(true);
					decl = new(mem) ASTDeclaration(ASTDeclarationType::FUNCTION,decl_val);
				}
				else if (match_keyword("enum"))
				{
					ASTEnumDecl *decl_val = parse_enum_decl(true);
					decl = new(mem) ASTDeclaration(ASTDeclarationType::ENUM,decl_val);
				}
				else if (match_type())
				{
					ASTVarDecl *decl_val = parse_vardecl(true);
					decl = new(mem) ASTDeclaration(ASTDeclarationType::VARDECL,decl_val);
				}
				else
				{
					fatal(" invalid use of 'pub' in " + (*(peek())).get_type());
				}
				break;
			}
			case TokenType::TOKEN_KEYWORD_FN:
			{
				ASTFunctionDecl *decl_val2 = parse_fn_decl();		
				decl = new(mem) ASTDeclaration(ASTDeclarationType::FUNCTION,decl_val2);
				break;
			}
			case TokenType::TOKEN_KEYWORD_ENUM:
			{
				ASTEnumDecl *decl_val = parse_enum_decl();		
				decl = new(mem) ASTDeclaration(ASTDeclarationType::ENUM,decl_val);
				break;
			}
			case TokenType::TOKEN_KEYWORD_NATIVE:
			{
				ASTNativeDecl *decl_val3 = parse_native_decl();		
				decl = new(mem) ASTDeclaration(ASTDeclarationType::NATIVE,decl_val3);
				break;
			}
			case TokenType::TOKEN_EOF:
			{
				consume();
				break;
			}
			default:
			{
				if (match_type())
				{
					ASTVarDecl *decl_val4 = parse_vardecl(false,true,false);
					decl = new(mem) ASTDeclaration(ASTDeclarationType::VARDECL,decl_val4);
				}
				else
				{
					fatal("unknown declarator " + token.get_type() + "  " + token.string);
				}
			}
		}

		return decl;
	}

	/**
	 * This function parses an enum declaration
	 */
	ASTEnumDecl *parse_enum_decl(bool is_public = false)
	{
		void *mem = alloc(sizeof(ASTEnumDecl));
		ASTEnumDecl *decl = new(mem) ASTEnumDecl();
		decl->add_public(is_public);

		expect_keyword("enum");
		if (match_identifier())
		{
			decl->add_ident(consume().string);
		}

		expect_symbol(":");

		while (not is_token(":"))
		{
			if (match_identifier())
			{
				std::string ident = consume().string;
				int value = 0;
				bool has_value = false;

				if (is_token("="))
				{
					consume();
					value = std::stoi(consume().string);
					has_value = true;
				}
			
				mem = alloc(sizeof(ASTEnumConstant));
				ASTEnumConstant *enum_constant = new(mem) ASTEnumConstant(ident,value,has_value);

				decl->add_constant(enum_constant);

				if (is_token(":"))
				{
					break;
				}
			}
		}

		expect_symbol(":");

		return decl;
	}

	/**
	 * Parses a native declaration block.
	 * Used to declare externally implemented functions.
	 */
	
	ASTNativeDecl *parse_native_decl()
	{
		void *mem =  alloc(sizeof(ASTNativeDecl));
		ASTNativeDecl *decl = new (mem) ASTNativeDecl();

		expect_keyword("native");
		expect_string_literal("C");
		expect_symbol(":");

		while (not is_token("."))
		{
			ASTFunctionDeclNative *fn_decl = parse_fn_decl_native();
			decl->add_function(fn_decl);

			if (is_token(":"))
			{
				break;
			}
		}

		expect_symbol(":");

		return decl;
	}

	/**
	 * Parses a native function declaration.
	 */

	ASTFunctionDeclNative *parse_fn_decl_native()
	{
		void *mem = alloc(sizeof(ASTFunctionDeclNative));
		ASTFunctionDeclNative *decl = new(mem) ASTFunctionDeclNative();

		expect_keyword("fn");

		if (match_identifier())
		{
			decl->add_ident(consume().string);
		}

		expect_symbol("(");

		while (not is_token(")"))
		{
			ASTFunctionArgument *arg = parse_fn_arg();
			decl->add_argument(arg);

			if (is_symbol(")"))
			{
				break;
			}

			expect_symbol(",");
		}

		expect_symbol(")");
		expect_symbol("->");

		ASTType *return_type = parse_type();
		decl->add_return_type(return_type);

		return decl;
	}

	/**
	 * Parses a function declaration.
	 * The function takes one parameter:
	 * - is_public -> Indicates whether the function is publicly visible
	 */

	ASTFunctionDecl *parse_fn_decl(bool is_public = false)
	{
		void *mem = alloc(sizeof(ASTFunctionDecl));
		ASTFunctionDecl *decl = new(mem) ASTFunctionDecl();

		decl->add_public(is_public);

		expect_keyword("fn");

		if (match_identifier())
		{
			decl->add_ident(consume().string);
		}

		expect_symbol("(");

		while (not is_token(")"))
		{
			ASTFunctionArgument *arg = parse_fn_arg();
			decl->add_argument(arg);

			if (is_symbol(")"))
			{
				break;
			}

			expect_symbol(",");
		}

		expect_symbol(")");
		expect_symbol("->");

		ASTType *return_type = parse_type();
		decl->add_return_type(return_type);

		if ( match_symbol(":"))
		{
			ASTBlockStmt *block = parse_block_stmt();
			decl->add_block(block);
		}
		else
		{
			fatal("error : function lacks a valid block ");
		}

		return decl;
	}

	/**
	 * Parses a function argument consisting of a type and identifier.
	 */

	ASTFunctionArgument *parse_fn_arg()
	{
		ASTType *type = parse_type();

		std::string ident;

		if (match_identifier())
		{
			ident = consume().string;
		}

		void *mem = alloc(sizeof(ASTFunctionArgument));
		ASTFunctionArgument *arg = new(mem) ASTFunctionArgument(type,ident);

		return arg;
	}

	/**
	 * Parses a type specification, including pointer modifiers.
	 */

	ASTType *parse_type()
	{
		void *mem = alloc(sizeof(ASTType));
		ASTType *type = new(mem) ASTType();

		if (match_type())
		{
			ASTDataType data_type;
			if (is_token_string("i32"))
			{
				data_type = ASTDataType::I32;
				consume();
			}
			else if (is_token_string("i64"))
			{
				data_type = ASTDataType::I64;
				consume();
			}
			else if (is_token_string("u32"))
			{
				data_type = ASTDataType::U32;
				consume();
			}
			else if (is_token_string("u64"))
			{
				data_type = ASTDataType::U64;
				consume();
			}
			else
			{
				DEBUG_PRINT("  parse type : sanity check ",(*(peek())).get_type() + " =>  " + (*(peek())).string);
			}

			type->add_type(data_type);
		}
		else
		{
			fatal("unsupported types ");
		}

		while (is_token("*"))
		{
			type->add_ptr(1);
			consume();
		}

		return type;
	}

	/**
	 * Parses a block statement delimited by ':' tokens.
	 */

	ASTBlockStmt *parse_block_stmt()
	{
		void *mem = alloc(sizeof(ASTBlockStmt));
		ASTBlockStmt *block = new(mem) ASTBlockStmt();

		expect_symbol(":");

		while (true)
		{
			if (is_symbol(":"))
			{
				break;
			}

			ASTStatement *stmt = parse_stmt();
			block->add_stmt(stmt);
		}

		expect_symbol(":");

		return block;
	}

	/**
	 * Parses a single statement within a block.
	 */

	ASTStatement *parse_stmt()
	{
		Tokens token = (*(peek()));
		void *mem = alloc(sizeof(ASTStatement));
		ASTStatement *stmt = nullptr;

		switch (token.type)
		{
			case TokenType::TOKEN_KEYWORD_RETURN:
			{
				ASTStatementType stmt_type = ASTStatementType::RETURN;
				ASTReturnStmt *stmt_stmt = parse_return_stmt();
				stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				break;
			}
			case TokenType::TOKEN_KEYWORD_WHILE:
			{
				ASTStatementType stmt_type = ASTStatementType::WHILE;
				ASTWhileStmt *stmt_stmt = parse_while_stmt();
				stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				break;
			}
			case TokenType::TOKEN_KEYWORD_LOOP:
			{
				ASTStatementType stmt_type = ASTStatementType::WHILE;
				ASTWhileStmt *stmt_stmt = parse_loop_stmt();
				stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				break;
			}
			case TokenType::TOKEN_KEYWORD_BREAK:
			{
				ASTStatementType stmt_type = ASTStatementType::BREAK;
				ASTBreakStmt *stmt_stmt = parse_break_stmt();
				stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				break;
			}
			case TokenType::TOKEN_KEYWORD_CONTINUE:
			{
				ASTStatementType stmt_type = ASTStatementType::CONTINUE;
				ASTContinueStmt *stmt_stmt = parse_continue_stmt();
				stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				break;
			}
			case TokenType::TOKEN_KEYWORD_IF:
			{
				ASTStatementType stmt_type = ASTStatementType::IF;
				ASTIfStmt *stmt_stmt = parse_if_stmt();
				stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				break;
			}
			case TokenType::TOKEN_KEYWORD_STATIC:
			{
				consume();
				if (match_type())
				{
					ASTStatementType stmt_type = ASTStatementType::VARDECL;
					ASTVarDecl *stmt_stmt = parse_vardecl(false,true);
					stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				}
			}
			default:
			{
				if (match_type())
				{
					ASTStatementType stmt_type = ASTStatementType::VARDECL;
					ASTVarDecl *stmt_stmt = parse_vardecl();
					stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				}
				else if ( is_token("=",1) or is_token("(",1))
				{
					ASTStatementType stmt_type = ASTStatementType::EXPR;
					ASTExpression *stmt_stmt = parse_expr(0);
					stmt = new(mem) ASTStatement(stmt_type,stmt_stmt);
				}
				else
				{
					fatal("unknown statement " + token.get_type() + "  " + token.string);
				}
				break;
			}
		}

		return stmt;
	}

	/**
	 * Parses a variable declaration.
	 */

	ASTVarDecl *parse_vardecl(bool is_public = false,bool is_static = false,bool is_extern = false)
	{
		ASTType *type = parse_type();
		std::string ident;

		if (is_public && is_static)
		{
			fatal("a public variable cannot be static ");
		}

		if (match_identifier())
		{
			ident = consume().string;
		}

		ASTExpression *expr = nullptr;
		if (is_extern == false)
		{
			expect_symbol("=");
			expr = parse_expr(0);
		}

		void *mem = alloc(sizeof(ASTVarDecl));
		ASTVarDecl *decl = new(mem) ASTVarDecl(type,ident,expr,is_public,is_static,is_extern);

		return decl;
	}

	ASTIfElifBlock *parse_elif_block()
	{
		expect_keyword("elif");
		ASTExpression *expr = parse_expr(0);

		ASTBlockStmt *block = nullptr;

		if (match_symbol(":"))
		{
			block = parse_block_stmt();
		}
		else
		{
			fatal(" expected a block after an elif but got   " + (*(peek())).get_type());
		}

		void *mem = alloc(sizeof(ASTIfElifBlock));
		ASTIfElifBlock *elif_block = new(mem) ASTIfElifBlock(expr,block);
		return elif_block;
	}


	/**
	 * Parses an else block
	 */
	
	ASTIfElseBlock *parse_else_block()
	{
		expect_keyword("else");

		ASTBlockStmt *block = nullptr;

		if (match_symbol(":"))
		{
			block = parse_block_stmt();
		}
		else
		{
			fatal(" expected a block after an elif but got   " + (*(peek())).get_type());
		}

		void *mem = alloc(sizeof(ASTIfElseBlock));
		ASTIfElseBlock *else_block = new(mem) ASTIfElseBlock(block);
		return else_block;
	}

	/**
	 * Parses an if / elif / else statement.
	 */

	ASTIfStmt *parse_if_stmt()
	{
		void *mem = alloc(sizeof(ASTIfStmt));
		ASTIfStmt *stmt = new(mem) ASTIfStmt();

		expect_keyword("if");
		ASTExpression *expr = parse_expr(0);
		stmt->add_expr(expr);

		if (match_symbol(":"))
		{
			ASTBlockStmt *block = parse_block_stmt();
			stmt->add_block(block);
		}
		else
		{
			fatal(" expected a block after an elif but got   " + (*(peek())).get_type());
		}

		while ( match_keyword("elif"))
		{
			ASTIfElifBlock *elif_block = parse_elif_block();
			stmt->add_elif_block(elif_block);
		}

		if ( match_keyword("else"))
		{
			ASTIfElseBlock *else_block = parse_else_block();
			stmt->add_else_block(else_block);
		}

		return stmt;
	}


	/**
	 * Parses a while statment
	 */
	
	ASTWhileStmt *parse_while_stmt()
	{
		void *mem = alloc(sizeof(ASTWhileStmt));
		ASTWhileStmt *stmt = new(mem) ASTWhileStmt();

		expect_keyword("while");
		ASTExpression *expr = parse_expr(0);
		stmt->add_expr(expr);

		if (match_symbol(":"))
		{
			ASTBlockStmt *block = parse_block_stmt();
			stmt->add_block(block);
		}
		else
		{
			fatal(" expected a block after an elif but got   " + (*(peek())).get_type());
		}

		return stmt;
	}

	/**
	 * Parses an infinite loop statement.
	 */

	ASTWhileStmt *parse_loop_stmt()
	{
		void *mem = alloc(sizeof(ASTWhileStmt));
		ASTWhileStmt *stmt = new(mem) ASTWhileStmt();

		expect_keyword("loop");
		mem = alloc(sizeof(ASTI32Expr));
		ASTI32Expr *i32_expr = new(mem) ASTI32Expr(1);
		mem = alloc(sizeof(ASTExpression));
		ASTExpression *expr = new(mem)ASTExpression(ASTExpressionType::I32,i32_expr);

		stmt->add_expr(expr);

		if (match_symbol(":"))
		{
			ASTBlockStmt *block = parse_block_stmt();
			stmt->add_block(block);
		}
		else
		{
			fatal(" expected a block after an elif but got   " + (*(peek())).get_type());
		}

		return stmt;
	}

	/**
	 * Parses a break statment
	 */
	
	ASTBreakStmt *parse_break_stmt()
	{
		void *mem = alloc(sizeof(ASTBreakStmt));
		ASTBreakStmt *stmt = new(mem) ASTBreakStmt();

		expect_keyword("break");

		return stmt;
	}
	

	/**
	 * Parses a continue statment
	 */
	
	ASTContinueStmt *parse_continue_stmt()
	{
		void *mem = alloc(sizeof(ASTContinueStmt));
		ASTContinueStmt *stmt = new(mem) ASTContinueStmt();

		expect_keyword("continue");

		return stmt;
	}


	/**
	 * Parses a return statment
	 */

	ASTReturnStmt *parse_return_stmt()
	{
		expect_keyword("return");
		ASTExpression *expr = parse_expr(0);

		void *mem = alloc(sizeof(ASTReturnStmt));
		ASTReturnStmt *stmt = new(mem) ASTReturnStmt(expr);
		return stmt;
	}

	/**
	 * Determines whether the current token represents a unary operator.
	 */

	bool is_unary()
	{
		return is_token("-") or is_token("~");
	}

	/**
	 * Determines whether the current token represents a binary operator.
	 */

	bool is_binary()
	{
		return is_token("-") or is_token("+") or is_token("*") or is_token("%") or is_token("/") or is_token("<") or is_token(">") or is_token("<=") or is_token(">=") or is_token("||") or is_token("&&")  or is_token("=");
	}

	/**
	 * The function returns the assign operator (=)
	 */

	ASTAssignOperator get_assign_op(Tokens token)
	{
		switch (token.type)
		{
			case TokenType::TOKEN_ASSIGN:
			{
				return ASTAssignOperator::ASSIGN;
				break;
			}
		}

		return ASTAssignOperator::None;
	}

	
	/**
	 * This function returns the unary operator
	 */

	ASTUnaryOperator get_unary_op(Tokens token)
	{
		switch (token.type)
		{
			case TokenType::TOKEN_SUB:
			{
				return ASTUnaryOperator::NEGATE;
				break;
			}
			case TokenType::TOKEN_TILDE:
			{
				return ASTUnaryOperator::COMPLEMENT;
				break;
			}
		}

		return ASTUnaryOperator::None;
	}


	/**
	 * This function return the type of binary operation
	 * represented by the token
	 */

	ASTBinaryOperator get_binary_op(Tokens token)
	{
		switch (token.type)
		{
			case TokenType::TOKEN_SUB:
			{
				return ASTBinaryOperator::SUB;
				break;
			}
			case TokenType::TOKEN_ADD:
			{
				return ASTBinaryOperator::ADD;
				break;
			}
			case TokenType::TOKEN_MUL:
			{
				return ASTBinaryOperator::MUL;
				break;
			}
			case TokenType::TOKEN_DIV:
			{
				return ASTBinaryOperator::DIV;
				break;
			}
			case TokenType::TOKEN_MOD:
			{
				return ASTBinaryOperator::MOD;
				break;
			}
			case TokenType::TOKEN_LESS:
			{
				return ASTBinaryOperator::LESS;
				break;
			}
			case TokenType::TOKEN_LESS_EQUAL:
			{
				return ASTBinaryOperator::LESS_EQUAL;
				break;
			}
			case TokenType::TOKEN_GREATER:
			{
				return ASTBinaryOperator::GREATER;
				break;
			}
			case TokenType::TOKEN_GREATER_EQUAL:
			{
				return ASTBinaryOperator::GREATER_EQUAL;
				break;
			}
			case TokenType::TOKEN_AND:
			{
				return ASTBinaryOperator::AND;
				break;
			}
			case TokenType::TOKEN_OR:
			{
				return ASTBinaryOperator::OR;
				break;
			}
		}

		return ASTBinaryOperator::None;
	}

	/**
	 * Returns the precedence level of the current operator token.
	 */

	int get_precedence()
	{
		if ( is_token("+") or is_token("-"))
		{
			return (int)ASTPrecedence::ADD;
		}
		else if ( is_token("<") or is_token("<=") or is_token(">") or is_token(">="))
		{
			return (int)ASTPrecedence::LESS;
		}
		else if ( is_token("!=") or is_token("=="))
		{
			return (int)ASTPrecedence::EQUAL;
		}
		else if ( is_token("&&") )
		{
			return (int)ASTPrecedence::AND;
		}
		else if ( is_token("||") )
		{
			return (int)ASTPrecedence::OR;
		}
		else if ( is_token("=") )
		{
			return (int)ASTPrecedence::ASSIGN;
		}

		return -1;
	}

	/**
	 * Parses an expression using precedence climbing.
	 * The function takes one parameter:
	 * - min_prec -> Minimum operator precedence allowed
	 */

	ASTExpression *parse_expr(int min_prec)
	{
		ASTExpression *lhs = parse_factor();

		while ( is_binary() and get_precedence() >= min_prec)
		{
			int prec = get_precedence();

			if (is_token("="))
			{
				Tokens op = consume();
				ASTExpression *rhs = parse_expr(prec);
				void *mem = alloc(sizeof(ASTAssignExpr));
				ASTAssignExpr *assign_expr = new(mem) ASTAssignExpr(lhs,get_assign_op(op),rhs);
				mem = alloc(sizeof(ASTExpression));
				ASTExpression *tmp_lhs = new(mem) ASTExpression(ASTExpressionType::ASSIGN,assign_expr);
				lhs = tmp_lhs;
			}
			else
			{
				Tokens op = consume();
				ASTExpression *rhs = parse_expr(prec + 1);
				void *mem = alloc(sizeof(ASTBinaryExpr));
				ASTBinaryExpr *binary_expr = new(mem) ASTBinaryExpr(lhs,get_binary_op(op),rhs);
				mem = alloc(sizeof(ASTExpression));
				ASTExpression *tmp_lhs = new(mem) ASTExpression(ASTExpressionType::BINARY,binary_expr);
				lhs = tmp_lhs;
			}
		}

		return lhs;
	}

	/**
	 * Parses the smallest unit of an expression (literals, identifiers,
	 * function calls, unary expressions, or parenthesized expressions).
	 */

	ASTExpression *parse_factor()
	{
		ASTExpression *expr = nullptr;

		if (is_token_type(TokenType::TOKEN_LITERAL_INT))
		{
			Tokens token = consume();
			int num = std::stoi(token.string);
			if (num > ( std::pow(2,64)))
			{
				fatal("integer constant is too large (larger than 64 bits) ");
			}
			else if ((num > (std::pow(2,63) - 1)) and (num <= (std::pow(2,64))) )
			{
				void *mem = alloc(sizeof(ASTU64Expr));
				ASTU64Expr *u64_expr = new(mem) ASTU64Expr(num);
				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::U64,u64_expr);
			}
			else if (num > (std::pow(2,31) - 1))
			{
				void *mem = alloc(sizeof(ASTI64Expr));
				ASTI64Expr *i64_expr = new(mem) ASTI64Expr(num);
				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::I64,i64_expr);
			}
			else
			{
				void *mem = alloc(sizeof(ASTI32Expr));
				ASTI32Expr *i32_expr = new(mem) ASTI32Expr(num);
				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::I32,i32_expr);
			}

		}
		else if(is_unary())
		{
			void *mem = alloc(sizeof(ASTUnaryExpr));
			ASTUnaryOperator op = get_unary_op(consume());
			ASTUnaryExpr *expr1 = new(mem) ASTUnaryExpr(op,parse_factor());
			mem = alloc(sizeof(ASTExpression));
			expr = new(mem) ASTExpression(ASTExpressionType::UNARY,expr1);
		}
		else if (is_token_type(TokenType::TOKEN_LITERAL_FLOAT))
		{
			Tokens token = consume();

			double num = std::stod(token.string);

			if (not std::isfinite(num))
			{
				fatal("floating-point constant is not finite");
			}
			else if (std::fabs(num) > DBL_MAX)
			{
				fatal("floating-point constant is too large (larger than f64)");
			}
			else if (std::fabs(num) > FLT_MAX)
			{
				// Must be f64
				void *mem = alloc(sizeof(ASTF64Expr));
				ASTF64Expr *f64_expr = new(mem) ASTF64Expr(num);

				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::F64, f64_expr);
			}
			else
			{
				// Fits in f32
				float f32_val = static_cast<float>(num);

				void *mem = alloc(sizeof(ASTF32Expr));
				ASTF32Expr *f32_expr = new(mem) ASTF32Expr(f32_val);

				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::F32, f32_expr);
			}
		}

		else if (is_token_string("cast"))
		{
			expect_keyword("cast");
			expect_symbol("<");
			ASTDataType data_type;

			if (match_type())
			{
				if (is_token_string("i32"))
				{
					data_type = ASTDataType::I32;
					consume();
				}
				else if (is_token_string("i64"))
				{
					data_type = ASTDataType::I64;
					consume();
				}
				else
				{
					DEBUG_PRINT("  sanity check ",(*(peek())).get_type());
				}
			}
			else
			{
				fatal("unsupported types ");
			}

			expect_symbol(">");
			expect_symbol("(");
			ASTExpression *expr1 = parse_expr(0);
			expect_symbol(")");

			void *mem = alloc(sizeof(ASTCastExpr));
			ASTCastExpr *expr2 = new(mem) ASTCastExpr(data_type,expr1);

			mem = alloc(sizeof(ASTExpression));
			expr = new(mem) ASTExpression(ASTExpressionType::CAST,expr2);

		}
		else if(is_token("("))
		{
			expect_symbol("(");
			expr = parse_expr(0);
			expect_symbol(")");
		}
		else if(is_identifier())
		{
			if (is_token("(",1))
			{
				void *mem = alloc(sizeof(ASTFunctionCallExpr));
				ASTFunctionCallExpr *expr1 = new(mem) ASTFunctionCallExpr();

				expr1->add_ident(consume().string);
				expect_symbol("(");

				while (not is_token(")"))
				{
					expr1->add_arg(parse_expr(0));

					if (is_token(")"))
					{
						break;
					}

					expect_symbol(",");
				}

				expect_symbol(")");

				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::FUNCTION_CALL,expr1);
			}
			else if(is_token("::",1))
			{
				void *mem = alloc(sizeof(ASTResolutionExpr));
				ASTResolutionExpr *expr1 = new(mem) ASTResolutionExpr();
				expr1->add_ident(consume().string);

				while (is_token("::"))
				{
					expect_symbol("::");					
					if (match_identifier())
					{
						expr1->add_ident(consume().string);
					}
					else
					{
						fatal("expected an identifier after ::");
					}
				}

				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::RESOLUTION,expr1);
			}
			else
			{
				void *mem = alloc(sizeof(ASTVariableExpr));
				ASTVariableExpr *expr1 = new(mem) ASTVariableExpr(consume().string);
				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::VARIABLE,expr1);
			}
		}
		else
		{
			fatal("unknown expression " + (*(peek())).get_type());
		}

		if (expr == nullptr)
		{
			DEBUG_PRINT("parse expr "," entry ");
		}
		return expr;
	}
};


#endif

