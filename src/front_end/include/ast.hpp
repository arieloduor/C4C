#ifndef C4C_AST_H
#define C4C_AST_H

#include "lexer.hpp"
#include "type_checking_datatypes.hpp"



class ASTProgram;
enum class ASTDeclarationType;
class ASTDeclaration;
enum class ASTDataType;
class ASTType;
class ASTMethodDecl;
class ASTFunctionArgument;
class ASTFunctionDecl;
class ASTNativeDecl;
class ASTFunctionDeclNative;
enum class ASTStatementType;
class ASTStatement;
class ASTBlockStmt;
class ASTVarDecl;
class ASTReturnStmt;
class ASTWhileStmt;
class ASTBreakStmt;
class ASTContinueStmt;
class ASTIfStmt;
class ASTIfElifBlock;
class ASTIfElseBlock;
enum class ASTExpressionType;
class ASTExpression;
class ASTUnaryExpr;
enum class ASTPrecedence;
class ASTBinaryExpr;
class ASTAssignExpr;
class ASTFunctionCallExpr;
class ASTCastExpr;



class ASTProgram
{
public:
	std::vector<ASTDeclaration *> decls;
	inline void add_decl(ASTDeclaration *decl)
	{
		this->decls.push_back(decl);
	}


	~ASTProgram()
	{
		this->decls.~vector();
	}

};

enum class ASTDeclarationType
{
	VARDECL,
	FUNCTION,
	NATIVE,
	ENUM,
	STRUCT,
	IMPL,
};


class ASTDeclaration
{
public:
	ASTDeclarationType type;
	void *decl;

	ASTDeclaration(ASTDeclarationType type,void *decl)
	{
		this->type = type;
		this->decl = decl;
	}
};


class ASTEnumConstant
{
public:
	std::string ident;
	int value;
	bool has_value = false;

	ASTEnumConstant(std::string ident,int value,bool has_value)
	{
		this->ident = ident;
		this->value = value;
		this->has_value = has_value;
	}
};



class ASTEnumDecl
{
public:
	std::string ident;
	std::vector<ASTEnumConstant *>constants;
	bool is_public = false;
	
	void add_constant(ASTEnumConstant *constant)
	{
		this->constants.push_back(constant);
	}

	void add_ident(std::string ident)
	{
		this->ident = ident;
	}

	void add_public(bool is_public)
	{
		this->is_public = is_public;
	}
};



class ASTMethodDecl
{
public:
	bool is_public;
	ASTType *return_type;
	std::string ident;
	bool constructor = false;
	//ASTBox box;
	std::vector<ASTFunctionArgument *>arguments;
	ASTBlockStmt *block;

	ASTMethodDecl()
	{
		this->is_public = false;
		this->return_type = nullptr;
		this->block = nullptr;
	}

	inline void add_constructor(bool constructor)
	{
		this->constructor = constructor;
	}

	inline void add_public(int is_public)
	{
		this->is_public = is_public;
	}

	inline void add_return_type(ASTType *return_type)
	{
		this->return_type = return_type;
	}
	
	inline void add_ident(std::string ident)
	{
		this->ident = ident;
	}

	inline void add_argument(ASTFunctionArgument *argument)
	{
		this->arguments.push_back(argument);
	}
	
	inline void add_block(ASTBlockStmt *block)
	{
		this->block = block;
	}
};



class ASTStructProperty
{
public:
	ASTType *type;
	std::string ident;

	ASTStructProperty(ASTType *type,std::string ident)
	{
		this->type = type;
		this->ident = ident;
	}
};


class ASTStructDecl
{
public:
	bool is_public;
	std::string ident;
	std::vector<ASTStructProperty *>properties;
	std::vector<ASTMethodDecl *> methods;

	ASTStructDecl()
	{
		this->is_public = false;
	}

	void add_public(bool is_public)
	{
		this->is_public = is_public;
	}

	void add_ident(std::string ident)
	{
		this->ident = ident;
	}

	void add_property(ASTStructProperty *property)
	{
		this->properties.push_back(property);
	}


	void add_method(ASTMethodDecl *method)
	{
		this->methods.push_back(method);
	}
};


class ASTImplDecl
{
public:
	std::string ident;
	std::vector<ASTMethodDecl *> methods;


	void add_ident(std::string ident)
	{
		this->ident = ident;
	}

	void add_method(ASTMethodDecl *method)
	{
		this->methods.push_back(method);
	}
};


class ASTBox
{
public:
	int row;
	int col;
	int start;
	int end;

	ASTBox(int row,int col,int start,int end)
	{
		this->row = row;
		this->col = col;
		this->start = start;
		this->end = end;
	}
};

enum class ASTDataType
{
	CHAR,
	I32,
	I64,
	U32,
	U64,
	F32,
	F64,
	STRUCT,
	ENUM,
};


class ASTType
{
public:
	ASTDataType type;
	std::string ident;
	int ptr;
	std::vector<ASTExpression *> array;
	//ASTBox box;
	ASTType()
	{
		this->ptr = 0;
	}

	inline void add_type(ASTDataType type)
	{
		this->type = type;
	}
	
	inline void add_ptr(int ptr)
	{
		this->ptr += ptr;
	}

	inline void add_array(ASTExpression *array)
	{
		this->array.push_back(array);
	}

	void add_ident(std::string ident)
	{
		this->ident = ident;
	}

	~ASTType()
	{
		this->array.~vector();
	}

};


class ASTFunctionArgument
{
public:
	ASTType *type;
	std::string ident;
	//ASTBox box;

	ASTFunctionArgument(ASTType *type,std::string ident)
	{
		this->type = type;
		this->ident = ident;
	}
};




class ASTFunctionDecl
{
public:
	bool is_public;
	ASTType *return_type;
	std::string ident;
	//ASTBox box;
	std::vector<ASTFunctionArgument *>arguments;
	ASTBlockStmt *block;

	ASTFunctionDecl()
	{
		this->is_public = false;
		this->return_type = nullptr;
		this->block = nullptr;
	}

	inline void add_public(int is_public)
	{
		this->is_public = is_public;
	}

	inline void add_return_type(ASTType *return_type)
	{
		this->return_type = return_type;
	}
	
	inline void add_ident(std::string ident)
	{
		this->ident = ident;
	}

	inline void add_argument(ASTFunctionArgument *argument)
	{
		this->arguments.push_back(argument);
	}
	
	inline void add_block(ASTBlockStmt *block)
	{
		this->block = block;
	}
};



class ASTNativeDecl
{
public:
	std::vector<ASTFunctionDeclNative *> functions;

	inline void add_function(ASTFunctionDeclNative *function)
	{
		this->functions.push_back(function);
	}
};


class ASTFunctionDeclNative
{
public:
	ASTType *return_type;
	std::string ident;
	std::vector<ASTFunctionArgument *>arguments;

	ASTFunctionDeclNative()
	{
		this->return_type = nullptr;
	}

	inline void add_return_type(ASTType *return_type)
	{
		this->return_type = return_type;
	}
	
	inline void add_ident(std::string ident)
	{
		this->ident = ident;
	}

	inline void add_argument(ASTFunctionArgument *argument)
	{
		this->arguments.push_back(argument);
	}
	
};


enum class ASTStatementType
{
	RETURN,
	VARDECL,
	EXPR,
	IF,
	WHILE,
	LOOP,
	BREAK,
	CONTINUE,
};


class ASTStatement
{
public:
	ASTStatementType type;
	void *stmt;
	ASTStatement(ASTStatementType type,void *stmt)
	{
		this->type = type;
		this->stmt = stmt;
	}
};



class ASTBlockStmt
{
public:
	std::vector<ASTStatement *> stmts;

	inline void add_stmt(ASTStatement *stmt)
	{
		this->stmts.push_back(stmt);
	}
};


class ASTVarSingleInit
{
public:
	ASTExpression *expr;

	ASTVarSingleInit(ASTExpression *expr)
	{
		this->expr = expr;
	}
};


class ASTVarStructMember
{
public:
    std::map<std::string,ASTExpression *> table;
    bool lookup(std::string name) 
    {
        if (this->table.find(name) == this->table.end())
        {
            return false;
        }

        return true;
    }

    void add(std::string name,ASTExpression *expr)
    {
        //this->map[name] = symbol;
        this->table.emplace(name, expr);
    }

    ASTExpression *get(std::string name)
    {
        return this->table.at(name);
        //return this->map[name];
    }

};




class ASTVarStructInit
{
public:
	std::string ident;
	ASTVarStructMember members;
	void add_member(std::string member,ASTExpression *expr)
	{
		this->members.add(member,expr);
	}

	void add_ident(std::string ident)
	{
		this->ident = ident;
	}
};


enum class ASTVarInitType
{
	SINGLE,
	STRUCT,
};


class ASTVarInit
{
public:
	ASTVarInitType type;
	void *init;

	ASTVarInit(ASTVarInitType type,void *init)
	{
		this->type = type;
		this->init = init;
	}
};



class ASTVarDecl
{
public:
	ASTType *type;
	std::string ident;
	std::string true_ident;
	ASTVarInit *init;
	bool is_public = false;
	bool is_static = false;
	bool is_extern = false;
	//ASTBox box;

	ASTVarDecl(ASTType *type,std::string ident,ASTVarInit *init,bool is_public=false,bool is_static=false,bool is_extern = false)
	{
		this->type = type;
		this->ident = this->true_ident = ident;
		this->init = init;
		this->is_public = is_public;
		this->is_static = is_static;
		this->is_extern = is_extern;
	}


};



class ASTReturnStmt
{
public:
	ASTExpression *expr;
	ASTReturnStmt(ASTExpression *expr)
	{
		this->expr = expr;
	}
};


class ASTWhileStmt
{
public:
	ASTExpression *expr;
	ASTBlockStmt *block;
	std::string label;

	ASTWhileStmt()
	{
		this->expr = nullptr;
		this->block = nullptr;
	}

	inline void add_expr(ASTExpression *expr)
	{
		this->expr = expr;
	}

	inline void add_block(ASTBlockStmt *block)
	{
		this->block = block;
	}

	inline void add_label(std::string label)
	{
		this->label = label;
	}

};


class ASTBreakStmt
{
public:
	std::string label;
	
	inline void add_label(std::string label)
	{
		this->label = label;
	}

};




class ASTContinueStmt
{
public:
	std::string label;
	
	inline void add_label(std::string label)
	{
		this->label = label;
	}

};




class ASTIfStmt
{
public:
	ASTExpression *expr;
	ASTBlockStmt *block;
	std::vector<ASTIfElifBlock *> elif_blocks;
	ASTIfElseBlock *else_block;


	ASTIfStmt()
	{
		this->expr = nullptr;
		this->block = nullptr;
		this->else_block = nullptr;
	}

	inline void add_expr(ASTExpression *expr)
	{
		this->expr = expr;
	}

	inline void add_block(ASTBlockStmt *block)
	{
		this->block = block;
	}

	inline void add_elif_block(ASTIfElifBlock *elif_block)
	{
		this->elif_blocks.push_back(elif_block);
	}

	inline void add_else_block(ASTIfElseBlock *else_block)
	{
		this->else_block = else_block;
	}

};



class ASTIfElifBlock
{
public:
	ASTExpression *expr;
	ASTBlockStmt *block;

	ASTIfElifBlock(ASTExpression *expr,ASTBlockStmt *block)
	{
		this->expr = expr;
		this->block = block;
	}
};



class ASTIfElseBlock
{
public:
	ASTBlockStmt *block;

	ASTIfElseBlock(ASTBlockStmt *block)
	{
		this->block = block;
	}
};



enum class ASTExpressionType
{
	UNARY = 1,
	BINARY,
	VARIABLE,
	I32,
	ASSIGN,
	FUNCTION_CALL,
	I64,
	CAST,
	STRING,
	U32,
	U64,
	F32,
	F64,
	RESOLUTION,
	ADDRESS_OF,
	PTR_READ,
	PTR_WRITE,
	ARRAY,
	ENUM_ACCESS,
	STRUCT_ACCESS,
	STRUCT_PTR_ACCESS,
	POST_INCREMENT,
	POST_DECREMENT,
	SELF,
};


enum class ASTUnaryOperator
{
	COMPLEMENT = 1,
	NEGATE,
	POST_INCREMENT,
	POST_DECREMENT,
	None,
};


enum class ASTBinaryOperator
{
	MUL = 1,
	DIV,
	MOD,
	ADD,
	SUB,
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	AND,
	OR,
	EQUAL,
	NOT_EQUAL,
	None,
};


enum class ASTAssignOperator
{
	ASSIGN = 1,
	None,
};

class ASTExpression
{
public:
	ASTExpressionType type;
	void *expr;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}


	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTExpression(ASTExpressionType type,void *expr)
	{
		this->type = type;
		this->expr = expr;
	}
};



class ASTAddressOfExpr
{
public:
	ASTExpression *expr;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTAddressOfExpr(ASTExpression *expr)
	{
		this->expr = expr;
	}
};





class ASTPtrReadExpr
{
public:
	ASTExpression *expr;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTPtrReadExpr(ASTExpression *expr)
	{
		this->expr = expr;
	}
};



class ASTPtrWriteExpr
{
public:
	ASTExpression *expr;
	ASTExpression *data;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTPtrWriteExpr(ASTExpression *expr,ASTExpression *data)
	{
		this->expr = expr;
		this->data = data;
	}
};



class ASTArrayExpr
{
public:
	ASTExpression *expr;
	ASTExpression *index;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTArrayExpr(ASTExpression *expr,ASTExpression *index)
	{
		this->expr = expr;
		this->index = index;
	}
};



class ASTStringExpr
{
public:
	std::string value;
	DataType data_type;

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTStringExpr(std::string value)
	{
		this->value = value;
	}
};


class ASTI32Expr
{
public:
	long int value;
	DataType data_type;
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTI32Expr(int value)
	{
		this->value = value;
	}
};


class ASTI64Expr
{
public:
	long int value;
	DataType data_type;
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTI64Expr(long int value)
	{
		this->value = value;
	}
};





class ASTU32Expr
{
public:
	unsigned int value;
	DataType data_type;
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTU32Expr(unsigned int value)
	{
		this->value = value;
	}
};


class ASTU64Expr
{
public:
	unsigned long int value;
	DataType data_type;

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTU64Expr(unsigned long int value)
	{
		this->value = value;
	}
};





class ASTF32Expr
{
public:
	float value;
	DataType data_type;
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTF32Expr(float value)
	{
		this->value = value;
	}
};


class ASTF64Expr
{
public:
	double value;
	DataType data_type;

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTF64Expr(double value)
	{
		this->value = value;
	}
};




class ASTResolutionExpr
{
public:
	std::vector<std::string> idents;
	DataType data_type;

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	void add_ident(std::string ident)
	{
		this->idents.push_back(ident);
	}
};




class ASTStructAccessExpr
{
public:
	ASTExpression *base;
	std::string member;
	DataType data_type;
	AggType agg_type;
	std::string base_type;

	void add_base_type(std::string ident)
	{
		this->base_type = ident;
	}

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTStructAccessExpr(ASTExpression *base,std::string member)
	{
		this->base = base;
		this->member = member;
	}
};




class ASTStructPtrAccessExpr
{
public:
	ASTExpression *base;
	std::string member;
	DataType data_type;
	AggType agg_type;
	std::string base_type;

	void add_base_type(std::string ident)
	{
		this->base_type = ident;
	}

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTStructPtrAccessExpr(ASTExpression *base,std::string member)
	{
		this->base = base;
		this->member = member;
	}
};



class ASTEnumAccessExpr
{
public:
	std::string base;
	std::string member;
	DataType data_type;

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTEnumAccessExpr(std::string base,std::string member)
	{
		this->base = base;
		this->member = member;
	}
};



class ASTVariableExpr
{
public:
	std::string ident;
	std::string true_ident;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTVariableExpr(std::string ident)
	{
		this->ident = ident;
		this->true_ident = ident;
	}
};



class ASTSelfExpr
{
public:
	std::string ident;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTSelfExpr(std::string ident)
	{
		this->ident = ident;
	}
};


class ASTUnaryExpr
{
public:
	ASTUnaryOperator op;
	ASTExpression *rhs;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	
	ASTUnaryExpr(ASTUnaryOperator op,ASTExpression *rhs)
	{
		this->op = op;
		this->rhs = rhs;
	}

};


enum class ASTPrecedence
{
	MUL = 50,
	DIV = 50,
	MOD = 50,

	ADD = 45,
	SUB = 45,

	LESS = 35,
	LESS_EQUAL = 35,
	GREATER = 35,
	GREATER_EQUAL = 35,

	EQUAL = 30,
	NOT_EQUAL = 30,

	AND = 10,
	OR = 5,
	ASSIGN = 1,
};



class ASTBinaryExpr
{
public:
	ASTExpression *lhs;
	ASTBinaryOperator op;
	ASTExpression *rhs;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	
	ASTBinaryExpr(ASTExpression *lhs,ASTBinaryOperator op,ASTExpression *rhs)
	{
		this->lhs = lhs;
		this->op = op;
		this->rhs = rhs;
	}

};



class ASTAssignExpr
{
public:
	ASTExpression *lhs;
	ASTAssignOperator op;
	ASTExpression *rhs;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}

	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	
	ASTAssignExpr(ASTExpression *lhs,ASTAssignOperator op,ASTExpression *rhs)
	{
		this->lhs = lhs;
		this->op = op;
		this->rhs = rhs;
	}

};


class ASTFunctionCallExpr
{
public:
	ASTExpression *base;
	std::vector<ASTExpression *> args;
	DataType data_type;
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}


	inline void add_base(ASTExpression *base)
	{
		this->base = base;
	}

	inline void add_arg(ASTExpression *arg)
	{
		this->args.push_back(arg);
	}
};



class ASTCastExpr
{
public:
	ASTDataType type;
	ASTExpression *rhs;
	DataType data_type;
	PointerType ptr_type;
	AggType agg_type;

	void add_agg_type(std::string ident)
	{
		this->agg_type.set(ident);
	}

	void add_pointer_type(bool is_ptr,DataType base_type,int ptr_no)
	{
		this->ptr_type.set(is_ptr,base_type,ptr_no);
	}
	
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTCastExpr(ASTDataType type,ASTExpression *rhs)
	{
		this->type = type;
		this->rhs = rhs;
	}
};

#endif

