#ifndef C4C_AST_H
#define C4C_AST_H

#include "lexer.hpp"
#include "type_checking_datatypes.hpp"



class ASTProgram;
enum class ASTDeclarationType;
class ASTDeclaration;
enum class ASTDataType;
class ASTType;
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
	I32,
	I64,
	U32,
	U64,
};


class ASTType
{
public:
	ASTDataType type;
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



class ASTVarDecl
{
public:
	ASTType *type;
	std::string ident;
	ASTExpression *expr;
	bool is_public = false;
	bool is_static = false;
	bool is_extern = false;
	//ASTBox box;

	ASTVarDecl(ASTType *type,std::string ident,ASTExpression *expr,bool is_public=false,bool is_static=false,bool is_extern = false)
	{
		this->type = type;
		this->ident = ident;
		this->expr = expr;
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
	U32,
	U64,
};


enum class ASTUnaryOperator
{
	COMPLEMENT = 1,
	NEGATE,
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


class ASTI32Expr
{
public:
	int value;
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




class ASTVariableExpr
{
public:
	std::string ident;
	DataType data_type;
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}

	ASTVariableExpr(std::string ident)
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
	std::string ident;
	std::vector<ASTExpression *> args;
	DataType data_type;
	void add_data_type(DataType data_type)
	{
		this->data_type = data_type;
	}


	inline void add_ident(std::string ident)
	{
		this->ident = ident;
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

