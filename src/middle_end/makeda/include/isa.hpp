#ifndef C4_MAKASM_ISA_H
#define C4_MAKASM_ISA_H




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
    POP,
    DUP,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    JT,
    JF,
    JMP,
    EQ,
    NE,
    L,
    LE,
    G,
    GE,
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
