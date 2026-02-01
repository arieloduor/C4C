#ifndef C4C_ASM_AST_H
#define C4C_ASM_AST_H

#include <string>
#include <vector>

class ASMProgram;
class ASMDeclaration;
class ASMFunction;
class ASMInstruction;


class ASMProgram
{
public:
	std::vector<ASMDeclaration *> decls;
	void add_decl(ASMDeclaration *decl)
	{
		this->decls.push_back(decl);
	}
};


enum class ASMDeclarationType
{
	FUNCTION,
	VARDECL,
};

class ASMDeclaration
{
public:
	ASMDeclarationType type;
	void *decl;

	ASMDeclaration(ASMDeclarationType type,void *decl)
	{
		this->type = type;
		this->decl = decl;
	}
};

class ASMFunction
{
public:
	bool is_public;
	std::string ident;
	std::vector<ASMInstruction *>instructions;
	std::vector<std::string>arguments;
	int stack_counter = 0;

	ASMFunction(bool is_public,std::string ident)
	{
		this->is_public = is_public;
		this->ident = ident;
	}

	void add_instruction(ASMInstruction *inst)
	{
		this->instructions.push_back(inst);
	}

	void add_stack_counter(int stack_counter)
	{
		this->stack_counter = stack_counter;
	}
};



enum class ASMType
{
	I32,
	I64,
	U32,
	U64,
};

class ASMGlobalVariable
{
public:
	bool is_public;
	std::string ident;
	int int_init = 0;
	ASMType data_type;
	void *data;
	

	ASMGlobalVariable(bool is_public,std::string ident)
	{
		this->ident = ident;
		this->is_public = is_public;
	}

	void add_data(ASMType data_type,void *data)
	{
		this->data_type = data_type;
		this->data = data;
	}



};





class ASMInstruction
{
public:
	ASMInstructionType type;
	void *instruction;

	ASMInstruction(ASMInstructionType type,void *instruction)
	{
		this->type = type;
		this->instruction = instruction;
	}

};


#endif
