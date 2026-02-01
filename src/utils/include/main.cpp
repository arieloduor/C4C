#include "argparse.hpp"



int main(int argc,char **argv)
{
    Argparser argparse("c4c","c4c compiler"," pass a file","1.0.0");
    argparse.add_argument(Argument("t","target","sets the target platform"," help : set target",ArgumentType::STRING));
    argparse.parse(argc,argv);
    std::cout << argparse.get_help("target") << std::endl;
}