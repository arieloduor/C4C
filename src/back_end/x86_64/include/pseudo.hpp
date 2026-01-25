#ifndef C4C_PSEUDO_H
#define C4C_PSEUDO_H

#include "intel64.hpp"
#include <map>

class Pseudo
{
public:
	std::string file_name;
	ASMProgram *program;

	int stack_counter = 0;
	std::map<std::string,int> table;
	Arena *arena;
	SymbolTable symbol_table;

	int offset = 0;
	Pseudo(std::string file_name,ASMProgram *program,Arena *arena,SymbolTable symbol_table)
	{
		this->file_name = file_name;
		this->program = program;
		this->arena = arena;
		this->symbol_table = symbol_table;

		for(ASMDeclaration *decl : this->program->decls)
		{
			replace_decl(decl);
		}
	}

	
	void *alloc(int size)
	{
		return this->arena->alloc(size);
	}
	void replace_decl(ASMDeclaration *decl)
	{
		switch (decl->type)
		{
			case ASMDeclarationType::FUNCTION:
			{
				replace_function((ASMFunction *)decl->decl);
				break;
			}
		}

	}


	void replace_function(ASMFunction *decl)
	{	
		this->stack_counter = 0;
		for (ASMInstruction *inst : decl->instructions)
		{
			replace_instruction(inst);
		}
		decl->add_stack_counter(this->stack_counter);
	}


	void replace_instruction(ASMInstruction *inst)
	{
		switch(inst->type)
		{
			case ASMInstructionType::RET:
			{
				replace_return_inst((ASMRetInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::MOV:
			{
				replace_mov_inst((ASMMovInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::ADD:
			{
				replace_add_inst((ASMAddInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::SUB:
			{
				replace_sub_inst((ASMSubInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::CMP:
			{
				replace_cmp_inst((ASMCmpInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::SET_COND:
			{
				replace_setcond_inst((ASMSetCondInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::NEG:
			{
				replace_neg_inst((ASMNegInst *)inst->instruction);
				break;
			}
		}
	}


	void replace_return_inst(ASMRetInst *inst)
	{

	}
	
	void replace_mov_inst(ASMMovInst *inst)
	{
		replace_operand(inst->dst);
		replace_operand(inst->src);
	}
	
	void replace_add_inst(ASMAddInst *inst)
	{
		replace_operand(inst->dst);
		replace_operand(inst->src);
	}

	void replace_sub_inst(ASMSubInst *inst)
	{
		replace_operand(inst->dst);
		replace_operand(inst->src);
	}

	void replace_cmp_inst(ASMCmpInst *inst)
	{
		replace_operand(inst->dst);
		replace_operand(inst->src);
	}

	void replace_setcond_inst(ASMSetCondInst *inst)
	{
		replace_operand(inst->dst);
	}

	void replace_neg_inst(ASMNegInst *inst)
	{
		replace_operand(inst->dst);
	}
	


	void replace_operand(ASMOperand *operand)
	{
		switch (operand->type)
		{
			case ASMOperandType::PSEUDO:
			{
				ASMPseudo *asm_pseudo = (ASMPseudo *)operand->operand;
				std::string key = asm_pseudo->ident;
				ASMType type = asm_pseudo->data_type;
				
				if (this->table.find(key) == this->table.end())
				{
					std::cout << " key : " << key<< std::endl;
					if(this->symbol_table.lookup(key) and this->symbol_table.get(key).global)
					{
						switch(type)
						{
							case ASMType::I32:
							{
								void *mem = alloc(sizeof(ASMData));
								ASMData *asm_data = new(mem) ASMData(4,key);
								operand->type = ASMOperandType::DATA;
								operand->operand = asm_data;
								break;
							}
							case ASMType::I64:
							{
								void *mem = alloc(sizeof(ASMData));
								ASMData *asm_data = new(mem) ASMData(8,key);
								operand->type = ASMOperandType::DATA;
								operand->operand = asm_data;
								break;
							}
						}
					}
					else
					{
						switch(type)
						{
							case ASMType::I32:
							{
								void *mem = alloc(sizeof(ASMStack));
								this->stack_counter += 1 + offset;
								this->table[key] = this->stack_counter;
								ASMStack *asm_stack = new(mem) ASMStack(4,"rbp",this->stack_counter * 4);
								operand->type = ASMOperandType::STACK;
								operand->operand = asm_stack;
								offset = 0;
								break;
							}
							case ASMType::I64:
							{
								void *mem = alloc(sizeof(ASMStack));
								this->stack_counter += 1 + offset;
								this->table[key] = this->stack_counter;
								ASMStack *asm_stack = new(mem) ASMStack(8,"rbp",this->stack_counter * 8);
								operand->type = ASMOperandType::STACK;
								operand->operand = asm_stack;
								offset = 1;
								break;
							}
						}
					}
				}
				else
				{
					switch (type)
					{
						case ASMType::I32:
						{
							void *mem = alloc(sizeof(ASMStack));
							int tmp_stack_counter = this->table[key];
							ASMStack *asm_stack = new(mem) ASMStack(4,"rbp",tmp_stack_counter * 4);
							operand->type = ASMOperandType::STACK;
							operand->operand = asm_stack;
							break;
						}
						case ASMType::I64:
						{
							void *mem = alloc(sizeof(ASMStack));
							int tmp_stack_counter = this->table[key];
							ASMStack *asm_stack = new(mem) ASMStack(8,"rbp",tmp_stack_counter * 8);
							operand->type = ASMOperandType::STACK;
							operand->operand = asm_stack;
							break;
						}
					}
				}
			}
		}

		return;
	}

};



#endif
