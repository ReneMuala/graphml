#include <string>
#include <iostream>
#include "parser.hpp"
#include "cairoInterpreter0.0.1.hpp"
#include "gisl.hpp"

int main(){
    /*Parser parser = Parser(std::string("res/graphml.gml"));
    // for(auto [k, v] : parser.getXmlNodeAttributes(parser.document, parser.cur)){
    //     std::cout << k << ": " << v << std::endl; 
    // }
    CairoInterpreter001 c001 = CairoInterpreter001(parser);

    c001.main();
    */

//    std::cout << gisl::evaluateAtomicLogicExpression("not(0,9)") << std::endl;

    auto vec = 
    // gisl::evaluateExpression(
    // "add(2,mul(add(9,90),div(5,mul(0,9))))"
    // );

    gisl::evaluateExpression(R"(set(w,200)get(w),div(get(w),2))");
    for(auto val : vec){
        std::cout << val << "\t";
    }
}