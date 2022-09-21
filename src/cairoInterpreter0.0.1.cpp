#include "cairoInterpreter0.0.1.hpp"
#include <iostream>
#include <regex>
#include "operators.hpp"

CairoInterpreter001::CairoInterpreter001(Parser & parser): CairoInterpreter(parser){

}

void CairoInterpreter001::main() {
    while(parser.nodeHasLink(Parser::NodeLink::NEXT)){
        std::cout << parser.getXmlNodeName() << std::endl;
        if(std::regex_match(parser.getXmlNodeName(), gradientRegex)){
            handleGradient();
        }
        parser.getNodeLink(Parser::NodeLink::NEXT);
    }
}

void CairoInterpreter001::handleGradient() {
    auto attributeSet = parser.getXmlNodeAttributes();
    std::string id, type;
    id = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "id", attributeSet);
    type = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "type", attributeSet);
    
    parser.getNodeLink(Parser::NodeLink::CHILDREN);
    while(parser.nodeHasLink(Parser::NodeLink::NEXT)){
        std::cout << "\t" <<  parser.getXmlNodeName() << std::endl;
        parser.getNodeLink(Parser::NodeLink::NEXT);
    }
    parser.getNodeLink(Parser::NodeLink::PARENT);
}
