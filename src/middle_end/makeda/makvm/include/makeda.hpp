#ifndef C4_MAKEDA_H
#define C4_MAKEDA_H

#include "../../include/isa.hpp"

class Makeda
{
    Stack stack;
    Program program;
public:

    Makeda(int size):stack(size)
    {
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
        for (this->program.ip = 0; this->program.ip < this->program.instructions.size() ; this->program.ip)
        {
            Instruction inst = program.instructions[this->program.ip];
            switch(inst.type)
            {
                case InstructionType::PUSH:
                {
                    push(inst.operand);
                    break;
                }
                case InstructionType::POP:
                {
                    pop();
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
                case InstructionType::JMP:
                {
                    jmp(inst.operand);
                    break;
                }
                case InstructionType::L:
                {
                    less();
                    break;
                }
                case InstructionType::LE:
                {
                    less_equal();
                    break;
                }
                case InstructionType::G:
                {
                    greater();
                    break;
                }
                case InstructionType::GE:
                {
                    greater_equal();
                    break;
                }
                case InstructionType::EQ:
                {
                    equal();
                    break;
                }
                case InstructionType::NE:
                {
                    not_equal();
                    break;
                }
                case InstructionType::JT:
                {
                    jmp_true(inst.operand);
                    break;
                }
                case InstructionType::JF:
                {
                    jmp_false(inst.operand);
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


    Trap push(i64 value)
    {
        if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);
        this->program.ip++;

        return Trap::OK;
    }


    Trap pop()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }

        this->stack.adjust_stack_pointer(-1);
        this->program.ip++;

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
        this->program.ip++;

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
        this->program.ip++;

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
        this->program.ip++;

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
        this->program.ip++;

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
        this->program.ip++;

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


        this->program.ip++;
        return Trap::OK;
    }


    Trap jmp(i64 value)
    {
        this->program.ip = value;
        return Trap::OK;
    }

    
    Trap jmp_true(i64 value)
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        
        if(this->stack.get_first())
        {
            this->program.ip = value;
            return Trap::OK;
        }

        this->program.ip++;
        return Trap::OK;
    }



    Trap jmp_false(i64 value)
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        
        if(not this->stack.get_first())
        {
            this->program.ip = value;
            return Trap::OK;
        }

        this->program.ip++;
        return Trap::OK;
    }


    Trap less()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        
        i64 value = this->stack.get_second() < this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);
        this->program.ip++;

        return Trap::OK;
    }


    Trap less_equal()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        
        i64 value = this->stack.get_second() <= this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);
        this->program.ip++;

        return Trap::OK;
    }



    Trap greater()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        
        i64 value = this->stack.get_second() > this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);
        this->program.ip++;

        return Trap::OK;
    }



    Trap greater_equal()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        
        i64 value = this->stack.get_second() >= this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);
        this->program.ip++;

        return Trap::OK;
    }



    Trap equal()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        
        i64 value = this->stack.get_second() == this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);
        this->program.ip++;

        return Trap::OK;
    }



    Trap not_equal()
    {
        if(check_underflow())
        {
            return Trap::UNDERFLOW;
        }
        else if(check_overflow())
        {
            return Trap::OVERFLOW;
        }

        
        i64 value = this->stack.get_second() != this->stack.get_first();
        this->stack.adjust_stack_pointer(-2);
        this->stack.push(value);
        this->stack.adjust_stack_pointer(1);
        this->program.ip++;

        return Trap::OK;
    }

};



#endif