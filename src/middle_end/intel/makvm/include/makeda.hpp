#ifndef C4_INTEL_H
#define C4_INTEL_H

#include "../../include/isa.hpp"

class Intel
{
    Stack stack;
    Program program;
    i64 GpRegisters[8] = {0};
public:

    Intel(int size):stack(size)
    {
        read_program_to_memory("test.m");
        //test_inst(&this->program);
        run();
    }


    void test_inst(Program *program)
    {
        /**
        program->add_instruction(Instruction(InstructionType::PUSH,3000));
        program->add_instruction(Instruction(InstructionType::PUSH,40));
        program->add_instruction(Instruction(InstructionType::PUSH,90));
        program->add_instruction(Instruction(InstructionType::PUSH,20));
        program->add_instruction(Instruction(InstructionType::PRINT,0));
        program->add_instruction(Instruction(InstructionType::MOV,0));
        program->add_instruction(Instruction(InstructionType::PRINT,0));
        program->add_instruction(Instruction(InstructionType::ADD,0));
        program->add_instruction(Instruction(InstructionType::PRINT,0));
        */


        program->add_instruction(Instruction(InstructionType::MOV,Operand(OperandType::REGISTER,(i64)RegisterGp::RAX),Operand(OperandType::IMMEDIATE,90)));
        program->add_instruction(Instruction(InstructionType::PRINT,Operand(OperandType::REGISTER,(i64)RegisterGp::RBX),Operand(OperandType::IMMEDIATE,900)));

    }



    void write_program_to_file(Program program)
    {
        FILE *fout = fopen("test.m","w");

        if(fout == NULL)
        {
            return;
        }

        for (Instruction inst : program.instructions)
        {
            fwrite(&inst,1,sizeof(Instruction),fout);
        }

        fclose(fout);
    }



    void read_program_to_memory(std::string name)
    {
        FILE *fin = fopen(name.c_str(),"r");
        if(fin == NULL)
        {
            return;
        }

        i64 size = 0;

        fseek(fin,0,SEEK_END);
        size = ftell(fin);
        fseek(fin,0,SEEK_SET);


        Instruction inst;
        i64 count = size / sizeof(Instruction);

        for(int i = 0; i < count; i++)
        {
            fread(&inst,1,sizeof(Instruction),fin);
            this->program.add_instruction(inst);
        }

        fclose(fin);
        
    }

    


    void run()
    {
        for (this->program.ip = 0; this->program.ip < this->program.instructions.size() ; this->program.ip)
        {
            Instruction *inst = &program.instructions[this->program.ip];
            switch(inst->type)
            {
                case InstructionType::PUSH:
                {
                    push(inst);
                    break;
                }
                case InstructionType::POP:
                {
                    pop(inst);
                    break;
                }
                case InstructionType::ADD:
                {
                    add(inst);
                    break;
                }
                case InstructionType::MOV:
                {
                    mov(inst);
                    break;
                }
                case InstructionType::JMP:
                {
                    jmp(inst);
                    break;
                }
                case InstructionType::PRINT:
                {
                    print(inst);
                    break;
                }
            }
        }
    }

    bool check_underflow()
    {
        if(this->stack.stack_pointer < 0)
        {
            return true;
        }

        return false;
    }


    bool check_overflow()
    {
        if(this->stack.stack_pointer >= this->stack.stack_size)
        {
            return true;
        }

        return false;
    }


    bool check_is_zero(i64 value)
    {
        return value == 0;
    }





    Trap push(Instruction *inst)
    {
        if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        i64 value = 0;

        switch(inst->operand1.type)
        {
            case OperandType::REGISTER:
            {
                value = this->GpRegisters[inst->operand1.value];
            }
        }

        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);
        this->program.ip++;

        return Trap::OK;
    }


    Trap pop(Instruction *inst)
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }

        switch(inst->operand1.type)
        {
            case OperandType::REGISTER:
            {
                this->GpRegisters[inst->operand1.value] = this->stack.get_first();
            }
        }

        this->stack.adjust_stack_pointer(-1);
        this->program.ip++;

        return Trap::OK;
    }


    Trap mov(Instruction *inst)
    {   

        switch(inst->operand1.type)
        {
            case OperandType::REGISTER:
            {
                switch(inst->operand2.type)
                {
                    case OperandType::REGISTER:
                    {
                        this->GpRegisters[inst->operand1.value] = this->GpRegisters[inst->operand2.value];
                        break;
                    }
                    case OperandType::IMMEDIATE:
                    {
                        this->GpRegisters[inst->operand1.value] = inst->operand2.value;
                        break;
                    }
                }
                break;
            }
        }

        this->program.ip++;
        return Trap::OK;
    }



    Trap add(Instruction *inst)
    {   

        switch(inst->operand1.type)
        {
            case OperandType::REGISTER:
            {
                switch(inst->operand2.type)
                {
                    case OperandType::REGISTER:
                    {
                        this->GpRegisters[inst->operand1.value] = this->GpRegisters[inst->operand1.value] + this->GpRegisters[inst->operand2.value];
                        break;
                    }
                    case OperandType::IMMEDIATE:
                    {
                        this->GpRegisters[inst->operand1.value] = this->GpRegisters[inst->operand1.value] + inst->operand2.value;
                        break;
                    }
                }
                break;
            }
        }

        this->program.ip++;
        return Trap::OK;
    }



    Trap jmp(Instruction *inst)
    {
        this->program.ip = inst->operand1.value;
        return Trap::OK;
    }

    
    Trap print(Instruction *inst)
    {
        
        i64 value = 0;
        std::string is_reg = "value:\n\t";

        switch(inst->operand1.type)
        {
            case OperandType::REGISTER:
            {
                is_reg = get_gp_register((RegisterGp)inst->operand1.value) + ":\n\t";
                value = this->GpRegisters[inst->operand1.value];
                break;
            }
            case OperandType::IMMEDIATE:
            {
                value = this->GpRegisters[inst->operand1.value];
                break;
            }
        }

        std::cout << is_reg << value << std::endl;
        this->program.ip++;
        return Trap::OK;
    }

    std::string get_gp_register(RegisterGp reg)
    {
        switch(reg)
        {
            case RegisterGp::RAX:
            {
                return "rax";
                break;
            }
            case RegisterGp::RBX:
            {
                return "rbx";
                break;
            }
            case RegisterGp::RCX:
            {
                return "rcx";
                break;
            }
            case RegisterGp::RDX:
            {
                return "rdx";
                break;
            }
            case RegisterGp::RDI:
            {
                return "rdi";
                break;
            }
            case RegisterGp::RSI:
            {
                return "rsi";
                break;
            }
            case RegisterGp::RBP:
            {
                return "rbp";
                break;
            }
            case RegisterGp::RSP:
            {
                return "rsp";
                break;
            }
            default:
            {
                return "bad register";
                break;
            }
        }
    }

   

};



#endif