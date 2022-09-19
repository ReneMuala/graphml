#include "gisl.hpp"
#include "operators.hpp"
#include <iostream>
#include "error.hpp"
#include <math.h>
#include <unordered_map>
#include <time.h>

namespace gisl
{
    static const std::regex specialChars = std::regex("(\\^|\\?|\\*|\\+|\\||\\(|\\))");
    time_t nowt = time(NULL);
    tm * now = gmtime(&nowt);
    std::unordered_map<std::string, double> variables = {
        {"@beta", 1},
        {"@year", now->tm_year+1900},
        {"@mount", now->tm_mon},
        {"@day", now->tm_mday},
        {"@weekday", now->tm_wday},
        {"@hour", now->tm_hour},
        {"@minute", now->tm_min},
        {"@second", now->tm_sec},
    };
    static const std::regex setRegex = std::regex("((set)\\s*\\(\\w+,(\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\s*\\))|(\\(\\w+\\s*(=|<-|<=)\\s*(\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\s*\\),?)", std::regex::icase);
    static const std::regex getRegex = std::regex("((get)\\s*\\(\\s*@?\\w+\\s*\\))|(\\(\\s*@?\\w+\\s*\\))", std::regex::icase);
} // namespace gils

std::vector<double>  gisl::getSet(std::string str){
    std::vector<double> set;
    std::string number; 
    static const std::string numbersStr = "-0123456789.";
    for (auto let : str) {
        if(let == numbersStr){
            number+=let;
        } else {
            if(!number.empty() && number!="-"){
                set.push_back(atof(number.c_str()));
            }  number.clear();
        }
    }
    return set;
}

double gisl::evaluateAtomicArithmeticExpression(std::string expression){
    static const std::regex addRegex = std::regex("\\s*(add|sum|\\+)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex subRegex = std::regex("\\s*(sub|-)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex multiRegex = std::regex("\\s*(mul|\\*)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex divRegex = std::regex("\\s*(div|/)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex powRegex = std::regex("\\s*(pow|\\*\\*)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);

    double result;

    auto numberSet = getSet(expression);

    if(std::regex_match(expression, addRegex)){
        result = 0;
        for (auto & value : numberSet){
            result+=value;
        }
    } else if(std::regex_match(expression, powRegex)){
        result = (numberSet.size()) ? numberSet[0] : 0;
        for(int i = 1 ; i < numberSet.size() ; i++)  result=pow(result, numberSet[i]);
    } else if(std::regex_match(expression, subRegex)){
        result = (numberSet.size()) ? numberSet[0] : 0;
        for(int i = 1 ; i < numberSet.size() ; i++)  result-=numberSet[i];
    } else if(std::regex_match(expression, multiRegex)){
        result = 1;
        for (auto & value : numberSet){
            result*=value;
        }
    } else if(std::regex_match(expression, divRegex)){
        result = 0;
        for (auto & value : numberSet){
            if(!result) result = value;
            else if(value != 0)  result/=value;
            else {
                result = 0;
                break;
            }
        }
    } else {
        gmlWarn(__FUNCTION__,__FILE__,__LINE__, (std::string("Unknown arithmetic operation: ")+expression).c_str());    
    } return result;
}

bool gisl::evaluateAtomicLogicExpression(std::string expression){
    static const std::regex andRegex = std::regex("\\s*(and|&)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex orRegex = std::regex("\\s*(or|\\|)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex xorRegex = std::regex("\\s*(xor|\\^)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex notRegex = std::regex("\\s*(not|\\~|\\!)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){1}\\)", std::regex::icase);

    bool result = false;
    
    auto numberSet = getSet(expression);

    if(numberSet.size()) result = numberSet[0];

    if(std::regex_match(expression, andRegex)){
        for(int i = 1 ; i < numberSet.size() ; i++)result &= (bool)numberSet[i];
    } else if(std::regex_match(expression, orRegex)){
        for(int i = 1 ; i < numberSet.size() ; i++)result = result || (bool)numberSet[i];
    } else if(std::regex_match(expression, xorRegex)){
        for(int i = 1 ; i < numberSet.size() ; i++)result ^= (bool)numberSet[i];
    } else if(std::regex_match(expression, notRegex)){
        result = !result;
    } else {
       gmlWarn(__FUNCTION__,__FILE__,__LINE__, (std::string("Unknown logic operation: ")+expression).c_str());    
    } return result;
}

void gisl::evaluateSet(std::string expression){
    static const std::regex ignoreInSetRegex = std::regex("(set\\s*\\()|[^[:alpha:]_]");
    auto value = getSet(expression);
    std::string varName = std::regex_replace(expression, ignoreInSetRegex, "");
    gisl::variables[varName] = value[0];
}

double gisl::evaluateGet(std::string expression){
    static const std::regex ignoreInGetRegex = std::regex("(get\\s*\\(|\\(|\\)|,)");
    std::string varName = std::regex_replace(expression, ignoreInGetRegex, "");
    for(auto [key, value] : gisl::variables){
        if(key == varName){
            return value;
        }
    } return 0;
}

std::vector<double> gisl::evaluateExpression(std::string expression){
    expression+='\0';
    static const std::regex arithExprRegex = std::regex("\\s*(add|sum|pow|\\+|sub|-|mul|\\*|\\*\\*|div|/)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)",std::regex::icase);
    static const std::regex logicExprRegex = std::regex("\\s*((and|&|or|\\||xor|\\^)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\))|((not|\\~|\\!)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){1}\\))",
    std::regex::icase);
    
    std::string targetStr;
    double targetResult;
    std::regex targetRegex;
    std::smatch atomicExpression, extraAtomicExpression;
    bool setDetected = false;
    InstructionType instructionType = None;
    while(true){
        if(std::regex_search(expression, atomicExpression,arithExprRegex)){
            instructionType = Arithmetic;
        } else if(std::regex_search(expression, atomicExpression,logicExprRegex)){
            instructionType = Logic;
        } else if (std::regex_search(expression,setRegex) || std::regex_search(expression,getRegex)){
            if(std::regex_search(expression, atomicExpression, gisl::setRegex)){
                instructionType = Set;
                setDetected = true;
            } if(std::regex_search(expression, extraAtomicExpression, gisl::getRegex)){
                if((!setDetected) || (setDetected && atomicExpression.prefix().length() > extraAtomicExpression.prefix().length())) {
                    atomicExpression = extraAtomicExpression;
                    instructionType = Get;
                }
            }
            setDetected = false;
        } else {
            break;
        }

        targetStr = atomicExpression.str();
        targetRegex = std::regex(std::regex_replace(targetStr, gisl::specialChars, "\\$&"));
        if(instructionType == Logic || instructionType == Arithmetic){
            targetResult = (instructionType == Arithmetic) ? evaluateAtomicArithmeticExpression(targetStr) : evaluateAtomicLogicExpression(targetStr);
            expression = std::regex_replace(expression, targetRegex, std::to_string(targetResult));
        } else if(instructionType == Set){
            evaluateSet(targetStr);
            expression = std::regex_replace(expression, targetRegex, "",std::regex_constants::format_first_only);
        } else if(instructionType == Get){
            expression = std::regex_replace(expression, targetRegex, std::to_string(evaluateGet(targetStr)), std::regex_constants::format_first_only);
        }
    } return getSet(expression);
}