#include "front_end/include/file_to_string.hpp"
#include "front_end/include/string_to_file.hpp"
#include "front_end/include/lexer.hpp"
#include "front_end/include/parser.hpp"
#include "middle_end/tac/include/ast_to_tac.hpp"

#include "back_end/x86_64/include/tac_to_intel64.hpp"
#include "back_end/x86_64/include/pseudo.hpp"
#include "back_end/x86_64/include/fixup.hpp"


#include "back_end/x86_64/include/codegen.hpp"


int main(int argc,char *argv[])
{
	std::string file_name(argv[1]);
	std::cout << " hello c4c compiler " << file_name << std::endl;

	FileToString fs(file_name);
	std::string file_contents = fs.read();
	//DEBUG_PRINT(file_contents,"");

	Lexer lexer(file_name,file_contents);
	std::vector<Tokens> tokens = lexer.scan_tokens();
	lexer.print();

	{
		Arena arena(100000);
		Parser parser(file_name,tokens,&arena);
		parser.parse_program();
		DEBUG_PRINT("sanity check : ", " after parser ");
		//arena.reset();
		
		AstToTac tac(file_name,parser.program,&arena);

		DEBUG_PRINT("sanity check : ", " after ast to tac ");
		TacToIntel64 intel(file_name,tac.program,&arena);

		DEBUG_PRINT("sanity check : ", " after tac to intel64");
		Pseudo pseudo(file_name,intel.program,&arena);
		DEBUG_PRINT("sanity check : ", " after replace pseudo");
		FixUp fix(file_name,pseudo.program,&arena);

		DEBUG_PRINT("sanity check : ", " after fix inst");
		Codegen gen(file_name,intel.program);
		DEBUG_PRINT("sanity check : ", " after codegen");
		StringToFile(file_name.substr(0, file_name.length() - 3) + ".asm",gen.string);
	}
}
