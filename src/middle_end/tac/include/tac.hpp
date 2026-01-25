#ifndef C4C_TAC_H
#define C4C_TAC_H

#include <string>
#include <vector>

class TACProgram;
class TACDeclaration;
class TACFunction;
class TACInstruction;
class TACValue;




enum class TACType
{
	I32 = 1,
	I64,
};



class TACProgram
{
public:
	std::vector<TACDeclaration *> decls;

	void add_decl(TACDeclaration *decl)
	{
		this->decls.push_back(decl);
	}
};


enum class TACDeclarationType
{
	FUNCTION,
	VARDECL,
};

class TACDeclaration
{
public:
	TACDeclarationType type;
	void *decl;

	TACDeclaration(TACDeclarationType type,void *decl)
	{
		this->type = type;
		this->decl = decl;
	}
};


class TACArgument
{
public:
	std::string ident;
	TACType type;

	TACArgument(std::string ident,TACType type)
	{
		this->ident = ident;
		this->type = type;
	}
};

class TACFunction
{
public:
	bool is_public;
	std::string ident;
	std::vector<TACInstruction *>instructions;
	std::vector<TACArgument> arguments;


	TACFunction(bool is_public,std::string ident)
	{
		this->is_public = is_public;
		this->ident = ident;
	}

	void add_instruction(TACInstruction *inst)
	{
		this->instructions.push_back(inst);
	}

	void add_argument(TACArgument argument)
	{
		this->arguments.push_back(argument);
	}
};

class TACGlobalVariable
{
public:
	bool is_public;
	std::string ident;
	TACType data_type;
	void *data;
	

	TACGlobalVariable(bool is_public,std::string ident)
	{
		this->ident = ident;
		this->is_public = is_public;
	}

	void add_data(TACType data_type,void *data)
	{
		this->data_type = data_type;
		this->data = data;
	}
};




enum class TACInstructionType
{
	RETURN,
	UNARY,
	BINARY,
	JMP,
	JMP_ZERO,
	JMP_NOT_ZERO,
	LABEL,
	COPY,
	FUNCTION_CALL,
	SIGN_EXTEND,
	TRUNCATE,
};


class TACInstruction
{
public:
	TACInstructionType type;
	void *instruction;

	TACInstruction(TACInstructionType type,void *instruction)
	{
		this->type = type;
		this->instruction = instruction;
	}

};



class TACFunctionCallInst
{
public:
	std::string ident;
	std::vector<TACValue *> arguments;
	TACValue *dst;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACFunctionCallInst(std::string ident,TACValue *dst)
	{
		this->ident = ident;
		this->dst = dst;
	}

	void add_argument(TACValue *arg)
	{
		this->arguments.push_back(arg);
	}
};



class TACReturnInst
{
public:
	TACValue *value;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACReturnInst(TACValue *value)
	{
		this->value = value;
	}
};


enum class TACUnaryOperator
{
	COMPLEMENT,
	NEGATE,
	None,
};


class TACUnaryInst
{
public:
	TACValue *dst;
	TACUnaryOperator op;
	TACValue *src;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACUnaryInst(TACValue *dst,TACUnaryOperator op,TACValue *src)
	{
		this->dst = dst;
		this->op = op;
		this->src = src;
	}
};




class TACSignExtendInst
{
public:
	TACValue *dst;
	TACValue *src;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACSignExtendInst(TACValue *dst,TACValue *src)
	{
		this->dst = dst;
		this->src = src;
	}
};





class TACTruncateInst
{
public:
	TACValue *dst;
	TACValue *src;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACTruncateInst(TACValue *dst,TACValue *src)
	{
		this->dst = dst;
		this->src = src;
	}
};







enum class TACBinaryOperator
{
	MUL,
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




class TACBinaryInst
{
public:
	TACValue *dst;
	TACBinaryOperator op;
	TACValue *src1;
	TACValue *src2;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACBinaryInst(TACValue *dst,TACValue *src1,TACBinaryOperator op,TACValue *src2)
	{
		this->dst = dst;
		this->src1 = src1;
		this->op = op;
		this->src2 = src2;
	}
};


class TACCopyInst
{
public:
	TACValue *dst;
	TACValue *src;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACCopyInst(TACValue *dst,TACValue *src)
	{
		this->dst = dst;
		this->src = src;
	}
};


class TACJmpInst
{
public:
	std::string label;
	TACJmpInst(std::string label)
	{
		this->label = label;
	}
};


class TACLabelInst
{
public:
	std::string label;
	TACLabelInst(std::string label)
	{
		this->label = label;
	}
};



class TACJmpIfZeroInst
{
public:
	TACValue *value;
	std::string label;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACJmpIfZeroInst(TACValue *value,std::string label)
	{
		this->value = value;
		this->label = label;
	}
};


class TACJmpIfNotZeroInst
{
public:
	TACValue *value;
	std::string label;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}
	
	TACJmpIfNotZeroInst(TACValue *value,std::string label)
	{
		this->value = value;
		this->label = label;
	}
};


enum class TACValueType
{
	CONSTANT = 2,
	VARIABLE,
};

class TACValue
{
public:
	TACValueType type;
	void *value;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACValue(TACValueType type,void *value)
	{
		this->type = type;
		this->value = value;
	}
};



enum class TACConstantType
{
	I32,
	I64,
};

class TACConstant
{
public:
	TACConstantType type;
	void *constant;

	TACConstant(TACConstantType type,void *constant)
	{
		this->type = type;
		this->constant = constant;
	}
};


class TACVariable
{
public:
	std::string ident;
	TACType data_type;

	void add_type(TACType data_type)
	{
		this->data_type = data_type;
	}

	TACVariable(std::string ident)
	{
		this->ident = ident;
	}
};




#endif

