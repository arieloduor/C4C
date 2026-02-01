#ifndef C4C_FIXUP_H
#define C4C_FIXUP_H

#include "pseudo.hpp"


class FixUp
{
public:
	std::string file_name;
	ASMProgram *program;

	int index = 0;
	int stack_counter = 0;
	std::vector<ASMInstruction *> *inst = nullptr;
	Arena *arena;


	FixUp(std::string file_name,ASMProgram *program,Arena *arena)
	{
		this->file_name = file_name;
		this->program = program;
		this->arena = arena;

		for(ASMDeclaration *decl : this->program->decls)
		{
			fix_decl(decl);
		}
	}

	int align(int number,int alignment)
	{
		return ((number + (alignment - 1)) & ~(alignment - 1));
	}
	
	void *alloc(int size)
	{
		return this->arena->alloc(size);
	}
	void fix_decl(ASMDeclaration *decl)
	{
		switch (decl->type)
		{
			case ASMDeclarationType::FUNCTION:
			{
				fix_function((ASMFunction *)decl->decl);
				break;
			}
		}

	}


	void fix_function(ASMFunction *decl)
	{	
		this->inst = &decl->instructions;
		this->stack_counter = decl->stack_counter;

		int aligned = align(this->stack_counter,8);
		
		void *mem = alloc(sizeof(ASMRegister));
		ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::RSP,8);
		
		mem = alloc(sizeof(ASMOperand));
		ASMOperand *asm_dst = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);
		
		
		mem = alloc(sizeof(int));
		int *asm_value = new(mem) int;
		*asm_value = aligned;

		mem = alloc(sizeof(ASMImmediate));
		ASMImmediate *asm_imm = new(mem) ASMImmediate(ASMImmediateType::I32,asm_value);

		mem = alloc(sizeof(ASMOperand));
		ASMOperand *asm_src = new(mem) ASMOperand(ASMOperandType::IMMEDIATE,asm_imm);
		

		mem = alloc(sizeof(ASMSubInst));
		ASMSubInst *asm_sub = new(mem) ASMSubInst(asm_dst,asm_src);
		
		mem = alloc(sizeof(ASMInstruction));
		ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::SUB,asm_sub);
		this->inst->insert(this->inst->begin() + 2,asm_inst);

		for (this->index = 0; this->index < this->inst->size(); this->index++)
		{
			ASMInstruction *inst = this->inst->at(this->index);
			fix_instruction(inst);
		}


		this->stack_counter = 0;
	}


	void fix_instruction(ASMInstruction *inst)
	{
		switch(inst->type)
		{
			case ASMInstructionType::RET:
			{
				fix_return_inst((ASMRetInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::MOV:
			{
				fix_mov_inst((ASMMovInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::MOVZEROEXTEND:
			{
				fix_mov_zero_extend_inst((ASMMovZeroExtendInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::ADD:
			{
				fix_add_inst((ASMAddInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::SUB:
			{
				fix_sub_inst((ASMSubInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::CMP:
			{
				fix_cmp_inst((ASMCmpInst *)inst->instruction);
				break;
			}
			case ASMInstructionType::NEG:
			{
				fix_neg_inst((ASMNegInst *)inst->instruction);
				break;
			}
		}
	}


	void fix_return_inst(ASMRetInst *inst)
	{
	}


	void fix_mov_zero_extend_inst(ASMMovZeroExtendInst *inst)
	{
		if(inst->dst->type == ASMOperandType::REGISTER)
		{
			this->inst->at(this->index)->type = ASMInstructionType::MOV;
		}
		else if(inst->dst->type == ASMOperandType::STACK)
		{
			//ASMStack *src_stack = (ASMStack *)inst->src->operand;

			void *mem = alloc(sizeof(ASMRegister));
			ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,4);
			
			mem = alloc(sizeof(ASMOperand));
			ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

			mem = alloc(sizeof(ASMMovInst));
			ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,inst->src);
			asm_mov->add_type(ASMType::I32);

			mem = alloc(sizeof(ASMInstruction));
			ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
			this->inst->at(this->index++) = asm_inst;


			mem = alloc(sizeof(ASMMovInst));
			asm_mov = new(mem) ASMMovInst(inst->dst,asm_scratch_reg);
			asm_mov->add_type(ASMType::I64);

			mem = alloc(sizeof(ASMInstruction));
			asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);

			this->inst->insert(this->inst->begin() + this->index,asm_inst);
		}
	}

	
	void fix_mov_inst(ASMMovInst *inst)
	{
		fix_general(inst->dst,inst->src);
	}

	void fix_general(ASMOperand *dst,ASMOperand *src)
	{
		if (dst->type == ASMOperandType::STACK)
		{
			ASMStack *dst_stack = (ASMStack *)dst->operand;
			
			if(src->type == ASMOperandType::STACK)
			{
				ASMStack *src_stack = (ASMStack *)src->operand;
				void *mem = alloc(sizeof(ASMRegister));
				ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,src_stack->size);
				
				mem = alloc(sizeof(ASMOperand));
				ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

				mem = alloc(sizeof(ASMMovInst));
				ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,src);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
				this->inst->at(this->index++) = asm_inst;


				mem = alloc(sizeof(ASMMovInst));
				asm_mov = new(mem) ASMMovInst(dst,asm_scratch_reg);
				
				mem = alloc(sizeof(ASMInstruction));
				asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);


				this->inst->insert(this->inst->begin() + this->index,asm_inst);
			}
			else if(src->type == ASMOperandType::DATA)
			{
				ASMData *src_stack = (ASMData *)src->operand;
				void *mem = alloc(sizeof(ASMRegister));
				ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,src_stack->size);
				
				mem = alloc(sizeof(ASMOperand));
				ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

				mem = alloc(sizeof(ASMMovInst));
				ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,src);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
				this->inst->at(this->index++) = asm_inst;


				mem = alloc(sizeof(ASMMovInst));
				asm_mov = new(mem) ASMMovInst(dst,asm_scratch_reg);
				
				mem = alloc(sizeof(ASMInstruction));
				asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);


				this->inst->insert(this->inst->begin() + this->index,asm_inst);
			}
		}
	
		else if (dst->type == ASMOperandType::DATA)
		{
			ASMData *dst_data = (ASMData *)dst->operand;
			
			if(src->type == ASMOperandType::STACK)
			{
				ASMStack *src_stack = (ASMStack *)src->operand;
				void *mem = alloc(sizeof(ASMRegister));
				ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,src_stack->size);
				
				mem = alloc(sizeof(ASMOperand));
				ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

				mem = alloc(sizeof(ASMMovInst));
				ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,src);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
				this->inst->at(this->index++) = asm_inst;


				mem = alloc(sizeof(ASMMovInst));
				asm_mov = new(mem) ASMMovInst(dst,asm_scratch_reg);
				
				mem = alloc(sizeof(ASMInstruction));
				asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);


				this->inst->insert(this->inst->begin() + this->index,asm_inst);
			}
			else if(src->type == ASMOperandType::DATA)
			{

				
				ASMData *src_stack = (ASMData *)src->operand;
				void *mem = alloc(sizeof(ASMRegister));
				ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,src_stack->size);
				
				mem = alloc(sizeof(ASMOperand));
				ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

				mem = alloc(sizeof(ASMMovInst));
				ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,src);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
				this->inst->at(this->index++) = asm_inst;


				mem = alloc(sizeof(ASMMovInst));
				asm_mov = new(mem) ASMMovInst(dst,asm_scratch_reg);
				
				mem = alloc(sizeof(ASMInstruction));
				asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);


				this->inst->insert(this->inst->begin() + this->index,asm_inst);
			}
		}
	
	}
	
	
	void fix_sub_inst(ASMSubInst *inst)
	{
		ASMOperand *dst = inst->dst;
		ASMOperand *src = inst->src;

		if (dst->type == ASMOperandType::STACK)
		{
			ASMStack *dst_stack = (ASMStack *)dst->operand;
			ASMStack *src_stack = (ASMStack *)src->operand;
			
			if(src->type == ASMOperandType::STACK)
			{

				void *mem = alloc(sizeof(ASMRegister));
				ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,src_stack->size);
				
				mem = alloc(sizeof(ASMOperand));
				ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

				mem = alloc(sizeof(ASMMovInst));
				ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,src);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
				this->inst->at(this->index++) = asm_inst;


				mem = alloc(sizeof(ASMSubInst));
				ASMSubInst *asm_sub = new(mem) ASMSubInst(dst,asm_scratch_reg);
				
				mem = alloc(sizeof(ASMInstruction));
				asm_inst = new(mem) ASMInstruction(ASMInstructionType::SUB,asm_sub);


				this->inst->insert(this->inst->begin() + this->index,asm_inst);
			}
		}
	}

	
	void fix_add_inst(ASMAddInst *inst)
	{
		ASMOperand *dst = inst->dst;
		ASMOperand *src = inst->src;

		if (dst->type == ASMOperandType::STACK)
		{
			ASMStack *dst_stack = (ASMStack *)dst->operand;
			ASMStack *src_stack = (ASMStack *)src->operand;
			
			if(src->type == ASMOperandType::STACK)
			{

				void *mem = alloc(sizeof(ASMRegister));
				ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,src_stack->size);
				
				mem = alloc(sizeof(ASMOperand));
				ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

				mem = alloc(sizeof(ASMMovInst));
				ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,src);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
				this->inst->at(this->index++) = asm_inst;


				mem = alloc(sizeof(ASMAddInst));
				ASMAddInst *asm_add = new(mem) ASMAddInst(dst,asm_scratch_reg);
				
				mem = alloc(sizeof(ASMInstruction));
				asm_inst = new(mem) ASMInstruction(ASMInstructionType::ADD,asm_add);


				this->inst->insert(this->inst->begin() + this->index,asm_inst);
			}
		}
	}


	void fix_cmp_inst(ASMCmpInst *inst)
	{
		ASMOperand *dst = inst->dst;
		ASMOperand *src = inst->src;

		if (dst->type == ASMOperandType::STACK)
		{
			ASMStack *dst_stack = (ASMStack *)dst->operand;
			ASMStack *src_stack = (ASMStack *)src->operand;
			
			if(src->type == ASMOperandType::STACK)
			{

				void *mem = alloc(sizeof(ASMRegister));
				ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,src_stack->size);
				
				mem = alloc(sizeof(ASMOperand));
				ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

				mem = alloc(sizeof(ASMMovInst));
				ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,src);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
				this->inst->at(this->index++) = asm_inst;


				mem = alloc(sizeof(ASMCmpInst));
				ASMCmpInst *asm_cmp = new(mem) ASMCmpInst(dst,asm_scratch_reg);
				
				mem = alloc(sizeof(ASMInstruction));
				asm_inst = new(mem) ASMInstruction(ASMInstructionType::CMP,asm_cmp);


				this->inst->insert(this->inst->begin() + this->index,asm_inst);
			}
		}
		else if (dst->type == ASMOperandType::IMMEDIATE)
		{
			ASMImmediate *asm_imm = (ASMImmediate *)dst->operand;

			if (asm_imm->type == ASMImmediateType::I32)
			{
				void *mem = alloc(sizeof(ASMRegister));
				ASMRegister *asm_reg = new(mem) ASMRegister(ASMRegisterType::R11,4);
				
				mem = alloc(sizeof(ASMOperand));
				ASMOperand *asm_scratch_reg = new(mem) ASMOperand(ASMOperandType::REGISTER,asm_reg);

				mem = alloc(sizeof(ASMMovInst));
				ASMMovInst *asm_mov = new(mem) ASMMovInst(asm_scratch_reg,dst);
				
				mem = alloc(sizeof(ASMInstruction));
				ASMInstruction *asm_inst = new(mem) ASMInstruction(ASMInstructionType::MOV,asm_mov);
				this->inst->at(this->index++) = asm_inst;


				mem = alloc(sizeof(ASMCmpInst));
				ASMCmpInst *asm_cmp = new(mem) ASMCmpInst(asm_scratch_reg,src);
				
				mem = alloc(sizeof(ASMInstruction));
				asm_inst = new(mem) ASMInstruction(ASMInstructionType::CMP,asm_cmp);


				this->inst->insert(this->inst->begin() + this->index,asm_inst);

			}
		}
	}

	void fix_neg_inst(ASMNegInst *inst)
	{
		return;
	}
	



};




#endif
