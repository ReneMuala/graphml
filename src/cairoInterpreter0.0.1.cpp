#include "cairoInterpreter0.0.1.hpp"
#include <iostream>
#include <regex>
#include "operators.hpp"

CairoInterpreter001::CairoInterpreter001(Parser & parser): CairoInterpreter(parser){

}

std::vector<double> getNumbers(std::string str){
    std::vector<double> numbers;
    std::string number = ""; const std::string numbersStr = "-0123456789";
    for (auto let : str) {
        if(let == numbersStr){
            number+=let;
        } else {
            if(!number.empty()){
                numbers.push_back(atof(number.c_str()));
                number="";
            }
        }
    } return numbers;
}

double CairoInterpreter001::parseNumericExpression(std::string str) {
    static const std::regex addRegex = std::regex("\\s*(add|sum|\\+)\\s*\\((\\s*-?\\d+\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex subRegex = std::regex("\\s*(sub|\\-)\\s*\\((\\s*-?\\d+\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex multiRegex = std::regex("\\s*(mul|\\*)\\s*\\((\\s*-?\\d+\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex divRegex = std::regex("\\s*(div|/)\\s*\\((\\s*-?\\d+\\s*,?){2,}\\)", std::regex::icase);
    double res = 0;
    auto set = getNumbers(str);
    if(std::regex_match(str, addRegex)){
        res = 0;
        for (auto num : set)  res+=num;
    } else if(std::regex_match(str, subRegex)){
        res = 0;
        for (auto num : set) res-=num;
    } else if(std::regex_match(str, multiRegex)){
        res = 1;
        for (auto num : set) res*=num;
    } else if(std::regex_match(str, divRegex)){
        res = 1;
        for (auto num : set) {
            if(num != 0) res/=num;
            else return  0;
        }
    } return res;
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
    std::string id;
    id = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "id", attributeSet);
    
    parser.getNodeLink(Parser::NodeLink::CHILDREN);
    while(parser.nodeHasLink(Parser::NodeLink::NEXT)){
        std::cout << "\t" <<  parser.getXmlNodeName() << std::endl;
        parser.getNodeLink(Parser::NodeLink::NEXT);
    }
    parser.getNodeLink(Parser::NodeLink::PARENT);
}
