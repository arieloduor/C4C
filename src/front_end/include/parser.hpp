#ifndef C4C_PARSER_H
#define C4C_PARSER_H

#include "ast.hpp"
#include "../../utils/include/arena.hpp"


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
		else if (keyword == "i32")
		{
			return TokenType::TOKEN_KEYWORD_I32;
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


	TokenType string_to_token(std::string string)
	{
		return symbol_to_token(string);
	}


	Tokens *peek(int look_ahead = 0)
	{
		if ( (this->index + look_ahead) >= this->tokens_length)
		{
			return nullptr;
		}

		return &this->tokens[this->index + look_ahead];
	}


	Tokens consume()
	{
		return this->tokens[this->index++];
	}


	bool is_token_type(TokenType type,int look_ahead = 0)
	{
		Tokens *token = peek(look_ahead);
		if (token == nullptr)
		{
			return false;
		}
		return token->type == type;
	}


	bool is_token(std::string string,int look_ahead = 0)
	{
		return is_token_type(string_to_token(string),look_ahead);
	}

	bool is_token_string(std::string string,int look_ahead = 0)
	{
		return is_token_type(keyword_to_token(string),look_ahead);
	}

	void fatal(std::string string)
	{
		DEBUG_PANIC(string);
	}


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

		return false;
	}


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



	bool match_keyword(std::string keyword,std::string string = "")
	{
		bool status = false;
		if (is_token_type(keyword_to_token(keyword)))
		{
			status = true;
		}
		
		return status;
	}


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


	bool is_identifier()
	{
		return is_token_type(TokenType::TOKEN_IDENT);
	}



	bool match_identifier()
	{
		if (is_identifier())
		{
			return true;
		}

		fatal("expected an identifier but got " + (*(peek())).get_type() );
		return true;
	}

	bool is_at_end()
	{
		return this->index >= this->tokens_length;
	}

	void *alloc(int size)
	{
		return this->arena->alloc(size);
	}
	
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

	ASTDeclaration *parse_decl()
	{
		Tokens token = (*(peek()));
		ASTDeclaration *decl = nullptr;
		void *mem = alloc(sizeof(ASTDeclaration));

		switch (token.type)
		{
			case TokenType::TOKEN_KEYWORD_PUB:
			{
				consume();
				if (match_keyword("fn"))
				{
					ASTFunctionDecl *decl_val1 = parse_fn_decl(true);
					decl = new(mem) ASTDeclaration(ASTDeclarationType::FUNCTION,decl_val1);
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
					ASTVarDecl *decl_val4 = parse_vardecl();
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
			else
			{
				DEBUG_PRINT("  sanity check ",(*(peek())).get_type());
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

	ASTVarDecl *parse_vardecl()
	{
		ASTType *type = parse_type();
		std::string ident;

		if (match_identifier())
		{
			ident = consume().string;
		}

		expect_symbol("=");
		ASTExpression *expr = parse_expr(0);

		void *mem = alloc(sizeof(ASTVarDecl));
		ASTVarDecl *decl = new(mem) ASTVarDecl(type,ident,expr);

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

	ASTBreakStmt *parse_break_stmt()
	{
		void *mem = alloc(sizeof(ASTBreakStmt));
		ASTBreakStmt *stmt = new(mem) ASTBreakStmt();

		expect_keyword("break");

		return stmt;
	}
	
	ASTContinueStmt *parse_continue_stmt()
	{
		void *mem = alloc(sizeof(ASTContinueStmt));
		ASTContinueStmt *stmt = new(mem) ASTContinueStmt();

		expect_keyword("continue");

		return stmt;
	}


	ASTReturnStmt *parse_return_stmt()
	{
		expect_keyword("return");
		ASTExpression *expr = parse_expr(0);

		void *mem = alloc(sizeof(ASTReturnStmt));
		ASTReturnStmt *stmt = new(mem) ASTReturnStmt(expr);
		return stmt;
	}

	bool is_unary()
	{
		return is_token("-") or is_token("~");
	}
	

	bool is_binary()
	{
		return is_token("-") or is_token("+") or is_token("*") or is_token("%") or is_token("/") or is_token("<") or is_token(">") or is_token("<=") or is_token(">=") or is_token("||") or is_token("&&")  or is_token("=");
	}

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
	

	ASTExpression *parse_expr(int min_prec)
	{
		ASTExpression *lhs = parse_factor();

		while ( is_binary() and get_precedence() >= min_prec)
		{
			int prec = get_precedence();

			if (is_token("="))
			{
				DEBUG_PRINT(" should not happen ","assign");
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
				DEBUG_PRINT(" $$$$$$$$$$$$$$$$$$     ",op.string);
			}
		}

		return lhs;
	}


	ASTExpression *parse_factor()
	{
		ASTExpression *expr = nullptr;

		if (is_token_type(TokenType::TOKEN_LITERAL_INT))
		{
			Tokens token = consume();
			int num = std::stoi(token.string);
			if (num > ( std::pow(2,63) - 1))
			{
				fatal("integer constant is too large (larger than 64 bits) ");
			}

			if (num > (std::pow(2,31) - 1))
			{
				void *mem = alloc(sizeof(ASTI64Expr));
				ASTI64Expr *i64_expr = new(mem) ASTI64Expr(std::stoi(token.string));
				mem = alloc(sizeof(ASTExpression));
				expr = new(mem) ASTExpression(ASTExpressionType::I64,i64_expr);
			}
			else
			{
				void *mem = alloc(sizeof(ASTI32Expr));
				ASTI32Expr *i32_expr = new(mem) ASTI32Expr(std::stoi(token.string));
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
			else
			{
				void *mem = alloc(sizeof(ASTVariableExpr));
				ASTVariableExpr *expr1 = new(mem) ASTVariableExpr(consume().string);
				DEBUG_PRINT(" parser : ",expr1->ident);
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

