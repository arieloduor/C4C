#include "makeda.hpp"
#include "../../makasm/include/codegen.hpp"
#include "../../../../utils/include/file_to_string.hpp"

int main(int argc,char **argv)
{
    std::string file_name(argv[1]);

    FileToString fs(file_name);
	std::string file_source = fs.read();

    //std::string file_source = "push 50\npush 10\npush 10\nprint\nadd\nprint\nmul\nprint\n";
    
    Lexer lexer(file_name,file_source);
    Codegen codegen(file_name,lexer.scan_tokens());
    Makeda makeda(64);
    
}