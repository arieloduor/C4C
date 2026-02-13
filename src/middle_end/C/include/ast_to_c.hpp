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
			case ASTDeclarationType::ENUM:
			{
				convert_enum_decl((ASTEnumDecl *)decl->decl);
				break;
			}
			case ASTDeclarationType::STRUCT:
			{
				convert_struct_decl((ASTStructDecl *)decl->decl);
				break;
			}
			case ASTDeclarationType::IMPL:
			{
				convert_impl_decl((ASTImplDecl *)decl->decl);
				break;
			}
			case ASTDeclarationType::NATIVE:
			{
				convert_native_decl((ASTNativeDecl *)decl->decl);
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

	void convert_enum_decl(ASTEnumDecl *decl)
	{
		write_body("enum " + decl->ident + "\n{\n");

		for (ASTEnumConstant *enum_const : decl->constants)
		{
			if(enum_const == nullptr)
			{
				continue;
			}

			write_body("\t" + enum_const->ident);

			if(enum_const->has_value)
			{
				write_body(" = " + std::to_string(enum_const->value));
			}

			write_body(",\n");
		}

		write_body("};\n\n");
	}


	void convert_struct_decl(ASTStructDecl *decl)
	{
		write_body("struct " + decl->ident + "\n{\n");

		int arg_length = decl->properties.size();
		int i = 0;

		for (ASTStructProperty *arg : decl->properties)
		{
			if (arg == nullptr)
			{
				continue;
			}

			std::string data_type;

			switch(arg->type->type)
			{
				case ASTDataType::CHAR:
				{
					data_type = "char ";
					break;
				}
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
				case ASTDataType::F32:
				{
					data_type = "float";
					break;
				}
				case ASTDataType::F64:
				{
					data_type = "double";
					break;
				}
				default:
				{
					break;
				}
			}

			data_type += " ";
			for (int i = 0; i < arg->type->ptr; i++)
			{
				data_type += "*";
			}

			write_body("\t" + data_type + arg->ident);
			write_body(";\n");
		}


		write_body("};\n\n");
	}



	void convert_impl_decl(ASTImplDecl *decl)
	{

		for (ASTMethodDecl *method : decl->methods)
		{
			if (method == nullptr)
			{
				continue;
			}
			

			write_body("\n");

			convert_method_decl(method,decl->ident);

			write_body("\n");
		}

	}



	void convert_method_decl(ASTMethodDecl *decl,std::string base)
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
			case ASTDataType::F32:
			{
				write_body(" ");
				break;
			}
			case ASTDataType::F64:
			{
				write_body("double ");
				break;
			}
		}

		write_body(base + "_" + decl->ident + "(");

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
				case ASTDataType::CHAR:
				{
					data_type = "char ";
					break;
				}
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
				case ASTDataType::F32:
				{
					data_type = "float";
					break;
				}
				case ASTDataType::F64:
				{
					data_type = "double";
					break;
				}
				case ASTDataType::ENUM:
				{
					data_type = "enum " + arg->type->ident;
					break;
				}
				case ASTDataType::STRUCT:
				{
					data_type = "struct " + arg->type->ident;
					break;
				}
				default:
				{
					break;
				}
			}

			data_type += " ";
			for (int i = 0; i < arg->type->ptr; i++)
			{
				data_type += "*";
			}

			write_body(data_type + arg->ident);

			if(i++ + 1 >= arg_length)
			{
				break;
			}

			write_body(",");

		}

		write_body(")\n");
		convert_block_stmt(decl->block);
	}








	void convert_function_native(ASTFunctionDeclNative *decl)
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
			case ASTDataType::F32:
			{
				write_body(" ");
				break;
			}
			case ASTDataType::F64:
			{
				write_body("double ");
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
				case ASTDataType::CHAR:
				{
					data_type = "char ";
					break;
				}
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
				case ASTDataType::F32:
				{
					data_type = "float";
					break;
				}
				case ASTDataType::F64:
				{
					data_type = "double";
					break;
				}
				default:
				{
					break;
				}
			}

			data_type += " ";
			for (int i = 0; i < arg->type->ptr; i++)
			{
				data_type += "*";
			}

			write_body(data_type + arg->ident);

			if(i++ + 1 >= arg_length)
			{
				break;
			}

			write_body(",");

		}

		write_body(");\n");

	}


	void convert_native_decl(ASTNativeDecl *decl)
	{
		for(ASTFunctionDeclNative *fn : decl->functions)
		{
			convert_function_native(fn);
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
			case ASTDataType::F32:
			{
				write_body(" ");
				break;
			}
			case ASTDataType::F64:
			{
				write_body("double ");
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
				case ASTDataType::CHAR:
				{
					data_type = "char ";
					break;
				}
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
				case ASTDataType::F32:
				{
					data_type = "float";
					break;
				}
				case ASTDataType::F64:
				{
					data_type = "double";
					break;
				}
				default:
				{
					break;
				}
			}

			data_type += " ";
			for (int i = 0; i < arg->type->ptr; i++)
			{
				data_type += "*";
			}

			write_body(data_type + arg->ident);

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
		write_body(tab + "{\n");
		for (ASTStatement *stmt : block->stmts)
		{
			convert_stmt(stmt,tab + "\t");
		}
		write_body(tab + "}\n");
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
		convert_block_stmt(stmt->block,tab);
	}


	void convert_if_stmt(ASTIfStmt *stmt,std::string tab)
	{
		write_body(tab);
		write_body("if(");
		convert_expr(stmt->expr);
		write_body(")\n");
		convert_block_stmt(stmt->block,tab);

		for (ASTIfElifBlock *elif_block : stmt->elif_blocks)
		{
			if (elif_block == nullptr)
            {
                continue;
            }

			write_body("else if(");
			convert_expr(elif_block->expr);
			write_body(")\n");
			convert_block_stmt(elif_block->block,tab );
		}

		if(stmt->else_block != nullptr)
		{
			write_body("else\n");
			convert_block_stmt(stmt->else_block->block,tab);
		}
	}



	void convert_vardecl_stmt(ASTVarDecl *stmt,std::string tab)
	{
		write_body(tab);
		std::string data_type;

		switch(stmt->type->type)
		{
			case ASTDataType::CHAR:
			{
				data_type = "char ";
				break;
			}
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
			case ASTDataType::F32:
			{
				data_type = "float ";
				break;
			}
			case ASTDataType::F64:
			{
				data_type = "double ";
				break;
			}
			case ASTDataType::ENUM:
			{
				data_type = "enum " + stmt->type->ident + " ";
				break;
			}
			case ASTDataType::STRUCT:
			{
				data_type = "struct " + stmt->type->ident + " ";
				break;
			}
			default:
			{

				DEBUG_PANIC("unsupported type ");
				break;
			}
		}

		for(int i = 0; i < stmt->type->ptr; i++)
		{
			data_type += "*";
		}

		std::string new_line;
		write_body(data_type + stmt->true_ident + " = ");

		if(stmt->init->type == ASTVarInitType::SINGLE)
		{
			convert_expr(((ASTVarSingleInit *)stmt->init->init)->expr);
		}
		else if(stmt->init->type == ASTVarInitType::STRUCT)
		{
			ASTVarStructMember map = ((ASTVarStructInit *)stmt->init->init)->members;
			write_body("(struct " + ((ASTVarStructInit *)stmt->init->init)->ident + "){\n");
			for (auto it = map.table.begin(); it != map.table.end(); ++it)
			{
				write_body(tab + "\t." + it->first + " = ");
				convert_expr(it->second);
				write_body(",\n");  
			}
			write_body(tab + "}");
			new_line = "\n";
		}

		
		write_body(";\n" + new_line);
	}



	void convert_return_stmt(ASTReturnStmt *stmt,std::string tab)
	{
		write_body(tab);
		write_body("return ");
		convert_expr(stmt->expr);
		write_body(";\n");
	}


	std::string convert_expr(ASTExpression *expr,bool write = true,std::string dot = ".")
	{
		switch (expr->type)
		{
			case ASTExpressionType::CAST:
			{
				convert_cast_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::STRING:
			{
				convert_string_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::I32:
			{
				convert_i32_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::I64:
			{
				convert_i64_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::U32:
			{
				convert_u32_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::U64:
			{
				convert_u64_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::F32:
			{
				convert_f32_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::F64:
			{
				convert_f64_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::FUNCTION_CALL:
			{
				convert_function_call_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::VARIABLE:
			{
				return convert_variable_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::SELF:
			{
				convert_self_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::ADDRESS_OF:
			{
				convert_address_of_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::PTR_READ:
			{
				write_body("(");
				convert_ptr_read_expr(expr->expr,expr->data_type,write);
				write_body(")");
				break;
			}
			case ASTExpressionType::PTR_WRITE:
			{
				write_body("(");
				convert_ptr_write_expr(expr->expr,expr->data_type,write);
				write_body(")");
				break;
			}
			case ASTExpressionType::ASSIGN:
			{
				convert_assign_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::ENUM_ACCESS:
			{
				convert_enum_access_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::STRUCT_ACCESS:
			{
				return convert_struct_access_expr(expr->expr,expr->data_type,write,dot);
				break;
			}
			case ASTExpressionType::STRUCT_PTR_ACCESS:
			{
				return convert_struct_ptr_access_expr(expr->expr,expr->data_type,write,dot);
				break;
			}
			case ASTExpressionType::UNARY:
			{
				convert_unary_expr(expr->expr,expr->data_type,write);
				break;
			}
			case ASTExpressionType::BINARY:
			{
				convert_binary_expr(expr->expr,expr->data_type,write);
				break;
			}
		}

		return "";

	}



	std::string convert_struct_ptr_access_expr(void *expr,DataType expr_type,bool write = true,std::string dot = "->")
	{
		if(dot == ".")
		{
			dot = "->";
		}

		std::string ret = convert_expr(((ASTStructPtrAccessExpr *)expr)->base);
		write_body(dot + ((ASTStructPtrAccessExpr *)expr)->member);

		return ret;
	}


	std::string convert_struct_access_expr(void *expr,DataType expr_type,bool write = true,std::string dot = ".")
	{
		std::string ret;

		ret = convert_expr(((ASTStructAccessExpr *)expr)->base,write);

		if(dot != ".")
		{
			write_body(((ASTStructAccessExpr *)expr)->base_type);
		}

		write_body(dot + ((ASTStructAccessExpr *)expr)->member);

		return ret;
	}


	void convert_enum_access_expr(void *expr,DataType expr_type,bool write = true)
	{
		write_body(((ASTEnumAccessExpr *)expr)->base + "_" + ((ASTEnumAccessExpr *)expr)->member);
	}

	void convert_string_expr(void *expr,DataType expr_type,bool write = true)
	{
		write_body("\"" + ((ASTStringExpr *)expr)->value +"\"");
	}

	void convert_i32_expr(void *expr,DataType expr_type,bool write = true)
	{
		write_body(std::to_string(((ASTI32Expr *)expr)->value));
	}


	void convert_i64_expr(void *expr,DataType expr_type,bool write = true)
	{
		write_body(std::to_string(((ASTI64Expr *)expr)->value));
	}



	void convert_u64_expr(void *expr,DataType expr_type,bool write = true)
	{
		write_body(std::to_string(((ASTU64Expr *)expr)->value));
	}


	void convert_u32_expr(void *expr,DataType expr_type,bool write = true)
	{
		write_body(std::to_string(((ASTU32Expr *)expr)->value));
	}




	void convert_f64_expr(void *expr,DataType expr_type,bool write = true)
	{
		write_body(std::to_string(((ASTF64Expr *)expr)->value));
	}


	void convert_f32_expr(void *expr,DataType expr_type,bool write = true)
	{
		write_body(std::to_string(((ASTF32Expr *)expr)->value));
	}


	void convert_function_call_expr(void *expr,DataType expr_type,bool write = true)
	{
		ASTFunctionCallExpr *fn_expr = (ASTFunctionCallExpr *)expr;

		std::string dot = ".";
		std::string ptr;
		
		if(fn_expr->base->type == ASTExpressionType::STRUCT_ACCESS)
		{
			dot = "_";
			ptr = ".";
		}
		else if(fn_expr->base->type == ASTExpressionType::STRUCT_PTR_ACCESS)
		{
			dot = "_";
			ptr = "->";
		}

		std::string is_self;
		if(ptr == ".")
		{
			is_self = convert_expr(fn_expr->base,false,dot);
		}
		else
		{
			convert_expr(fn_expr->base,true,dot);
		}

		write_body("(");

		if(ptr == ".")
		{
			write_body("&" + is_self);
		}
		else if(ptr == "->")
		{
			write_body(is_self);
		}

		int arg_length = fn_expr->args.size();

		if(arg_length > 0 and dot == "_")
		{
			write_body(",");
		}

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



	std::string convert_variable_expr(void *expr,DataType expr_type,bool write = true)
	{
		ASTVariableExpr *var_expr = (ASTVariableExpr *)expr;

		if(write == true)
		{
			write_body(var_expr->true_ident);
		}

		return var_expr->true_ident;
	}



	void convert_self_expr(void *expr,DataType expr_type,bool write = true)
	{
		ASTSelfExpr *self_expr = (ASTSelfExpr *)expr;
		write_body(self_expr->ident);
	}



	void convert_address_of_expr(void *expr,DataType expr_type,bool write = true)
	{
		ASTAddressOfExpr *address_of_expr = (ASTAddressOfExpr *)expr;
		write_body("&");
		convert_expr(address_of_expr->expr);
	}



	void convert_ptr_read_expr(void *expr,DataType expr_type,bool write = true)
	{
		ASTPtrReadExpr *ptr_read_expr = (ASTPtrReadExpr *)expr;
		write_body("*");
		convert_expr(ptr_read_expr->expr);
	}



	void convert_ptr_write_expr(void *expr,DataType expr_type,bool write = true)
	{
		ASTPtrWriteExpr *ptr_write = (ASTPtrWriteExpr *)expr;
		write_body("*");
		convert_expr(ptr_write->expr);
		write_body(" = ");
		convert_expr(ptr_write->data);

	}



	void convert_assign_expr(void *expr,DataType expr_type,bool write = true)
	{
		ASTAssignExpr *assign_expr = (ASTAssignExpr *)expr;
		convert_expr(assign_expr->lhs);
		write_body(" = ");
		convert_expr(assign_expr->rhs);
	}



	void convert_cast_expr(void *expr,DataType expr_type,bool write = true)
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
			case ASTDataType::F32:
			{
				data_type = "float ";
				break;
			}
			case ASTDataType::F64:
			{
				data_type = "double ";
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
	

	void convert_binary_expr(void *expr,DataType expr_type,bool write = true)
	{
		ASTBinaryExpr *bin_expr = (ASTBinaryExpr *)expr;

		convert_expr(bin_expr->lhs);
		convert_binop(bin_expr->op);
		convert_expr(bin_expr->rhs);
	}



	void convert_unary_expr(void *expr,DataType expr_type,bool write = true)
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
			case ASTBinaryOperator::EQUAL:
			{
				write_body(" == ");
				break;
			}

		}
	}


};


#endif