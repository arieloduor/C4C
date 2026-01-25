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
	CALL,
	MOVSX,
};

#include "../../include/asm_ast.hpp"

class ASMOperand;


enum class ASMOperandType
{
	REGISTER = 1,
	STACK,
	IMMEDIATE,
	PSEUDO,
	DATA,
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
	I64,
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
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

	ASMPseudo(std::string ident)
	{
		this->ident = ident;
	}
};




class ASMData
{
public:
	int size = 0;
	std::string address;
	
	ASMData(int size,std::string address)	
	{
		this->size = size;
		this->address = address;
	}
};



class ASMOperand
{
public:
	ASMOperandType type;
	void *operand;
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}
	
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
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

	ASMMovInst(ASMOperand *dst,ASMOperand *src)
	{
		this->dst = dst;
		this->src = src;
	}

};




class ASMMovSxInst
{
public:
	ASMOperand *dst;
	ASMOperand *src;
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

	ASMMovSxInst(ASMOperand *dst,ASMOperand *src)
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
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

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
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

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





class ASMCallInst
{
public:
	std::string label;
	ASMCallInst(std::string label)
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
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

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
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

	ASMNegInst(ASMOperand *dst)
	{
		this->dst = dst;
	}

};




class ASMNotInst
{
public:
	ASMOperand *dst;
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

	ASMNotInst(ASMOperand *dst)
	{
		this->dst = dst;
	}

};





class ASMPushInst
{
public:
	ASMOperand *dst;
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

	ASMPushInst(ASMOperand *dst)
	{
		this->dst = dst;
	}

};



class ASMPopInst
{
public:
	ASMOperand *dst;
	ASMType data_type;

	void add_type(ASMType data_type)
	{
		this->data_type = data_type;
	}

	ASMPopInst(ASMOperand *dst)
	{
		this->dst = dst;
	}

};





#endif
