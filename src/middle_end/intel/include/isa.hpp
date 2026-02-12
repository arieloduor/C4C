#ifndef C4_MAKASM_ISA_H
#define C4_MAKASM_ISA_H




#include "../../../utils/include/utils.hpp"

enum class OperandType
{
    REGISTER,
    MEMORY,
    IMMEDIATE,
};

class Operand
{
public:
    OperandType type;
    i64 value;

    Operand(OperandType type,i64 value)
    {
        this->type = type;
        this->value = value;
    }

    Operand() = default;
};


enum class InstructionType
{
    PUSH,
    POP,
    MOV,
    ADD,
    JMP,
    PRINT,
};



enum class RegisterGp
{
    RAX = 0,
    RBX,
    RCX,
    RDX,
    RDI,
    RSI,
    RBP,
    RSP,
    BAD,
};




class Instruction
{
public:
    InstructionType type;
    Operand operand1;
    Operand operand2;

    Instruction(InstructionType type,Operand operand1,Operand operand2)
    {
        this->type = type;
        this->operand1 = operand1;
        this->operand2 = operand2;
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
    i64 ip = 0;
    
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


#endif
