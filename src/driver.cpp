#include "front_end/include/file_to_string.hpp"
#include "front_end/include/string_to_file.hpp"
#include "front_end/include/lexer.hpp"
#include "front_end/include/preprocessor.hpp"
#include "front_end/include/parser.hpp"
#include "front_end/include/identifier_resolution.hpp"
#include "front_end/include/type_checking.hpp"
#include "front_end/include/loop_labelling.hpp"


#include "middle_end/C/include/ast_to_c.hpp"
#include "middle_end/JS/include/ast_to_js.hpp"

#include "middle_end/tac/include/ast_to_tac.hpp"
#include "back_end/x86_64/include/tac_to_intel64.hpp"
#include "back_end/x86_64/include/pseudo.hpp"
#include "back_end/x86_64/include/fixup.hpp"


#include "back_end/x86_64/include/codegen.hpp"
#include "utils/include/linkedlist.h"

int main(int argc,char *argv[])
{
	std::string file_name(argv[1]);
	std::cout << " hello c4c compiler " << file_name << std::endl;

	FileToString fs(file_name);
	std::string file_contents = fs.read();
	//DEBUG_PRINT(file_contents,"");

	Preprocessor preprocessor(file_name, file_contents);
	std::string processed_output = preprocessor.run();
	preprocessor.print_errors();
	
	DEBUG_PRINT("processed output is: ", processed_output);
	Lexer lexer(file_name,processed_output);
	std::vector<Tokens> tokens = lexer.scan_tokens();
	lexer.print_errors();
	//lexer.print();

	{
		Arena arena(1000000);
		
		Parser parser(file_name,tokens,&arena);
		parser.parse_program();
		DEBUG_PRINT("sanity check : ", " after parser ");
		//arena.reset();

		IdentifierResolution resolve(file_name,parser.program);


		AstToC C(file_name,resolve.program);
		StringToFile(file_name.substr(0, file_name.length() - 3) + ".c",C.string);

		AstToJS JS(file_name,resolve.program);
		StringToFile(file_name.substr(0, file_name.length() - 3) + ".js",JS.string);
		
		
		/*

		TypeChecking type_check(file_name,resolve.program);
		DEBUG_PRINT("sanity check : ", " after resolve ");
		
		LoopLabelling loop_label(file_name,type_check.program,resolve.global_counter);

		AstToTac tac(file_name,loop_label.program,&arena,loop_label.global_counter,type_check.table);

		DEBUG_PRINT("sanity check : ", " after ast to tac ");
		TacToIntel64 intel(file_name,tac.program,&arena);

		DEBUG_PRINT("sanity check : ", " after tac to intel64");
		Pseudo pseudo(file_name,intel.program,&arena,type_check.table);
		DEBUG_PRINT("sanity check : ", " after replace pseudo");
		FixUp fix(file_name,pseudo.program,&arena);

		DEBUG_PRINT("sanity check : ", " after fix inst");
		Codegen gen(file_name,intel.program);
		DEBUG_PRINT("sanity check : ", " after codegen");
		StringToFile(file_name.substr(0, file_name.length() - 3) + ".asm",gen.string);
		*/
	}
}
