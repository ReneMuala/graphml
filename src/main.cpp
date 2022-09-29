#include <string>
#include <iostream>
#include "parser.hpp"
#include "cairoInterpreter0.0.1.hpp"
#include "gisl.hpp"

int main(int argc, const char ** argv){
    if(argc == 2){
        Parser parser = Parser(std::string(argv[1]));
        CairoInterpreter001 c001 = CairoInterpreter001(parser);
        c001.main();
    } else {
        std::cerr << "Graphml Beta (0.0.1) Copyright (c) 2022 René Descartes Muala\nUsage: \n\t" << argv[0] << " <filename>\nWhere <filename> is a valid Graphml file.";
    }
    // for(auto [k, v] : parser.getXmlNodeAttributes(parser.document, parser.cur)){
    //     std::cout << k << ": " << v << std::endl; 
    // }
    
//    std::cout << gisl::evaluateAtomicLogicExpression("not(0,9)") << std::endl;
    // auto vec = 
    // gisl::evaluateExpression(
    // R"(-( 60% ), neg(10%))"
    // );

    // gisl::evaluateExpression(R"(
        
        


    // )");

    // for(auto val : vec){
    //     std::cout << val << "\t";
    // }

}
