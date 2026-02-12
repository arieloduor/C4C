#ifndef C4_MAKASM_H
#define C4_MAKASM_H

#include "lexer.hpp"
#include "../../include/isa.hpp"


class Codegen
{
public:
    std::string name;
    std::vector<Tokens> tokens;

	int tokens_length;
	int index = 0;

    Program program;

    Codegen(std::string name,std::vector<Tokens> tokens)
    {
        this->name = name;
        this->tokens = tokens;

		this->tokens_length = tokens.size();
		this->index = 0;

        while (not is_at_end())
        {
            gen_makasm();
        }

        write_program_to_file(this->program);
    }




    /**
	 * Returns a pointer to the current token or a future token without consuming it.
	 * The function takes one parameter:
	 * -look_ahead  -> Offset from the current token
	 * 
	 * It returns Pointer to token, or nullptr if out of bounds
	 */

	Tokens *peek(int look_ahead = 0)
	{
		if ( (this->index + look_ahead) >= this->tokens_length)
		{
			return nullptr;
		}

		return &this->tokens[this->index + look_ahead];
	}


	/**
	 * Consumes and returns the current token, advancing the parser cursor.
	 */

	Tokens consume()
	{
		return this->tokens[this->index++];
	}


    /**
	 * Checks whether the index has reached the end of the token's
	 * length.
	 */

	bool is_at_end()
	{
		return this->index >= this->tokens_length;
	}


    /**
	 * Reports a fatal parsing error and terminates compilation.
	 * This function does not return.
	 */

	void fatal(std::string string)
	{
		DEBUG_PANIC(string);
	}



    TokenType string_to_token(std::string keyword)
	{
		if (keyword == "mov")
		{
			return TokenType::TOKEN_MOV;
		}
        else if (keyword == "add")
		{
			return TokenType::TOKEN_ADD;
		}
		else if (keyword == "sub")
		{
			return TokenType::TOKEN_SUB;
		}
        else if (keyword == "mul")
		{
			return TokenType::TOKEN_MUL;
		}
        else if (keyword == "div")
		{
			return TokenType::TOKEN_DIV;
		}
        else if (keyword == "mod")
		{
			return TokenType::TOKEN_MOD;
		}
        else if (keyword == "push")
		{
			return TokenType::TOKEN_PUSH;
		}
        else if (keyword == "pop")
		{
			return TokenType::TOKEN_POP;
		}
        else if (keyword == "jmp")
		{
			return TokenType::TOKEN_JMP;
		}
        else if (keyword == "print")
		{
			return TokenType::TOKEN_PRINT;
		}
        else
        {
            return TokenType::TOKEN_EOF;
        }
    }

    /**
	 * Checks whether the current or lookahead token matches a symbol.
	 */

	bool is_token_type(TokenType type, int look_ahead = 0)
	{
		Tokens *token = peek(look_ahead);
		if (token == nullptr)
		{
			return false;
		}
		return token->type == type;
	}


	/**
	 * Checks whether the current or lookahead token matches a symbol.
	 */

	bool is_token(std::string string,int look_ahead = 0)
	{
		return is_token_type(string_to_token(string),look_ahead);
	}


    /**
	 * Ensures that the current token matches the given keyword.
	 * Advances the token stream or reports a fatal error.
	 */

	bool expect_keyword(std::string keyword,std::string string = "")
	{
		bool status = false;
		if (is_token_type(string_to_token(keyword)))
		{
			status = true;
		}
		else
		{
			fatal("expected " + keyword);
		}
		
		consume();
		return status;
	}


    void gen_makasm()
    {
        if(is_token("push"))
        {
            gen_push();
        }
        else if(is_token("mov"))
        {
            gen_mov();
        }
        else if(is_token("add"))
        {
            gen_add();
        }
        else if(is_token("jmp"))
        {
            gen_jmp();
        }
        else if(is_token("print"))
        {
            gen_print();
        }
        else if(is_token("eof"))
        {
            consume();
        }
        else
        {
            fatal("illegal instruction  : " + (*(peek())).string);
        }
    }


    void gen_jmp()
    {
        consume();
        Tokens token = consume();  

        if(token.type != TokenType::TOKEN_LITERAL_INT)
        {
            fatal("expected an integer after jmp");
        }

        Operand operand(OperandType::IMMEDIATE,std::stol(token.string));

        Instruction inst(InstructionType::JMP,operand,operand);
        this->program.add_instruction(inst);
    }


    void gen_push()
    {
        consume();
        Tokens token = consume();  

        if(token.type != TokenType::TOKEN_LITERAL_INT)
        {
            fatal("expected an integer after push");
        }

        


        Operand operand(OperandType::REGISTER,std::stol(token.string));

        Instruction inst(InstructionType::PUSH,operand,operand);
        this->program.add_instruction(inst);
    }



    void gen_mov()
    {
        consume();
        Tokens dst = consume();
        Tokens comma = consume();
        if(comma.type != TokenType::TOKEN_COMMA)
        {
            //fatal("expected a huh comma after register");
        }

        Tokens src = consume();
        std::string reg = dst.string;

        RegisterGp dst_reg = RegisterGp::BAD;

        if (reg == "rax")
        {
            dst_reg = RegisterGp::RAX;
        }
        else if (reg == "rbx")
        {
            dst_reg = RegisterGp::RBX;
        }
        else if (reg == "rcx")
        {
            dst_reg = RegisterGp::RCX;
        }
        else if (reg == "rdx")
        {
            dst_reg = RegisterGp::RDX;
        }
        else if (reg == "rdi")
        {
            dst_reg = RegisterGp::RDI;
        }
        else if (reg == "rsi")
        {
            dst_reg = RegisterGp::RSI;
        }
        else if (reg == "rbp")
        {
            dst_reg = RegisterGp::RBP;
        }
        else if (reg == "rsp")
        {
            dst_reg = RegisterGp::RSP;
        }

        i64 src_value = std::stol(src.string);

        Operand operand1(OperandType::REGISTER,(i64)dst_reg);
        Operand operand2(OperandType::IMMEDIATE,(i64)src_value);


        Instruction inst(InstructionType::MOV,operand1,operand2);
        this->program.add_instruction(inst);
    }




    void gen_add()
    {
        consume();
        Tokens dst = consume();
        Tokens comma = consume();
        if(comma.type != TokenType::TOKEN_COMMA)
        {
            fatal("expected a comma after register");
        }

        Tokens src = consume();
        std::string reg = dst.string;

        RegisterGp dst_reg = RegisterGp::BAD;

        if (reg == "rax")
        {
            dst_reg = RegisterGp::RAX;
        }
        else if (reg == "rbx")
        {
            dst_reg = RegisterGp::RBX;
        }
        else if (reg == "rcx")
        {
            dst_reg = RegisterGp::RCX;
        }
        else if (reg == "rdx")
        {
            dst_reg = RegisterGp::RDX;
        }
        else if (reg == "rdi")
        {
            dst_reg = RegisterGp::RDI;
        }
        else if (reg == "rsi")
        {
            dst_reg = RegisterGp::RSI;
        }
        else if (reg == "rbp")
        {
            dst_reg = RegisterGp::RBP;
        }
        else if (reg == "rsp")
        {
            dst_reg = RegisterGp::RSP;
        }

        i64 src_value = std::stol(src.string);

        Operand operand1(OperandType::REGISTER,(i64)dst_reg);
        Operand operand2(OperandType::IMMEDIATE,(i64)src_value);


        Instruction inst(InstructionType::ADD,operand1,operand2);
        this->program.add_instruction(inst);
    }

    void gen_print()
    {
        consume();
        Tokens dst = consume();

        std::string reg = dst.string;

        RegisterGp dst_reg = RegisterGp::BAD;

        if (reg == "rax")
        {
            dst_reg = RegisterGp::RAX;
        }
        else if (reg == "rbx")
        {
            dst_reg = RegisterGp::RBX;
        }
        else if (reg == "rcx")
        {
            dst_reg = RegisterGp::RCX;
        }
        else if (reg == "rdx")
        {
            dst_reg = RegisterGp::RDX;
        }
        else if (reg == "rdi")
        {
            dst_reg = RegisterGp::RDI;
        }
        else if (reg == "rsi")
        {
            dst_reg = RegisterGp::RSI;
        }
        else if (reg == "rbp")
        {
            dst_reg = RegisterGp::RBP;
        }
        else if (reg == "rsp")
        {
            dst_reg = RegisterGp::RSP;
        }


        Operand operand1(OperandType::REGISTER,(i64)dst_reg);

        Instruction inst(InstructionType::PRINT,operand1,operand1);
        this->program.add_instruction(inst);
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

};



#endif