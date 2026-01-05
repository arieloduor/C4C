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
