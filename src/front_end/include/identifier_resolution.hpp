#ifndef C4C_IDENTIFIER_RESOLUTION_H
#define C4C_IDENTIFIER_RESOLUTION_H

#include <string>
#include <map>
#include "ast.hpp"

class MapEntry
{
public:
    std::string name;
    MapEntry(std::string name)
    {
        this->name = name;
    }
};


class Map
{
public:
    std::map<std::string,MapEntry> map;
    bool lookup(std::string name) 
    {
        if (this->map.find(name) == this->map.end())
        {
            return false;
        }

        return true;
    }

    void add(std::string name,MapEntry entry)
    {
        //this->map[name] = entry;
        this->map.emplace(name, entry);
    }


    std::string get_name(std::string name)
    {
        return this->map.at(name).name;
        //return this->map[name].name;
    }
};



class IdentifierResolution
{
public:
	std::string file_name;
	ASTProgram *program;
    std::string global_ident;
    int global_counter = 0;

    IdentifierResolution(std::string file_name,ASTProgram *program)
    {
        this->file_name = file_name;
        this->program = program;
        this->global_ident = "c4_tmp";

        Map g_map;
        Map *ident_map = &g_map;
        
        for (ASTDeclaration *decl : this->program->decls)
        {
            if (decl == nullptr)
            {
                continue;
            }

            resolve_decl(decl,ident_map);
        }
    }


    void resolve_decl(ASTDeclaration *decl,Map *ident_map)
    {
        switch (decl->type)
		{
			case ASTDeclarationType::FUNCTION:
			{
				resolve_function((ASTFunctionDecl *)decl->decl,ident_map);
				break;
			}
		}

    }


    void resolve_function(ASTFunctionDecl *decl,Map *ident_map)
    {
        resolve_block_stmt(decl->block,ident_map);
    }



    void resolve_block_stmt(ASTBlockStmt *block,Map *ident_map)
	{

		for (ASTStatement *stmt : block->stmts)
		{
			resolve_stmt(stmt,ident_map);
		}
	}

    void resolve_stmt(ASTStatement *stmt,Map *ident_map)
	{
		switch(stmt->type)
		{
			case ASTStatementType::RETURN:
			{
                resolve_return_stmt((ASTReturnStmt *)stmt->stmt,ident_map);
				break;
			}
            case ASTStatementType::IF:
			{
                resolve_if_stmt((ASTIfStmt *)stmt->stmt,ident_map);
				break;
			}
            case ASTStatementType::VARDECL:
			{
                resolve_vardecl_stmt((ASTVarDecl *)stmt->stmt,ident_map);
				break;
			}
			case ASTStatementType::EXPR:
			{
				resolve_expr((ASTExpression *)stmt->stmt,ident_map);
				break;
			}
		}
	}

    void resolve_vardecl_stmt(ASTVarDecl *decl,Map *ident_map)
    {
        if (ident_map->lookup(decl->ident))
        {
            fatal("redeclared variable " + decl->ident);
        }
        else
        {
            std::string tmp_name = make_tmp(decl->ident + "_");
            ident_map->add(decl->ident,MapEntry(tmp_name));

            if (decl->expr != nullptr)
            {
                DEBUG_PRINT(" inside resolve expr ", " vardecl stmt ");
                resolve_expr(decl->expr,ident_map);
            }

            DEBUG_PRINT("vardecl  " + decl->ident + "  ",tmp_name);

            decl->ident = tmp_name;
        }
    }


    

    void resolve_if_stmt(ASTIfStmt *stmt,Map *ident_map)
    {
        resolve_expr(stmt->expr,ident_map);
        resolve_block_stmt(stmt->block,ident_map);

        for(ASTIfElifBlock *elif_block : stmt->elif_blocks)
        {
            if (elif_block == nullptr)
            {
                continue;
            }

            resolve_expr(elif_block->expr,ident_map);
            resolve_block_stmt(elif_block->block,ident_map);

        }

        if(stmt->else_block != nullptr)
        {
            resolve_block_stmt(stmt->else_block->block,ident_map);
        }
    }



    void resolve_return_stmt(ASTReturnStmt *stmt,Map *ident_map)
    {
        resolve_expr(stmt->expr,ident_map);
    }

    void resolve_expr(ASTExpression *expr,Map *ident_map)
	{
		switch (expr->type)
		{
			case ASTExpressionType::ASSIGN:
			{
                ASTAssignExpr *assign_expr = (ASTAssignExpr *)expr->expr;

                if (not is_lvalue(assign_expr->lhs))
                {
                    fatal("invalid lvalue");
                }

                resolve_expr(assign_expr->lhs,ident_map);
                resolve_expr(assign_expr->rhs,ident_map);
                break;
            }
            case ASTExpressionType::VARIABLE:
			{
                ASTVariableExpr *var_expr = (ASTVariableExpr *)expr->expr;
                //fatal(" fatal  -> " + var_expr->ident);
                std::string name = var_expr->ident;
                if (ident_map->lookup(name))
                {
                    var_expr->ident = ident_map->get_name(name);
                    DEBUG_PRINT("inside lookup true : " + name + " ", " expr_var " + ident_map->get_name(name));
                }
                else
                {
                    fatal("undeclared variable  :  " + name);
                }
                break;
            }
            case ASTExpressionType::UNARY:
			{
                ASTUnaryExpr *unary_expr = (ASTUnaryExpr *)expr->expr;

                resolve_expr(unary_expr->rhs,ident_map);
                break;
            }
            case ASTExpressionType::BINARY:
			{
                ASTBinaryExpr *binary_expr = (ASTBinaryExpr *)expr->expr;

                resolve_expr(binary_expr->lhs,ident_map);
                resolve_expr(binary_expr->rhs,ident_map);
                break;
            }
        }
    }


    bool is_lvalue(ASTExpression *expr)
    {
        switch (expr->type)
		{
			case ASTExpressionType::VARIABLE:
			{
                return true;
                break;
            }
            default:
            {
                return false;
                break;
            }
        }
    }


    std::string make_tmp(std::string base = "")
	{
		return base + this->global_ident + "." + std::to_string(this->global_counter++) ; 
	}


    void fatal(std::string string)
    {
        DEBUG_PANIC(string);
    }
};



#endif