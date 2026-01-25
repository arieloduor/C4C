#ifndef C4C_AST_TO_C_H
#define C4C_AST_TO_C_H

#include "../../../front_end/include/ast.hpp"
#include <string>
#include <vector>


class AstToC
{
public:
    std::string file_name;
    ASTProgram *program;
    std::string string;


	void write_body(std::string string)
	{
		this->string += string;
	}

    AstToC(std::string file_name,ASTProgram *program)
    {
        this->file_name = file_name;
        this->program = program;

        for (ASTDeclaration *decl : this->program->decls)
		{
			if (decl == nullptr)
			{
				break;
			}
			
            convert_decl(decl);
		}


    }

    
	void convert_decl(ASTDeclaration *decl)
	{
		switch (decl->type)
		{
			case ASTDeclarationType::FUNCTION:
			{
				convert_function((ASTFunctionDecl *)decl->decl);
				break;
			}
			default:
			{
				DEBUG_PRINT("here "," null tac_decl");
			}
			/*
			case ASTDeclarationType::VARDECL:
			{
				TACFunction *tac_vardecl = convert_global_vardecl((ASTVarDecl *)decl->decl);
				tac_decl = new(mem) TACDeclaration(TACDeclarationType::VARDECL,tac_vardecl);
				break;
			}*/
		}
	}



	void convert_function(ASTFunctionDecl *decl)
	{
		switch(decl->return_type->type)
		{
			case ASTDataType::I32:
			{
				write_body("int ");
				break;
			}
			case ASTDataType::I64:
			{
				write_body("long int ");
				break;
			}
			case ASTDataType::U32:
			{
				write_body("unsigned int ");
				break;
			}
			case ASTDataType::U64:
			{
				write_body("unsigned long int ");
				break;
			}
		}

		write_body(decl->ident + "(");

		int arg_length = decl->arguments.size();
		int i = 0;

		for (ASTFunctionArgument *arg : decl->arguments)
		{
			if (arg == nullptr)
			{
				continue;
			}

			std::string data_type;

			switch(arg->type->type)
			{
				case ASTDataType::I32:
				{
					data_type = "int";
					break;
				}
				case ASTDataType::I64:
				{
					data_type = "long int";
					break;
				}
				case ASTDataType::U32:
				{
					data_type = "unsigned int";
					break;
				}
				case ASTDataType::U64:
				{
					data_type = "unsigned long int";
					break;
				}
				default:
				{
					break;
				}
			}

			write_body(data_type + " " + arg->ident);

			if(i++ + 1 >= arg_length)
			{
				break;
			}

			write_body(",");

		}

		write_body(")\n");
		convert_block_stmt(decl->block);
	}



	void convert_block_stmt(ASTBlockStmt *block,std::string tab="")
	{
		tab += "\t";
		write_body("{\n");
		for (ASTStatement *stmt : block->stmts)
		{
			convert_stmt(stmt,tab);
		}
		write_body("}\n");
	}


	void convert_stmt(ASTStatement *stmt,std::string tab)
	{
		switch(stmt->type)
		{
			case ASTStatementType::RETURN:
			{
				convert_return_stmt((ASTReturnStmt *)stmt->stmt,tab);
				break;
			}
			case ASTStatementType::IF:
			{
				convert_if_stmt((ASTIfStmt *)stmt->stmt,tab);
				break;
			}
			case ASTStatementType::WHILE:
			{
				convert_while_stmt((ASTWhileStmt *)stmt->stmt,tab);
				break;
			}
			case ASTStatementType::BREAK:
			{
				convert_break_stmt((ASTBreakStmt *)stmt->stmt,tab);
				break;
			}
			case ASTStatementType::CONTINUE:
			{
				convert_continue_stmt((ASTContinueStmt *)stmt->stmt,tab);
				break;
			}
			case ASTStatementType::VARDECL:
			{
				convert_vardecl_stmt((ASTVarDecl *)stmt->stmt,tab);
				break;
			}
			case ASTStatementType::EXPR:
			{
				write_body(tab);
				convert_expr((ASTExpression *)stmt->stmt);
				write_body(";\n");
				break;
			}
		}
	}



	void convert_break_stmt(ASTBreakStmt *stmt,std::string tab)
	{
		write_body(tab);
		write_body("break;\n");
	}

	void convert_continue_stmt(ASTContinueStmt *stmt,std::string tab)
	{
		write_body(tab);
		write_body("continue;\n");
	}

	void convert_while_stmt(ASTWhileStmt *stmt,std::string tab)
	{
		write_body(tab);
		write_body("while (");
		convert_expr(stmt->expr);
		write_body(")\n");
		convert_block_stmt(stmt->block,tab + "\t");
	}


	void convert_if_stmt(ASTIfStmt *stmt,std::string tab)
	{
		write_body(tab);
		write_body("if(");
		convert_expr(stmt->expr);
		write_body(")\n");
		convert_block_stmt(stmt->block,tab + "\t");

		for (ASTIfElifBlock *elif_block : stmt->elif_blocks)
		{
			if (elif_block == nullptr)
            {
                continue;
            }

			write_body("else if(");
			convert_expr(elif_block->expr);
			write_body(")\n");
			convert_block_stmt(elif_block->block,tab + "\t");
		}

		if(stmt->else_block != nullptr)
		{
			write_body("else\n");
			convert_block_stmt(stmt->else_block->block,tab + "\t");
		}
	}



	void convert_vardecl_stmt(ASTVarDecl *stmt,std::string tab)
	{
		write_body(tab);
		std::string data_type;

		switch(stmt->type->type)
		{
			case ASTDataType::I32:
			{
				data_type = "int ";
				break;
			}
			case ASTDataType::I64:
			{
				data_type = "long int ";
				break;
			}
			case ASTDataType::U32:
			{
				data_type = "unsigned int ";
				break;
			}
			case ASTDataType::U64:
			{
				data_type = "unsigned long int ";
				break;
			}
			default:
			{
				DEBUG_PANIC("tac cast ");
				break;
			}
		}

		write_body(data_type + stmt->ident + " = ");
		convert_expr(stmt->expr);
		write_body(";\n");
	}



	void convert_return_stmt(ASTReturnStmt *stmt,std::string tab)
	{
		write_body(tab);
		write_body("return ");
		convert_expr(stmt->expr);
		write_body(";\n");
	}


	void convert_expr(ASTExpression *expr)
	{
		switch (expr->type)
		{
			case ASTExpressionType::CAST:
			{
				convert_cast_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::I32:
			{
				convert_i32_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::I64:
			{
				convert_i64_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::U32:
			{
				convert_u32_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::U64:
			{
				convert_u64_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::FUNCTION_CALL:
			{
				convert_function_call_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::VARIABLE:
			{
				convert_variable_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::ASSIGN:
			{
				convert_assign_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::UNARY:
			{
				convert_unary_expr(expr->expr,expr->data_type);
				break;
			}
			case ASTExpressionType::BINARY:
			{
				convert_binary_expr(expr->expr,expr->data_type);
				break;
			}
		}

	}


	void convert_i32_expr(void *expr,DataType expr_type)
	{
		write_body(std::to_string(((ASTI32Expr *)expr)->value));
	}


	void convert_u64_expr(void *expr,DataType expr_type)
	{
		write_body(std::to_string(((ASTU64Expr *)expr)->value));
	}


	void convert_u32_expr(void *expr,DataType expr_type)
	{
		write_body(std::to_string(((ASTU32Expr *)expr)->value));
	}


	void convert_i64_expr(void *expr,DataType expr_type)
	{
		write_body(std::to_string(((ASTI64Expr *)expr)->value));
	}


	void convert_function_call_expr(void *expr,DataType expr_type)
	{
		ASTFunctionCallExpr *fn_expr = (ASTFunctionCallExpr *)expr;

		write_body(fn_expr->ident + "(");

		int arg_length = fn_expr->args.size();
		int i = 0;

		for ( ASTExpression *arg : fn_expr->args)
		{
			convert_expr(arg);
			if(i++ + 1 >= arg_length)
			{
				break;
			}

			write_body(",");
		}

		write_body(")");
	}



	void convert_variable_expr(void *expr,DataType expr_type)
	{
		ASTVariableExpr *var_expr = (ASTVariableExpr *)expr;
		write_body(var_expr->ident);
	}


	void convert_assign_expr(void *expr,DataType expr_type)
	{
		ASTAssignExpr *assign_expr = (ASTAssignExpr *)expr;
		convert_expr(assign_expr->lhs);
		write_body(" = ");
		convert_expr(assign_expr->rhs);
	}



	void convert_cast_expr(void *expr,DataType expr_type)
	{
		ASTCastExpr *cast_expr = (ASTCastExpr *)expr;
	
		std::string data_type;
		switch(cast_expr->type)
		{
			case ASTDataType::I32:
			{
				data_type = "int";
				break;
			}
			case ASTDataType::I64:
			{
				data_type = "long int";
				break;
			}
			case ASTDataType::U32:
			{
				data_type = "unsigned int";
				break;
			}
			case ASTDataType::U64:
			{
				data_type = "unsigned long int";
				break;
			}
			default:
			{
				break;
			}
		}

		write_body("(" + data_type + ")");
		convert_expr(cast_expr->rhs);
	}
	

	void convert_binary_expr(void *expr,DataType expr_type)
	{
		ASTBinaryExpr *bin_expr = (ASTBinaryExpr *)expr;

		convert_expr(bin_expr->lhs);
		convert_binop(bin_expr->op);
		convert_expr(bin_expr->rhs);
	}



	void convert_unary_expr(void *expr,DataType expr_type)
	{
		convert_unop(((ASTUnaryExpr *)expr)->op);
		convert_expr(((ASTUnaryExpr *)expr)->rhs);
	}





	void convert_unop(ASTUnaryOperator op)
	{
		switch (op)
		{
			case ASTUnaryOperator::NEGATE:
			{
				write_body("-");
				break;
			}
			case ASTUnaryOperator::COMPLEMENT:
			{
				write_body("~");
				break;
			}
		}
		
	}





	void convert_binop(ASTBinaryOperator op)
	{
		switch (op)
		{
			case ASTBinaryOperator::ADD:
			{
				write_body(" + ");
				break;
			}
			case ASTBinaryOperator::SUB:
			{
				write_body(" - ");
				break;
			}
			case ASTBinaryOperator::LESS:
			{
				write_body(" < ");
				break;
			}
			case ASTBinaryOperator::LESS_EQUAL:
			{
				write_body(" <= ");
				break;
			}
			case ASTBinaryOperator::GREATER:
			{
				write_body(" > ");
				break;
			}
			case ASTBinaryOperator::GREATER_EQUAL:
			{
				write_body(" >= ");
				break;
			}
			case ASTBinaryOperator::AND:
			{
				write_body(" && ");
				break;
			}
			case ASTBinaryOperator::OR:
			{
				write_body(" || ");
				break;
			}
		}

	
	}


};


#endif