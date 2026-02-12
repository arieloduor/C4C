#ifndef C4_MAKEDA_H
#define C4_MAKEDA_H

#include "../../../utils/include/utils.hpp"

enum class OperandType
{
    I64,
};

class Operand
{
public:
    OperandType type;
    void *value; 
};


enum class InstructionType
{
    PUSH,
    DUP,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    POP,
    JMP_TRUE,
    JMP_FALSE,
    PRINT,
};



class Instruction
{
public:
    InstructionType type;
    i64 operand;

    Instruction(InstructionType type,i64 operand)
    {
        this->type = type;
        this->operand = operand;
    }

    Instruction() = default;
};


class Stack
{
public:
    i64 stack_pointer;
    i64 *stack;
    i64 stack_size;

    Stack(int size)
    {
        this->stack_size = size;
        this->stack = (i64 *)malloc(sizeof(i64));
        this->stack_pointer = 0;
    }

    void push(i64 value)
    {
        this->stack[this->stack_pointer] = value;
    }

    void adjust_stack_pointer(i64 offset)
    {
        this->stack_pointer += offset;
    }

    i64 get_first()
    {
        return this->stack[this->stack_pointer - 1];
    }

    i64 get_second()
    {
        return this->stack[this->stack_pointer - 2];
    }

};


class Program
{
public:
    std::vector<Instruction> instructions;
    
    void add_instruction(Instruction instruction)
    {
        this->instructions.push_back(instruction);
    }
};


enum class Trap
{
    OK,
    OVERFLOW,
    UNDERFLOW,
    DIVISION_BY_ZERO,
};

class Makeda
{
    Stack stack;
    Program program;
public:

    Makeda(int size):stack(size)
    {
        Program test;
        test_inst(&test);
        write_program_to_file(test);
        read_program_to_memory("test.m");
        run();
    }


    void test_inst(Program *program)
    {
        program->add_instruction(Instruction(InstructionType::PUSH,3000));
        program->add_instruction(Instruction(InstructionType::PUSH,40));
        program->add_instruction(Instruction(InstructionType::PUSH,90));
        program->add_instruction(Instruction(InstructionType::PUSH,20));
        program->add_instruction(Instruction(InstructionType::PRINT,0));
        program->add_instruction(Instruction(InstructionType::ADD,0));
        program->add_instruction(Instruction(InstructionType::PRINT,0));
        program->add_instruction(Instruction(InstructionType::ADD,0));
        program->add_instruction(Instruction(InstructionType::PRINT,0));
        program->add_instruction(Instruction(InstructionType::DIV,0));
        program->add_instruction(Instruction(InstructionType::PRINT,0));
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
        for (Instruction inst : program.instructions)
        {
            switch(inst.type)
            {
                case InstructionType::PUSH:
                {
                    push(inst.operand);
                    break;
                }
                case InstructionType::ADD:
                {
                    add();
                    break;
                }
                case InstructionType::SUB:
                {
                    sub();
                    break;
                }
                case InstructionType::MUL:
                {
                    mul();
                    break;
                }
                case InstructionType::DIV:
                {
                    div();
                    break;
                }
                case InstructionType::PRINT:
                {
                    print();
                    break;
                }
            }
        }
    }

    bool check_underflow()
    {
        if(this->stack.stack_pointer <= 0)
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


    Trap push(i64 value)
    {
        if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);

        return Trap::OK;
    }


    Trap add()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        
        i64 value = this->stack.get_second() + this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);

        return Trap::OK;
    }



    Trap sub()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        i64 value = this->stack.get_second() - this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);

        return Trap::OK;
    }



    Trap mul()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        i64 value = this->stack.get_second() * this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);

        return Trap::OK;
    }



    Trap div()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }
        else if(check_is_zero(this->stack.get_first()))
        {
            return Trap::DIVISION_BY_ZERO;
        }


        i64 value = this->stack.get_second() / this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);

        return Trap::OK;
    }


    Trap mod()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }
        else if(check_is_zero(this->stack.get_first()))
        {
            return Trap::DIVISION_BY_ZERO;
        }


        i64 value = this->stack.get_second() % this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);

        return Trap::OK;
    }


    Trap print()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }

        std::cout << "Stack : [  ] " << std::endl;

        for(int i = 0; i < this->stack.stack_pointer; i++)
        {
            std::cout << "\t "  << this->stack.stack[i] << std::endl;
        }

        return Trap::OK;
    }
    
};



#endif