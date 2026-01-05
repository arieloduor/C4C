#ifndef C4C_INTEL64_H
#define C4C_INTEL64_H



enum class ASMInstructionType
{
	MOV = 1,
	RET,
	NEG,
	PUSH,
	POP,
	ADD,
	SUB,
	NOT,
	CMP,
	SET_COND,
	JMP,
	JMP_COND,
	LABEL,
};

#include "../../include/asm_ast.hpp"

class ASMOperand;


enum class ASMOperandType
{
	REGISTER = 1,
	STACK,
	IMMEDIATE,
	PSEUDO,
};

enum class ASMRegisterType
{
	RAX = 1,
	RBX,
	RCX,
	RDX,
	RBP,
	RSP,
	RDI,
	RSI,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15,
};

class ASMRegister
{
public:
	ASMRegisterType type;
	int size;
	
	ASMRegister(ASMRegisterType type,int size)
	{
		this->type = type;
		this->size = size;
		DEBUG_PRINT("in constructor " + std::to_string(size), "  =>  " + std::to_string(this->size));
	}
};

class ASMStack
{
public:
	int size = 0;
	std::string address;
	int index = 0;	
	
	ASMStack(int size,std::string address,int index)	
	{
		this->size = size;
		this->address = address;
		this->index = index;
	}
};


enum class ASMImmediateType
{
	I32,
};

class ASMImmediate
{
public:
	ASMImmediateType type;
	void *value;
	
	ASMImmediate(ASMImmediateType type,void *value)
	{
		this->type = type;
		this->value = value;
	}
};


class ASMPseudo
{
public:
	std::string ident;	
	ASMPseudo(std::string ident)
	{
		this->ident = ident;
	}
};


class ASMOperand
{
public:
	ASMOperandType type;
	void *operand;
	
	ASMOperand(ASMOperandType type,void *operand)
	{
		this->type = type;
		this->operand = operand;
	}
};


class ASMMovInst
{
public:
	ASMOperand *dst;
	ASMOperand *src;

	ASMMovInst(ASMOperand *dst,ASMOperand *src)
	{
		this->dst = dst;
		this->src = src;
	}

};



class ASMCmpInst
{
public:
	ASMOperand *dst;
	ASMOperand *src;

	ASMCmpInst(ASMOperand *dst,ASMOperand *src)
	{
		this->dst = dst;
		this->src = src;
	}

};


class ASMSubInst
{
public:
	ASMOperand *dst;
	ASMOperand *src;

	ASMSubInst(ASMOperand *dst,ASMOperand *src)
	{
		this->dst = dst;
		this->src = src;
	}

};




class ASMLabelInst
{
public:
	std::string label;
	ASMLabelInst(std::string label)
	{
		this->label = label;
	}
};

enum class ASMCondition
{
	EQUAL,
	NOT_EQUAL,
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
};



class ASMJmpInst
{
public:
	std::string label;
	ASMJmpInst(std::string label)
	{
		this->label = label;
	}
};



class ASMJmpCondInst
{
public:
	ASMCondition condition;
	std::string label;
	ASMJmpCondInst(ASMCondition condition,std::string label)
	{
		this->condition = condition;
		this->label = label;
	}
};


class ASMSetCondInst
{
public:
	ASMCondition condition;
	ASMOperand *dst;

	ASMSetCondInst(ASMCondition condition,ASMOperand *dst)
	{
		this->condition = condition;
		this->dst = dst;
	}

};



class ASMAddInst
{
public:
	ASMOperand *dst;
	ASMOperand *src;

	ASMAddInst(ASMOperand *dst,ASMOperand *src)
	{
		this->dst = dst;
		this->src = src;
	}

};




class ASMRetInst
{
public:

};


class ASMNegInst
{
public:
	ASMOperand *dst;

	ASMNegInst(ASMOperand *dst)
	{
		this->dst = dst;
	}

};




class ASMNotInst
{
public:
	ASMOperand *dst;

	ASMNotInst(ASMOperand *dst)
	{
		this->dst = dst;
	}

};





class ASMPushInst
{
public:
	ASMOperand *dst;

	ASMPushInst(ASMOperand *dst)
	{
		this->dst = dst;
	}

};



class ASMPopInst
{
public:
	ASMOperand *dst;

	ASMPopInst(ASMOperand *dst)
	{
		this->dst = dst;
	}

};





#endif
