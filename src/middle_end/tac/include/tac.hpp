#ifndef C4C_TAC_H
#define C4C_TAC_H

#include <string>
#include <vector>

class TACProgram;
class TACDeclaration;
class TACFunction;
class TACInstruction;
class TACValue;

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

class TACFunction
{
public:
	bool is_public;
	std::string ident;
	std::vector<TACInstruction *>instructions;

	TACFunction(bool is_public,std::string ident)
	{
		this->is_public = is_public;
		this->ident = ident;
	}

	void add_instruction(TACInstruction *inst)
	{
		this->instructions.push_back(inst);
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



class TACReturnInst
{
public:
	TACValue *value;
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

	TACUnaryInst(TACValue *dst,TACUnaryOperator op,TACValue *src)
	{
		this->dst = dst;
		this->op = op;
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

	TACValue(TACValueType type,void *value)
	{
		this->type = type;
		this->value = value;
	}
};



enum class TACConstantType
{
	I32,
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

	TACVariable(std::string ident)
	{
		this->ident = ident;
	}
};




#endif

