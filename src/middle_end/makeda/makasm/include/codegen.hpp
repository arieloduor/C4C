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
		if (keyword == "add")
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
        else if (keyword == "l")
		{
			return TokenType::TOKEN_L;
		}
        else if (keyword == "le")
		{
			return TokenType::TOKEN_LE;
		}
        else if (keyword == "g")
		{
			return TokenType::TOKEN_G;
		}
        else if (keyword == "ge")
		{
			return TokenType::TOKEN_GE;
		}
        else if (keyword == "eq")
		{
			return TokenType::TOKEN_EQ;
		}
        else if (keyword == "ne")
		{
			return TokenType::TOKEN_NE;
		}
        else if (keyword == "jt")
		{
			return TokenType::TOKEN_JT;
		}
        else if (keyword == "jf")
		{
			return TokenType::TOKEN_JF;
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
        else if(is_token("pop"))
        {
            gen_pop();
        }
        else if(is_token("add"))
        {
            gen_add();
        }
        else if(is_token("sub"))
        {
            gen_sub();
        }
        else if(is_token("mul"))
        {
            gen_mul();
        }
        else if(is_token("div"))
        {
            gen_div();
        }
        else if(is_token("mod"))
        {
            gen_mod();
        }
        else if(is_token("jmp"))
        {
            gen_jmp();
        }
        else if(is_token("l"))
        {
            gen_l();
        }
        else if(is_token("le"))
        {
            gen_le();
        }
        else if(is_token("g"))
        {
            gen_g();
        }
        else if(is_token("ge"))
        {
            gen_ge();
        }
        else if(is_token("eq"))
        {
            gen_eq();
        }
        else if(is_token("ne"))
        {
            gen_ne();
        }
        else if(is_token("jt"))
        {
            gen_jt();
        }
        else if(is_token("jf"))
        {
            gen_jf();
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

        Instruction inst(InstructionType::JMP,std::stol(token.string));
        this->program.add_instruction(inst);
    }


    void gen_jt()
    {
        consume();
        Tokens token = consume();  

        if(token.type != TokenType::TOKEN_LITERAL_INT)
        {
            fatal("expected an integer after jt");
        }

        Instruction inst(InstructionType::JT,std::stol(token.string));
        this->program.add_instruction(inst);
    }


    void gen_jf()
    {
        consume();
        Tokens token = consume();  

        if(token.type != TokenType::TOKEN_LITERAL_INT)
        {
            fatal("expected an integer after jf");
        }

        Instruction inst(InstructionType::JF,std::stol(token.string));
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

        Instruction inst(InstructionType::PUSH,std::stol(token.string));
        this->program.add_instruction(inst);
    }

    void gen_pop()
    {
        consume();

        Instruction inst(InstructionType::POP,0);
        this->program.add_instruction(inst);
    }


    void gen_add()
    {
        consume();

        Instruction inst(InstructionType::ADD,0);
        this->program.add_instruction(inst);
    }

    void gen_sub()
    {
        consume();

        Instruction inst(InstructionType::SUB,0);
        this->program.add_instruction(inst);
    }


    void gen_mul()
    {
        consume();

        Instruction inst(InstructionType::MUL,0);
        this->program.add_instruction(inst);
    }

    void gen_div()
    {
        consume();

        Instruction inst(InstructionType::DIV,0);
        this->program.add_instruction(inst);
    }

    void gen_mod()
    {
        consume();

        Instruction inst(InstructionType::MOD,0);
        this->program.add_instruction(inst);
    }


    void gen_l()
    {
        consume();

        Instruction inst(InstructionType::L,0);
        this->program.add_instruction(inst);
    }



    void gen_le()
    {
        consume();

        Instruction inst(InstructionType::LE,0);
        this->program.add_instruction(inst);
    }


    void gen_g()
    {
        consume();

        Instruction inst(InstructionType::G,0);
        this->program.add_instruction(inst);
    }



    void gen_ge()
    {
        consume();

        Instruction inst(InstructionType::GE,0);
        this->program.add_instruction(inst);
    }



    void gen_eq()
    {
        consume();

        Instruction inst(InstructionType::EQ,0);
        this->program.add_instruction(inst);
    }



    void gen_ne()
    {
        consume();

        Instruction inst(InstructionType::NE,0);
        this->program.add_instruction(inst);
    }


    void gen_print()
    {
        consume();

        Instruction inst(InstructionType::PRINT,0);
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