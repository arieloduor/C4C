#ifndef C4C_TYPE_CHECKING_H
#define C4C_TYPE_CHECKING_H

#include <string>
#include <map>
#include "ast.hpp"

/**

enum class DataType
{
    FUNCTION = 1,
    I32,
    I64,
};

*/


class TypeFunction
{
public:
    DataType return_type;
    int arg_count;

    TypeFunction(DataType return_type,int arg_count)
    {
        this->return_type = return_type;
        this->arg_count = arg_count;
    }

    TypeFunction() = default;
};

class Symbol
{
public:
    std::string name;
    DataType type;
    TypeFunction val;
    bool local = false;
    bool global = false;
    bool is_public = false;
    bool init = false;
    bool tentative = false;
    int int_init = 0;
    long int int64_init = 0;
    bool is_int = true;


    Symbol(std::string name,DataType type,bool local = true)
    {
        this->name = name;
        this->type = type;
        this->local = local;
    }

    void add_val(TypeFunction val)
    {
        this->val = val;
    }

    void add_global(bool global)
    {
        this->global = global;
    }


    void add_public(bool is_public)
    {
        this->is_public = is_public;
    }


    void add_tentative(bool tentative)
    {
        this->tentative = tentative;
    }


    void add_init(bool init)
    {
        this->init = init;
    }

    void add_int_init(int int_init)
    {
        this->int_init = int_init;
    }

    void add_int64_init(int int64_init)
    {
        this->int64_init = int64_init;
    }
};


class SymbolTable
{
public:
    std::map<std::string,Symbol> table;
    bool lookup(std::string name) 
    {
        if (this->table.find(name) == this->table.end())
        {
            return false;
        }

        return true;
    }

    void add(std::string name,Symbol symbol)
    {
        //this->map[name] = symbol;
        this->table.emplace(name, symbol);
    }

    Symbol get(std::string name)
    {
        return this->table.at(name);
        //return this->map[name];
    }

    std::string get_name(std::string name)
    {
        return this->table.at(name).name;
        //return this->map[name].name;
    }


    DataType get_type(std::string name)
    {
        return this->table.at(name).type;
        //return this->map[name].name;
    }

    TypeFunction get_val(std::string name)
    {
        return this->table.at(name).val;
        //return this->map[name].name;
    }

    bool get_global(std::string name)
    {
        return this->table.at(name).global;
        //return this->map[name].name;
    }
};



class TypeChecking
{
public:
	std::string file_name;
	ASTProgram *program;
    std::string global_ident;
    int global_counter = 0;
    SymbolTable table;

    TypeChecking(std::string file_name,ASTProgram *program)
    {
        this->file_name = file_name;
        this->program = program;
        
        SymbolTable *symbol_table = &table;

        for (ASTDeclaration *decl : this->program->decls)
        {
            if (decl == nullptr)
            {
                continue;
            }

            check_decl(decl,symbol_table);
        }
    }


    void check_decl(ASTDeclaration *decl,SymbolTable *symbol_table)
    {
        switch (decl->type)
		{
			case ASTDeclarationType::FUNCTION:
			{
				check_function((ASTFunctionDecl *)decl->decl,symbol_table);
				break;
			}
            case ASTDeclarationType::VARDECL:
			{
				check_global_vardecl((ASTVarDecl *)decl->decl,symbol_table);
				break;
			}
		}

    }


    void check_global_vardecl(ASTVarDecl *decl,SymbolTable *symbol_table)
    {
        
        switch (decl->type->type)
        {
            case ASTDataType::I32:
            {
                Symbol symbol(decl->ident,DataType::I32);
                symbol.add_public(decl->is_public);
                symbol.add_global(true);
                symbol.add_init(false);

                get_i32_init(decl->expr->expr);

                if (decl->expr == nullptr)
                {
                    if (decl->is_extern)
                    {
                        symbol.add_init(false);
                    }
                    else
                    {
                        symbol.add_init(true);
                        symbol.add_tentative(true);
                        symbol.add_int_init(0);
                    }
                }
                else if (decl->expr->type == ASTExpressionType::I32)
                {
                    symbol.add_init(true);
                    symbol.add_tentative(false);
                    symbol.add_int_init(get_i32_init(decl->expr->expr));
                }
                else
                {
                    fatal(" non-constant initializer used on local static variable");
                }

                bool is_public = not decl->is_static;

                if(symbol_table->lookup(decl->ident))
                {
                    if(symbol_table->get_type(decl->ident) != DataType::I32)
                    {
                        fatal("function redeclared as a variable");
                    }

                    if (decl->is_extern)
                    {
                        is_public = symbol_table->get_global(decl->ident);
                    }
                    else if ( symbol_table->get_global(decl->ident) != is_public)
                    {
                        fatal(" variable linkage conflicts with previous declaration");
                    }

                    if (symbol_table->get(decl->ident).tentative == false )
                    {
                        if (symbol.tentative == false)
                        {
                            fatal(" conflicting file scope variable definitions ");
                        }
                        else
                        {
                            symbol.add_tentative(symbol_table->get(decl->ident).tentative);
                            symbol.add_init(symbol_table->get(decl->ident).init);
                            symbol.add_int_init(symbol_table->get(decl->ident).int_init);
                        }
                    }
                    else if (symbol.tentative)
                    {
                        symbol.add_tentative(true);
                    }
                }

                symbol_table->add(decl->ident,symbol);

                decl->expr->add_data_type(DataType::I32);
                break;
            }
            case ASTDataType::I64:
            {
                Symbol symbol(decl->ident,DataType::I64);
                symbol.add_public(decl->is_public);
                symbol.add_global(true);
                symbol.add_init(false);

                get_i64_init(decl->expr->expr);

                if (decl->expr == nullptr)
                {
                    if (decl->is_extern)
                    {
                        symbol.add_init(false);
                    }
                    else
                    {
                        symbol.add_init(true);
                        symbol.add_tentative(true);
                        symbol.add_int64_init(0);
                    }
                }
                else if (decl->expr->type == ASTExpressionType::I32)
                {
                    symbol.add_init(true);
                    symbol.add_tentative(false);
                    symbol.add_int64_init(get_i64_init(decl->expr->expr));
                }
                else
                {
                    fatal(" non-constant initializer used on local static variable");
                }

                bool is_public = not decl->is_static;

                if(symbol_table->lookup(decl->ident))
                {
                    if(symbol_table->get_type(decl->ident) != DataType::I64)
                    {
                        fatal("function redeclared as a variable");
                    }

                    if (decl->is_extern)
                    {
                        is_public = symbol_table->get_global(decl->ident);
                    }
                    else if ( symbol_table->get_global(decl->ident) != is_public)
                    {
                        fatal(" variable linkage conflicts with previous declaration");
                    }

                    if (symbol_table->get(decl->ident).tentative == false )
                    {
                        if (symbol.tentative == false)
                        {
                            fatal(" conflicting file scope variable definitions ");
                        }
                        else
                        {
                            symbol.add_tentative(symbol_table->get(decl->ident).tentative);
                            symbol.add_init(symbol_table->get(decl->ident).init);
                            symbol.add_int64_init(symbol_table->get(decl->ident).int64_init);
                        }
                    }
                    else if (symbol.tentative)
                    {
                        symbol.add_tentative(true);
                    }
                }

                symbol_table->add(decl->ident,symbol);
                decl->expr->add_data_type(DataType::I64);
                break;
            }
        }
        
        
    }


    void check_function(ASTFunctionDecl *decl,SymbolTable *symbol_table)
    {
        DataType return_type;
        if (decl->return_type->type == ASTDataType::I32)
        {
            return_type = DataType::I32;
        }
        else if (decl->return_type->type == ASTDataType::I64)
        {
            return_type = DataType::I64;
        }
        else
        {
            fatal(" invalid return type in function " + decl->ident);
        }

        TypeFunction f_type(return_type,decl->arguments.size());

        if (symbol_table->lookup(decl->ident))
        {
            fatal("redeclared function " + decl->ident);
        }
        
        Symbol symbol(decl->ident,DataType::FUNCTION);
        symbol.add_val(f_type);
        symbol.add_global(decl->is_public);

        symbol_table->add(decl->ident,symbol);

        for (ASTFunctionArgument *arg : decl->arguments)
        {
            if (arg == nullptr)
            {
                continue;
            }
            symbol_table->add(arg->ident,Symbol(arg->ident,DataType::I32));

        }

        if (decl->block != nullptr)
        {
            check_block_stmt(decl->block,symbol_table,return_type);
        }
    }



    void check_block_stmt(ASTBlockStmt *block,SymbolTable *symbol_table,DataType return_type)
	{
		for (ASTStatement *stmt : block->stmts)
		{
			check_stmt(stmt,symbol_table,return_type);
		}
	}

    void check_stmt(ASTStatement *stmt,SymbolTable *symbol_table,DataType return_type)
	{
		switch(stmt->type)
		{
			case ASTStatementType::RETURN:
			{
                check_return_stmt((ASTReturnStmt *)stmt->stmt,symbol_table,return_type);
				break;
			}
            case ASTStatementType::IF:
			{
                check_if_stmt((ASTIfStmt *)stmt->stmt,symbol_table,return_type);
				break;
			}
            case ASTStatementType::WHILE:
			{
                check_while_stmt((ASTWhileStmt *)stmt->stmt,symbol_table,return_type);
				break;
			}
            case ASTStatementType::VARDECL:
			{
                check_vardecl_stmt((ASTVarDecl *)stmt->stmt,symbol_table);
				break;
			}
			case ASTStatementType::EXPR:
			{
				check_expr((ASTExpression *)stmt->stmt,symbol_table);
				break;
			}
		}
	}

    int get_i32_init(void *expr)
    {
        ASTI32Expr *i32_expr = (ASTI32Expr *)expr;
        return i32_expr->value; 
    }


    int get_i64_init(void *expr)
    {
        ASTI64Expr *i64_expr = (ASTI64Expr *)expr;
        return i64_expr->value; 
    }


    void check_vardecl_stmt(ASTVarDecl *decl,SymbolTable *symbol_table)
    {
        switch(decl->type->type)
        {
            case ASTDataType::I32:
            {
                DEBUG_PRINT(" vardecl =>  "," i32 ");
                break;
            }
            case ASTDataType::I64:
            {
                DEBUG_PRINT(" vardecl =>  "," i64 ");
                break;
            }
            default:
            {
                DEBUG_PANIC(" the gods have spoken!!");
                break;
            }
        }

        switch (decl->type->type)
        {
            case ASTDataType::I32:
            {
                if (decl->is_extern)
                {
                    if(decl->expr != nullptr)
                    {
                        fatal(" local extern variable declared with an initializer is illegal");
                    }

                    if(symbol_table->lookup(decl->ident))
                    {
                        if(symbol_table->get_type(decl->ident) != DataType::I32)
                        {
                            fatal("function redeclared as a variable");
                        }
                    }
                    else
                    {
                        Symbol symbol(decl->ident,DataType::I32);
                        symbol.add_global(true);
                        symbol.add_init(false);
                        symbol_table->add(decl->ident,symbol);
                    }
                }
                else if(decl->is_static)
                {
                    Symbol symbol(decl->ident,DataType::I32);
                    symbol.add_global(false);
                    symbol.add_init(false);

                    if (decl->expr == nullptr)
                    {
                        symbol.add_init(true);
                        symbol.add_int_init(0);
                    }
                    else if (decl->expr->type == ASTExpressionType::I32)
                    {
                        symbol.add_init(true);
                        symbol.add_int_init(get_i32_init(decl->expr->expr));
                    }
                    else
                    {
                        fatal(" non-constant initializer used on local static variable");
                    }

                    symbol_table->add(decl->ident,symbol);
                }

                else
                {
                    symbol_table->add(decl->ident,Symbol(decl->ident,DataType::I32,true));

                    if (decl->expr != nullptr)
                    {
                        check_expr(decl->expr,symbol_table);
                    }
                }
                break;
            }
            case ASTDataType::I64:
            {
                if (decl->is_extern)
                {
                    if(decl->expr != nullptr)
                    {
                        fatal(" local extern variable declared with an initializer is illegal");
                    }

                    if(symbol_table->lookup(decl->ident))
                    {
                        if(symbol_table->get_type(decl->ident) != DataType::I64)
                        {
                            fatal("function redeclared as a variable");
                        }
                    }
                    else
                    {
                        Symbol symbol(decl->ident,DataType::I64);
                        symbol.add_global(true);
                        symbol.add_init(false);
                        symbol_table->add(decl->ident,symbol);
                    }
                }
                else if(decl->is_static)
                {
                    Symbol symbol(decl->ident,DataType::I64);
                    symbol.add_global(false);
                    symbol.add_init(false);

                    if (decl->expr == nullptr)
                    {
                        symbol.add_init(true);
                        symbol.add_int_init(0);
                    }
                    else if (decl->expr->type == ASTExpressionType::I32)
                    {
                        symbol.add_init(true);
                        symbol.add_int_init(get_i32_init(decl->expr->expr));
                    }
                    else
                    {
                        fatal(" non-constant initializer used on local static variable");
                    }

                    symbol_table->add(decl->ident,symbol);
                }

                else
                {
                    symbol_table->add(decl->ident,Symbol(decl->ident,DataType::I64,true));

                    if (decl->expr != nullptr)
                    {
                        check_expr(decl->expr,symbol_table);
                    }
                }
                break;
            }
            default:
            {
                DEBUG_PANIC("unsupported type in vardecl ");
                break;
            }
        }
    }



    void check_while_stmt(ASTWhileStmt *stmt,SymbolTable *symbol_table,DataType return_type)
    {
        check_expr(stmt->expr,symbol_table);
        check_block_stmt(stmt->block,symbol_table,return_type);
    }

    

    void check_if_stmt(ASTIfStmt *stmt,SymbolTable *symbol_table,DataType return_type)
    {
        check_expr(stmt->expr,symbol_table);
        check_block_stmt(stmt->block,symbol_table,return_type);

        for(ASTIfElifBlock *elif_block : stmt->elif_blocks)
        {
            if (elif_block == nullptr)
            {
                continue;
            }

            check_expr(elif_block->expr,symbol_table);
            check_block_stmt(elif_block->block,symbol_table,return_type);

        }

        if(stmt->else_block != nullptr)
        {
            check_block_stmt(stmt->else_block->block,symbol_table,return_type);
        }
    }



    void check_return_stmt(ASTReturnStmt *stmt,SymbolTable *symbol_table,DataType return_type)
    {
        check_expr(stmt->expr,symbol_table);

        if(stmt->expr->data_type != return_type)
        {
            fatal("returned value data type conflicts with the function's data type");
        }
    }

    void check_expr(ASTExpression *expr,SymbolTable *symbol_table)
	{
		switch (expr->type)
		{
            case ASTExpressionType::I32:
            {
                ASTI32Expr *i32_expr = (ASTI32Expr *)expr->expr;
                i32_expr->add_data_type(DataType::I32);
                expr->add_data_type(i32_expr->data_type);
                break;
            }
            case ASTExpressionType::I64:
            {
                ASTI64Expr *i64_expr = (ASTI64Expr *)expr->expr;
                i64_expr->add_data_type(DataType::I64);
                expr->add_data_type(i64_expr->data_type);
                break;
            }
            case ASTExpressionType::CAST:
            {
                ASTCastExpr *cast_expr = (ASTCastExpr *)expr->expr;
                check_expr(cast_expr->rhs,symbol_table);

                switch (cast_expr->type)
                {
                    case ASTDataType::I32:
                    {
                        cast_expr->add_data_type(DataType::I32);
                        break;
                    }
                    case ASTDataType::I64:
                    {
                        cast_expr->add_data_type(DataType::I64);
                        break;
                    }
                    default:
                    {
                        fatal(" cast to unsupported type");
                        break;
                    }
                }
                
                expr->add_data_type(cast_expr->data_type);
                break;
            }
			case ASTExpressionType::ASSIGN:
			{
                ASTAssignExpr *assign_expr = (ASTAssignExpr *)expr->expr;

                check_expr(assign_expr->lhs,symbol_table);
                check_expr(assign_expr->rhs,symbol_table);

                switch (assign_expr->lhs->data_type)
                {
                    case DataType::I32:
                    {
                        if(assign_expr->rhs->data_type == DataType::I64)
                        {
                            fatal(" i64 used with an i32 => perform cast for this to compile");
                        }
                        else
                        {
                            assign_expr->add_data_type(DataType::I32);
                        }
                        break;
                    }
                    case DataType::I64:
                    {
                        if(assign_expr->rhs->data_type == DataType::I32)
                        {
                            fatal(" i32 used with an i64 => perform cast for this to compile");
                        }
                        else
                        {
                            assign_expr->add_data_type(DataType::I64);
                        }
                        break;
                    }
                    default:
                    {
                        fatal("unsupported datatype encountered");
                        break;
                    }
                }
                
                expr->add_data_type(assign_expr->lhs->data_type);

                break;
            }
            case ASTExpressionType::VARIABLE:
			{
                ASTVariableExpr *var_expr = (ASTVariableExpr *)expr->expr;
                //fatal(" fatal  -> " + var_expr->ident);
                std::string name = var_expr->ident;
                
                if (symbol_table->lookup(name)  and symbol_table->get_type(name) != DataType::I32  and symbol_table->get_type(name) != DataType::I64)
                {
                    fatal("function used as variable");    
                }
                

                var_expr->add_data_type(symbol_table->get_type(name));
                expr->add_data_type(var_expr->data_type);
                break;
            }
            case ASTExpressionType::FUNCTION_CALL:
			{
                ASTFunctionCallExpr *fn_expr = (ASTFunctionCallExpr *)expr->expr;
                //fatal(" fatal  -> " + var_expr->ident);
                std::string name = fn_expr->ident;
                DataType t_type = symbol_table->get_type(name);
                TypeFunction f_type = symbol_table->get_val(name);
                fn_expr->add_data_type(t_type);
                expr->add_data_type(t_type);

                if (t_type == DataType::I32 or t_type == DataType::I64)
                {
                    fatal(" variable used as function name ");
                }

                if (f_type.arg_count != fn_expr->args.size())
                {
                    fatal(" function call with invalid number of arguments");
                }


                for (ASTExpression *arg : fn_expr->args)
                {
                    if (arg == nullptr)
                    {
                        continue;
                    }

                    check_expr(arg,symbol_table);
                }

                break;
            }
            case ASTExpressionType::UNARY:
			{
                ASTUnaryExpr *unary_expr = (ASTUnaryExpr *)expr->expr;

                check_expr(unary_expr->rhs,symbol_table);
                
                switch(unary_expr->op)
                {
                    case ASTUnaryOperator::COMPLEMENT:
                    {
                        unary_expr->add_data_type(DataType::I32);
                        break;
                    }
                    default:
                    {
                        unary_expr->add_data_type(unary_expr->rhs->data_type);
                        break;
                    }
                }

                expr->add_data_type(unary_expr->data_type);

                break;
            }
            case ASTExpressionType::BINARY:
			{
                ASTBinaryExpr *binary_expr = (ASTBinaryExpr *)expr->expr;

                check_expr(binary_expr->lhs,symbol_table);
                check_expr(binary_expr->rhs,symbol_table);

                switch (binary_expr->op)
                {
                    case ASTBinaryOperator::AND:
                    case ASTBinaryOperator::OR:
                    {
                        binary_expr->add_data_type(DataType::I32);
                        break;
                    }
                    default:
                    {
                        switch (binary_expr->lhs->data_type)
                        {
                            case DataType::I32:
                            {
                                if(binary_expr->rhs->data_type == DataType::I64)
                                {
                                    fatal(" i64 used with an i32 => perform cast for this to compile");
                                }
                                else
                                {
                                    binary_expr->add_data_type(DataType::I32);
                                }
                                break;
                            }
                            case DataType::I64:
                            {
                                if(binary_expr->rhs->data_type == DataType::I32)
                                {
                                    fatal(" i32 used with an i64 => perform cast for this to compile");
                                }
                                else
                                {
                                    binary_expr->add_data_type(DataType::I64);
                                }
                                break;
                            }
                            default:
                            {
                                fatal("unsupported datatype encountered");
                                break;
                            }
                        }
                        break;
                    }
                }

                expr->add_data_type(binary_expr->lhs->data_type);
                break;
            }
        }
    }


    void fatal(std::string string)
    {
        DEBUG_PANIC(string);
    }
};



#endif