#ifndef C4C_CODEGEN_H
#define C4C_CODEGEN_H

#include "intel64.hpp"

class Codegen
{
public:
	std::string file_name;
	ASMProgram *program;
	std::string string = "\n\nsection  .text\n\n";
	std::string data_string;
	bool data = false;
	
	Codegen(std::string file_name,ASMProgram *program)
	{
		this->file_name = file_name;
		this->program = program;

		for (ASMDeclaration *decl : this->program->decls)
		{
			if (decl == nullptr)
			{
				break;
			}
			gen_decl(decl);
		}
	
		this->string = this->data_string + this->string;
		//DEBUG_PRINT(" ASM \n\n",this->string);
	}

	void write_data(std::string string)
	{
		this->data_string += string;
	}

	void write_body(std::string string)
	{
		this->string += string;
	}

	void gen_decl(ASMDeclaration *decl)
	{
		switch (decl->type)
		{
			case ASMDeclarationType::FUNCTION:
			{
				gen_function((ASMFunction *)decl->decl);
				break;
			}
			case ASMDeclarationType::VARDECL:
			{
				gen_global_vardecl((ASMGlobalVariable *)decl->decl);
				break;
			}
		}

	}


	void gen_global_vardecl(ASMGlobalVariable *decl)
	{
		if (this->data == false)
		{
			write_data("\nsection .data\n");
		}

		switch (decl->data_type)
		{
			case ASMType::I32:
			{
				write_data("\t" + decl->ident + " dd " + std::to_string(*((int *)decl->data)) + "\n");
				break;
			}
			case ASMType::I64:
			{
				write_data("\t" + decl->ident + " dq " + std::to_string(*((long int *)decl->data)) + "\n");
				break;
			}
		}
	}

	void gen_function(ASMFunction *decl)
	{	
		if (decl->is_public)
		{
			gen_global(decl->ident);
		}

		write_body(decl->ident + ":\n");
		
		for (ASMInstruction *inst : decl->instructions)
		{
			if (inst == nullptr)
			{
				continue;
			}
			gen_instruction(inst);
		}
		
	}

	void gen_instruction(ASMInstruction *inst)
	{
		switch(inst->type)
		{
			case ASMInstructionType::RET:
			{
				gen_return_inst((ASMRetInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::MOV:
			{
				gen_mov_inst((ASMMovInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::SUB:
			{
				gen_sub_inst((ASMSubInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::ADD:
			{
				gen_add_inst((ASMAddInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::NEG:
			{
				gen_neg_inst((ASMNegInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::NOT:
			{
				gen_not_inst((ASMNotInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::PUSH:
			{
				gen_push_inst((ASMPushInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::POP:
			{
				gen_pop_inst((ASMPopInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::CMP:
			{
				gen_cmp_inst((ASMCmpInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::CALL:
			{
				gen_call_inst((ASMCallInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::JMP:
			{
				gen_jmp_inst((ASMJmpInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::LABEL:
			{
				gen_label((ASMLabelInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::SET_COND:
			{
				gen_setcond_inst((ASMSetCondInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::JMP_COND:
			{
				gen_jmpcond_inst((ASMJmpCondInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::MOVSX:
			{
				gen_movsx_inst((ASMMovSxInst *)inst->instruction);
				break;
			}
			default:
			{
				return;
			}
		}
	}




	
	
	void gen_global(std::string global)
	{
		write_body("global " + global + "\n");
	}


	
	void gen_label(ASMLabelInst *inst)
	{
		write_body(inst->label + ":\n");
	}



	void gen_return_inst(ASMRetInst *inst)
	{
		write_body("\tret\n");
	}
	
	void gen_mov_inst(ASMMovInst *inst)
	{
		switch(inst->dst->data_type)
		{
			case ASMType::I64:
			{
				switch(inst->dst->type)
				{
					case ASMOperandType::REGISTER:
					{
						ASMRegister *asm_reg = (ASMRegister *)inst->dst->operand;
						std::cout << "asm size :  " << asm_reg->size << std::endl;
						asm_reg->size = 8;
						break;
					}
				}
				break;
			}
			case ASMType::I32:
			{
				std::cout << "mov"<<std::endl;
				break;
			}
			default:
			{
				DEBUG_PANIC(" --------------  ");
			}
		}

		write_body("\tmov ");
		gen_operand(inst->dst);
		write_body(",");
		gen_operand(inst->src);
		write_body("\n");
		
	}
	
	void gen_movsx_inst(ASMMovSxInst *inst)
	{
		write_body("\tmovsx ");
		gen_operand(inst->dst);
		write_body(",");
		gen_operand(inst->src);
		write_body("\n");
		
	}
	

	void gen_call_inst(ASMCallInst *inst)
	{
		write_body("\tcall " + inst->label + "\n");
	}



	void gen_jmp_inst(ASMJmpInst *inst)
	{
		write_body("\tjmp " + inst->label + "\n");
	}

	void gen_sub_inst(ASMSubInst *inst)
	{
		write_body("\tsub ");
		gen_operand(inst->dst);
		write_body(",");
		gen_operand(inst->src);
		write_body("\n");
	}
	
	void gen_cmp_inst(ASMCmpInst *inst)
	{
		write_body("\tcmp ");
		gen_operand(inst->dst);
		write_body(",");
		gen_operand(inst->src);
		write_body("\n");
	}

	
	void gen_jmpcond_inst(ASMJmpCondInst *inst)
	{
		switch(inst->condition)
		{
			case ASMCondition::EQUAL:
			{
				write_body("\tje ");
				break;
			}
			case ASMCondition::NOT_EQUAL:
			{
				write_body("\tjne ");
				break;
			}
			case ASMCondition::LESS:
			{
				write_body("\tjl ");
				break;
			}
			case ASMCondition::LESS_EQUAL:
			{
				write_body("\tjle ");
				break;
			}
			case ASMCondition::GREATER:
			{
				write_body("\tjg ");
				break;
			}
			case ASMCondition::GREATER_EQUAL:
			{
				write_body("\tjge ");
				break;
			}
		}

		write_body(inst->label + "\n");
	}



	void gen_setcond_inst(ASMSetCondInst *inst)
	{
		switch(inst->condition)
		{
			case ASMCondition::LESS:
			{
				write_body("\tsetl ");
				break;
			}
			case ASMCondition::LESS_EQUAL:
			{
				write_body("\tsetle ");
				break;
			}
			case ASMCondition::GREATER:
			{
				write_body("\tsetg ");
				break;
			}
			case ASMCondition::GREATER_EQUAL:
			{
				write_body("\tsetge ");
				break;
			}
		}

		gen_operand(inst->dst,1);
		write_body("\n");
	}

	void gen_add_inst(ASMAddInst *inst)
	{
		write_body("\tadd ");
		gen_operand(inst->dst);
		write_body(",");
		gen_operand(inst->src);
		write_body("\n");
	}

	void gen_neg_inst(ASMNegInst *inst)
	{
		write_body("\tneg ");
		gen_operand(inst->dst);
		write_body("\n");
	}
	


	void gen_not_inst(ASMNotInst *inst)
	{
		write_body("\tnot ");
		gen_operand(inst->dst);
		write_body("\n");
	}
	


	void gen_push_inst(ASMPushInst *inst)
	{
		write_body("\tpush ");
		gen_operand(inst->dst);
		write_body("\n");
	}
	


	void gen_pop_inst(ASMPopInst *inst)
	{
		write_body("\tpop ");
		gen_operand(inst->dst);
		write_body("\n");
	}
	



	void gen_operand(ASMOperand *operand,int size = 0)
	{
		
		switch (operand->type)
		{
			case ASMOperandType::IMMEDIATE:
			{
				ASMImmediate *asm_imm = (ASMImmediate *)operand->operand;
				gen_immediate(asm_imm);
				break;
			}
			case ASMOperandType::REGISTER:
			{
				ASMRegister *asm_reg = (ASMRegister *)operand->operand;
				gen_register(asm_reg);
				break;
			}
			case ASMOperandType::STACK:
			{
				ASMStack *asm_stack = (ASMStack *)operand->operand;
				gen_stack(asm_stack,size);
				break;
			}
			case ASMOperandType::DATA:
			{
				ASMData *asm_data = (ASMData *)operand->operand;
				gen_data(asm_data,size);
				break;
			}
			default:
			{
			}
		}

		return;
	}


	void gen_data(ASMData *asm_data,int size = 0)
	{
		if (size == 0)
		{
			size = asm_data->size;
		}

		switch (size)
		{
			case 1:
			{
				write_body(asm_data->address);
				break;
			}
			case 4:
			{
				write_body("[rel " + asm_data->address + "]");
				break;
			}
			case 8:
			{
				write_body("[rel " + asm_data->address + "]");
				break;
			}
		}

	}



	void gen_stack(ASMStack *asm_stack,int size = 0)
	{
		if (size == 0)
		{
			size = asm_stack->size;
		}

		switch (size)
		{
			case 1:
			{
				write_body("BYTE [" + asm_stack->address + " - " + std::to_string(asm_stack->index) + "]");
				break;
			}
			case 4:
			{
				write_body("DWORD [" + asm_stack->address + " - " + std::to_string(asm_stack->index) + "]");
				break;
			}
			case 8:
			{
				write_body("QWORD [" + asm_stack->address + " - " + std::to_string(asm_stack->index) + "]");
				break;
			}
		}
	}



	void gen_register(ASMRegister *asm_reg)
	{
		switch(asm_reg->type)
		{
			case ASMRegisterType::RAX:
			{
				if (asm_reg->size == 4)
				{
					write_body("eax");
				}
				else if (asm_reg->size == 8)
				{
					write_body("rax");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
			case ASMRegisterType::RBX:
			{
				if (asm_reg->size == 4)
				{
					write_body("ebx");
				}
				else if (asm_reg->size == 8)
				{
					write_body("rbx");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
			case ASMRegisterType::RCX:
			{
				if (asm_reg->size == 4)
				{
					write_body("ecx");
				}
				else if (asm_reg->size == 8)
				{
					write_body("rcx");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
			case ASMRegisterType::RDX:
			{
				if (asm_reg->size == 4)
				{
					write_body("edx");
				}
				else if (asm_reg->size == 8)
				{
					write_body("rdx");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
			case ASMRegisterType::RDI:
			{
				if (asm_reg->size == 4)
				{
					write_body("edi");
				}
				else if (asm_reg->size == 8)
				{
					write_body("rdi");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
			case ASMRegisterType::RSI:
			{
				if (asm_reg->size == 4)
				{
					write_body("esi");
				}
				else if (asm_reg->size == 8)
				{
					write_body("rsi");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
			case ASMRegisterType::RBP:
			{
				if (asm_reg->size == 4)
				{
					write_body("ebp");
				}
				else if (asm_reg->size == 8)
				{
					write_body("rbp");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
			case ASMRegisterType::RSP:
			{
				if (asm_reg->size == 4)
				{
					write_body("esp");
				}
				else if (asm_reg->size == 8)
				{
					write_body("rsp");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
			case ASMRegisterType::R11:
			{
				if (asm_reg->size == 4)
				{
					write_body("r11d");
				}
				else if (asm_reg->size == 8)
				{
					write_body("r11");
				}
				else
				{
					std::cout << (int)asm_reg->type << std::endl;
					DEBUG_PANIC("unknown register size " + std::to_string(asm_reg->size));
				}
				
				break;
			}
		}
	}
	void gen_immediate(ASMImmediate *asm_imm)
	{
		switch(asm_imm->type)
		{
			case ASMImmediateType::I32:
			{
				gen_i32_immediate((int *)asm_imm->value);
				break;
			}
		}
	}
	void gen_i32_immediate(int *value)
	{
		write_body(std::to_string(*value));
	}
};



#endif

