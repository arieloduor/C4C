#ifndef C4C_TAC_TO_X86_64_H
#define C4C_TAC_TO_X86_64_H

#include "intel64.hpp"
//#include "../../../middle_end/tac/include/tac.hpp"

class TacToIntel64
{
public:
	std::string file_name;
	TACProgram *tac_program;
	ASMProgram *program;
	std::string string;
	Arena *arena;
	std::vector<ASMInstruction *> *inst = nullptr;
	
	TacToIntel64(std::string file_name,TACProgram *tac_program,Arena *arena)
	{
		this->file_name = file_name;
		this->tac_program = tac_program;
		this->arena = arena;

		void *mem = alloc(sizeof(ASMProgram));
		this->program = new(mem) ASMProgram();

		for (TACDeclaration *decl : this->tac_program->decls)
		{
			if (decl == nullptr)
			{
				break;
			}
			ASMDeclaration *asm_decl = convert_decl(decl);
			this->program->add_decl(asm_decl);
		}

	}


	void *alloc(int size)
	{
		return this->arena->alloc(size);
	}


	ASMDeclaration *convert_decl(TACDeclaration *decl)
	{
		void *mem = alloc(sizeof(ASMDeclaration));
		ASMDeclaration *asm_decl = nullptr;

		switch (decl->type)
		{
			case TACDeclarationType::FUNCTION:
			{
				ASMFunction *asm_fn = convert_function((TACFunction *)decl->decl);
				asm_decl = new(mem) ASMDeclaration(ASMDeclarationType::FUNCTION,asm_fn);
				break;
			}
		}

		return asm_decl;
	}


	ASMFunction *convert_function(TACFunction *decl)
	{
		void *mem = alloc(sizeof(ASMFunction));
		ASMFunction *asm_fn = new(mem) ASMFunction(decl->is_public,decl->ident);
		this->inst = &asm_fn->instructions;


		mem = alloc(sizeof(ASMRegister));
		ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::RBP,8);
		

		mem = alloc(sizeof(ASMOperand));
		ASMOperand *asm_dst = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);
		
		mem = alloc(sizeof(ASMPushInst));
		ASMPushInst *asm_push = new(mem) ASMPushInst(asm_dst);
		
		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::PUSH,asm_push);
		this->inst->push_back(asm_inst);

		mem = alloc(sizeof(ASMRegister));
		asm_reg = new(mem) ASMRegister(ASMRegisterType::RSP,8);
		

		mem = alloc(sizeof(ASMOperand));
		ASMOperand *asm_src = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);
		
		
		mem = alloc(sizeof(ASMMovInst));
		ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_dst,asm_src);
		
		mem = alloc(sizeof(ASMInstruction));
		asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
		this->inst->push_back(asm_inst);
		
		for (TACInstruction *inst : decl->instructions)
		{
			if (inst == nullptr)
			{
				continue;
			}
			convert_instruction(inst);
		}

		this->inst = nullptr;
		return asm_fn;
	}


	void convert_instruction(TACInstruction *inst)
	{
		switch(inst->type)
		{
			case TACInstructionType::RETURN:
			{
				convert_return_inst((TACReturnInst *)inst->instruction);
				break;
			}
			case TACInstructionType::JMP:
			{
				convert_jmp_inst((TACJmpInst *)inst->instruction);
				break;
			}
			case TACInstructionType::JMP_ZERO:
			{
				convert_jmp_zero_inst((TACJmpIfZeroInst *)inst->instruction);
				break;
			}
			case TACInstructionType::JMP_NOT_ZERO:
			{
				convert_jmp_not_zero_inst((TACJmpIfNotZeroInst *)inst->instruction);
				break;
			}
			case TACInstructionType::COPY:
			{
				convert_copy_inst((TACCopyInst *)inst->instruction);
				break;
			}
			case TACInstructionType::UNARY:
			{
				convert_unary_inst((TACUnaryInst *)inst->instruction);
				break;
			}
			case TACInstructionType::LABEL:
			{
				convert_label_inst((TACLabelInst *)inst->instruction);
				break;
			}
			case TACInstructionType::BINARY:
			{
				convert_binary_inst((TACBinaryInst *)inst->instruction);
				break;
			}
		}
	}

	void convert_label_inst(TACLabelInst *inst)
	{
		void *mem = alloc(sizeof(ASMLabelInst));
		ASMLabelInst *asm_label = new(mem) ASMLabelInst(inst->label);

		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::LABEL,asm_label);
		this->inst->push_back(asm_inst);

	}

	void convert_jmp_zero_inst(TACJmpIfZeroInst *inst)
	{
		ASMOperand *asm_value = convert_value(inst->value);

		void *mem = alloc(sizeof(int));
		int *int_value = new(mem) int;
		*int_value = 0;

		mem = alloc(sizeof(ASMImmediate));
		ASMImmediate *asm_imm = new(mem) ASMImmediate(ASMImmediateType::I32,int_value);
		
		mem = alloc(sizeof(ASMOperand));
		ASMOperand *asm_operand = new(mem) ASMOperand(ASMOperandType::IMMEDIATE,asm_imm);

		mem = alloc(sizeof(ASMCmpInst));
		ASMCmpInst *asm_cmp = new(mem) ASMCmpInst(asm_value,asm_operand);

		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::CMP,asm_cmp);
		this->inst->push_back(asm_inst);


		mem = alloc(sizeof(ASMJmpCondInst));
		ASMJmpCondInst *asm_jmp_cond = new(mem) ASMJmpCondInst(ASMCondition::EQUAL,inst->label);
		
		mem = alloc(sizeof(ASMInstruction));
		asm_inst = new(mem) ASMInstruction(ASMInstructionType::JMP_COND,asm_jmp_cond);
		this->inst->push_back(asm_inst);

	}


	void convert_jmp_not_zero_inst(TACJmpIfNotZeroInst *inst)
	{
		std::cout << " huh  " <<std::endl;
		ASMOperand *asm_value = convert_value(inst->value);

		void *mem = alloc(sizeof(int));
		int *int_value = new(mem) int;
		*int_value = 0;

		mem = alloc(sizeof(ASMImmediate));
		ASMImmediate *asm_imm = new(mem) ASMImmediate(ASMImmediateType::I32,int_value);
		
		mem = alloc(sizeof(ASMOperand));
		ASMOperand *asm_operand = new(mem) ASMOperand(ASMOperandType::IMMEDIATE,asm_imm);

		mem = alloc(sizeof(ASMCmpInst));
		ASMCmpInst *asm_cmp = new(mem) ASMCmpInst(asm_value,asm_operand);

		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::CMP,asm_cmp);
		this->inst->push_back(asm_inst);


		mem = alloc(sizeof(ASMJmpCondInst));
		ASMJmpCondInst *asm_jmp_cond = new(mem) ASMJmpCondInst(ASMCondition::NOT_EQUAL,inst->label);
		
		mem = alloc(sizeof(ASMInstruction));
		asm_inst = new(mem) ASMInstruction(ASMInstructionType::JMP_COND,asm_jmp_cond);
		this->inst->push_back(asm_inst);

	}



	void convert_copy_inst(TACCopyInst *inst)
	{
		ASMOperand *asm_dst = convert_value(inst->dst);
		ASMOperand *asm_src = convert_value(inst->src);
		
		void *mem = alloc(sizeof(ASMMovInst));
		ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_dst,asm_src);
		
		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
		this->inst->push_back(asm_inst);
	
	}

	void convert_jmp_inst(TACJmpInst *inst)
	{
		void *mem = alloc(sizeof(ASMJmpInst));
		ASMJmpInst *asm_jmp = new(mem) ASMJmpInst(inst->label);
		
		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::JMP,asm_jmp);
		this->inst->push_back(asm_inst);
	}

	void convert_return_inst(TACReturnInst *inst)
	{
		void *mem = alloc(sizeof(ASMRegister));
		ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::RAX,4);

		mem = alloc(sizeof(ASMOperand));
		ASMOperand *asm_dst = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);
		ASMOperand *asm_src = convert_value(inst->value);
		
		
		mem = alloc(sizeof(ASMMovInst));
		ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_dst,asm_src);
		
		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
		this->inst->push_back(asm_inst);
		
		
	
	
		mem = alloc(sizeof(ASMRegister));
		asm_reg = new(mem) ASMRegister(ASMRegisterType::RSP,8);
		

		mem = alloc(sizeof(ASMOperand));
		asm_dst = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);
		
		mem = alloc(sizeof(ASMRegister));
		asm_reg = new(mem) ASMRegister(ASMRegisterType::RBP,8);
		

		mem = alloc(sizeof(ASMOperand));
		asm_src = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);
		

		mem = alloc(sizeof(ASMMovInst));
		asm_mov = new(mem) ASMMovInst(asm_dst,asm_src);
		
		mem = alloc(sizeof(ASMInstruction));
		asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
		this->inst->push_back(asm_inst);
		
		
		
		mem = alloc(sizeof(ASMPopInst));
		ASMPopInst *asm_pop = new(mem) ASMPopInst(asm_src);
		
		mem = alloc(sizeof(ASMInstruction));
		asm_inst = new(mem) ASMInstruction(ASMInstructionType::POP,asm_pop);
		this->inst->push_back(asm_inst);

		
		mem = alloc(sizeof(ASMRetInst));
		ASMRetInst *asm_ret = new(mem) ASMRetInst();

		mem = alloc(sizeof(ASMInstruction));
		asm_inst = new(mem) ASMInstruction(ASMInstructionType::RET,asm_ret);
		this->inst->push_back(asm_inst);
	}
	


	void convert_binary_inst(TACBinaryInst *inst)
	{
		ASMOperand *asm_dst = convert_value(inst->dst);
		ASMOperand *asm_src1 = convert_value(inst->src1);
		ASMOperand *asm_src2 = convert_value(inst->src2);
		
		void *mem = alloc(sizeof(ASMMovInst));
		ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_dst,asm_src1);
		
		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
		this->inst->push_back(asm_inst);

		switch(inst->op)
		{
			case TACBinaryOperator::ADD:
			{
				void *mem = alloc(sizeof(ASMAddInst));
				ASMAddInst *asm_add = new(mem) ASMAddInst(asm_dst,asm_src2);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::ADD,asm_add);
				this->inst->push_back(asm_inst);

				break;
			}
			case TACBinaryOperator::SUB:
			{
				std::cout << " sub  " << std::endl;
				void *mem = alloc(sizeof(ASMSubInst));
				ASMSubInst *asm_sub = new(mem) ASMSubInst(asm_dst,asm_src2);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::SUB,asm_sub);
				this->inst->push_back(asm_inst);

				break;
			}
			case TACBinaryOperator::LESS:
			{
				convert_binary_logical(asm_dst,asm_src2,ASMCondition::LESS);
				break;
			}
			case TACBinaryOperator::LESS_EQUAL:
			{
				convert_binary_logical(asm_dst,asm_src2,ASMCondition::LESS_EQUAL);
				break;
			}
			case TACBinaryOperator::GREATER:
			{
				convert_binary_logical(asm_dst,asm_src2,ASMCondition::GREATER);
				break;
			}
			case TACBinaryOperator::GREATER_EQUAL:
			{
				convert_binary_logical(asm_dst,asm_src2,ASMCondition::GREATER_EQUAL);
				break;
			}
			
			case TACBinaryOperator::None:
			{
				DEBUG_PANIC("operator none tac " + std::to_string((int)inst->op));
				break;
			}
		}

	}


	void convert_binary_logical(ASMOperand *asm_dst,ASMOperand *asm_src2,ASMCondition condition)
	{
		void *mem = alloc(sizeof(ASMCmpInst));
		ASMCmpInst *asm_cmp = new(mem) ASMCmpInst(asm_dst,asm_src2);
		
		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::CMP,asm_cmp);
		this->inst->push_back(asm_inst);
		
		mem = alloc(sizeof(int));
		int *int_value = new(mem) int;
		*int_value = 0;

		mem = alloc(sizeof(ASMImmediate));
		ASMImmediate *asm_imm = new(mem) ASMImmediate(ASMImmediateType::I32,int_value);
		
		mem = alloc(sizeof(ASMOperand));
		ASMOperand *asm_operand = new(mem) ASMOperand(ASMOperandType::IMMEDIATE,asm_imm);

		mem = alloc(sizeof(ASMMovInst));
		ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_dst,asm_operand);

		mem = alloc(sizeof(ASMInstruction));
		asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
		this->inst->push_back(asm_inst);

		mem = alloc(sizeof(ASMSetCondInst));
		ASMSetCondInst *asm_setcond = new(mem) ASMSetCondInst(condition,asm_dst);
		
		mem = alloc(sizeof(ASMInstruction));
		asm_inst = new(mem) ASMInstruction(ASMInstructionType::SET_COND,asm_setcond);
		this->inst->push_back(asm_inst);
			
	}

	void convert_unary_inst(TACUnaryInst *inst)
	{
		ASMOperand *asm_dst = convert_value(inst->dst);
		ASMOperand *asm_src = convert_value(inst->src);
		
		void *mem = alloc(sizeof(ASMMovInst));
		ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_dst,asm_src);
		
		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
		this->inst->push_back(asm_inst);


		switch (inst->op)
		{
			case TACUnaryOperator::NEGATE:
			{
				mem = alloc(sizeof(ASMNegInst));
				ASMNegInst *asm_neg = new(mem) ASMNegInst(asm_dst);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::NEG,asm_neg);
				this->inst->push_back(asm_inst);
				break;
			}
			case TACUnaryOperator::COMPLEMENT:
			{
				mem = alloc(sizeof(ASMNotInst));
				ASMNotInst *asm_not = new(mem) ASMNotInst(asm_dst);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::NOT,asm_not);
				this->inst->push_back(asm_inst);
				break;
			}
		}
		
	}


	ASMOperand *convert_value(TACValue *value)
	{
		ASMOperand *asm_operand = nullptr;
		
		switch (value->type)
		{
			case TACValueType::CONSTANT:
			{
				TACConstant *tac_const = (TACConstant *)value->value;

				switch(tac_const->type)
				{
					case TACConstantType::I32:
					{
						void *mem = alloc(sizeof(int));
						int *asm_value = new(mem) int;
						*asm_value = *((int *)tac_const->constant);
						std::cout << "  const " << *asm_value << std::endl;

						mem = alloc(sizeof(ASMImmediate));
						ASMImmediate *asm_imm = new(mem) ASMImmediate(ASMImmediateType::I32,asm_value);
						mem = alloc(sizeof(ASMOperand));
						asm_operand = new(mem) ASMOperand(ASMOperandType::IMMEDIATE,asm_imm);
						break;
					}
				}
				break;
			}
			case TACValueType::VARIABLE:
			{
				TACVariable *tac_var = (TACVariable *)value->value;

				void *mem = alloc(sizeof(ASMPseudo));
				ASMPseudo *asm_pseudo = new(mem) ASMPseudo(tac_var->ident);

				mem = alloc(sizeof(ASMOperand));
				asm_operand = new(mem) ASMOperand(ASMOperandType::PSEUDO,asm_pseudo);
				break;
			}

			default:
			{
			}
		}

		return asm_operand;
	}
};


#endif
