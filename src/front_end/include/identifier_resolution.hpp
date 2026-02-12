#ifndef C4C_IDENTIFIER_RESOLUTION_H
#define C4C_IDENTIFIER_RESOLUTION_H

#include <string>
#include <map>
#include "ast.hpp"


class MapEntry
{
public:
    std::string name;
    bool current = true;
    bool linkage = false;
    MapEntry(std::string name,bool current,bool linkage = false)
    {
        this->name = name;
        this->current = current;
        this->linkage = linkage;
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

    MapEntry get(std::string name)
    {
        return this->map.at(name);
        //return this->map[name].name;
    }

    std::string get_name(std::string name)
    {
        return this->map.at(name).name;
        //return this->map[name].name;
    }


    bool get_current(std::string name)
    {
        return this->map.at(name).current;
        //return this->map[name].name;
    }

    bool get_linkage(std::string name)
    {
        return this->map.at(name).current;
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
            case ASTDeclarationType::ENUM:
			{
				resolve_enum_decl((ASTEnumDecl *)decl->decl,ident_map);
				break;
			}
			case ASTDeclarationType::FUNCTION:
			{
				resolve_function((ASTFunctionDecl *)decl->decl,ident_map);
				break;
			}
            case ASTDeclarationType::VARDECL:
			{
				resolve_global_vardecl((ASTVarDecl *)decl->decl,ident_map);
				break;
			}
		}

    }


    void resolve_global_vardecl(ASTVarDecl *decl,Map *ident_map)
    {
        std::string tmp_name = decl->ident;
        ident_map->add(decl->ident,MapEntry(tmp_name,true,true));
    }


    Map copy_ident_map(Map *ident_map)
    {
        Map new_ident_map = *ident_map;

        for (auto it = new_ident_map.map.begin(); it != new_ident_map.map.end(); ++it)
        {
            it->second.current = false;   
        }

        return new_ident_map;
    }


    void resolve_enum_decl(ASTEnumDecl *decl,Map *ident_map)
    {
        if (ident_map->lookup(decl->ident) and ident_map->get_current(decl->ident))
        {
            fatal("redeclared enum " + decl->ident);
        }

        std::string enum_ident = decl->ident;

        ident_map->add(decl->ident,MapEntry(decl->ident,true,true));

        for(ASTEnumConstant *enum_constant : decl->constants)
        {
            if(enum_constant == nullptr)
			{
				continue;
			}

            std::string ident = enum_constant->ident;
            std::string constant_ident = "__C4_" + enum_ident + "_" + ident;
            if (ident_map->lookup(constant_ident) and ident_map->get_current(constant_ident))
            {
                fatal("redeclared enum constant " + ident + " in enum `" + enum_ident + "`");
            }
            
            ident_map->add(decl->ident,MapEntry(constant_ident,true,true));
            enum_constant->ident = constant_ident;
        }

    }
    

    void resolve_function(ASTFunctionDecl *decl,Map *ident_map)
    {
        if (ident_map->lookup(decl->ident) and ident_map->get_current(decl->ident))
        {
            fatal("redeclared function " + decl->ident);
        }
        
        ident_map->add(decl->ident,MapEntry(decl->ident,true,true));

        Map new_ident_map = copy_ident_map(ident_map);


        for (ASTFunctionArgument *arg : decl->arguments)
        {
            if (arg == nullptr)
            {
                continue;
            }

            resolve_function_argument(arg,&new_ident_map);
        }

        if (decl->block != nullptr)
        {
            resolve_block_stmt(decl->block,&new_ident_map);
        }
    }


    void resolve_function_argument(ASTFunctionArgument*arg,Map *ident_map)
    {
        if (ident_map->lookup(arg->ident) and ident_map->get_current(arg->ident))
        {
            fatal("redeclared function argument " + arg->ident);
        }
        else
        {
            std::string tmp_name = make_tmp(arg->ident + "_");
            ident_map->add(arg->ident,MapEntry(tmp_name,true));
            arg->ident = tmp_name;
        }
    }


    void resolve_block_stmt(ASTBlockStmt *block,Map *ident_map)
	{
        Map new_ident_map = copy_ident_map(ident_map);
		for (ASTStatement *stmt : block->stmts)
		{
			resolve_stmt(stmt,&new_ident_map);
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
            case ASTStatementType::WHILE:
			{
                resolve_while_stmt((ASTWhileStmt *)stmt->stmt,ident_map);
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
            MapEntry prev_entry = ident_map->get(decl->ident);
            if (ident_map->get_current(decl->ident))
            {
                if ( not (ident_map->get_linkage(decl->ident) and decl->is_extern))
                {
                    fatal(" local declarations conflict");
                }
            }
        }


        if (decl->is_extern)
        {
            std::string tmp_name = decl->ident;
            ident_map->add(decl->ident,MapEntry(tmp_name,true,true));
        }
        else
        {
            std::string tmp_name = make_tmp(decl->ident + "_");
            ident_map->add(decl->ident,MapEntry(tmp_name,true));

            if (decl->expr != nullptr)
            {
                resolve_expr(decl->expr,ident_map);
            }

            decl->ident = tmp_name;
        }
    }



    void resolve_while_stmt(ASTWhileStmt *stmt,Map *ident_map)
    {
        resolve_expr(stmt->expr,ident_map);
        resolve_block_stmt(stmt->block,ident_map);
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
            case ASTExpressionType::ADDRESS_OF:
            {
                ASTAddressOfExpr *address_of_expr = (ASTAddressOfExpr *)expr->expr;
                resolve_expr(address_of_expr->expr,ident_map);
                break;
            }
            case ASTExpressionType::PTR_READ:
            {
                ASTPtrReadExpr *ptr_read = (ASTPtrReadExpr *)expr->expr;
                resolve_expr(ptr_read->expr,ident_map);
                break;
            }
            case ASTExpressionType::PTR_WRITE:
            {
                ASTPtrWriteExpr *ptr_write = (ASTPtrWriteExpr *)expr->expr;
                resolve_expr(ptr_write->expr,ident_map);
                resolve_expr(ptr_write->data,ident_map);
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
                }
                else
                {
                    fatal("undeclared variable  :  " + name);
                }
                break;
            }
            case ASTExpressionType::FUNCTION_CALL:
			{
                ASTFunctionCallExpr *fn_expr = (ASTFunctionCallExpr *)expr->expr;
                //fatal(" fatal  -> " + var_expr->ident);
                std::string name = fn_expr->ident;
                if (ident_map->lookup(name))
                {
                    fn_expr->ident = ident_map->get_name(name);

                    for (ASTExpression *arg : fn_expr->args)
                    {
                        if (arg == nullptr)
                        {
                            continue;
                        }

                        resolve_expr(arg,ident_map);

                    }
                    
                }
                else
                {
                    fatal("undeclared function called  :  " + name);
                }
                break;
            }
            case ASTExpressionType::UNARY:
			{
                ASTUnaryExpr *unary_expr = (ASTUnaryExpr *)expr->expr;

                resolve_expr(unary_expr->rhs,ident_map);
                break;
            }
            case ASTExpressionType::CAST:
			{
                ASTCastExpr *cast_expr = (ASTCastExpr *)expr->expr;

                resolve_expr(cast_expr->rhs,ident_map);
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
		return "__c4internal_" + base + this->global_ident + "_" + std::to_string(this->global_counter++) ; 
	}


    void fatal(std::string string)
    {
        DEBUG_PANIC(string);
    }



};



#endif