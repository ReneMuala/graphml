#include "gisl.hpp"
#include "operators.hpp"
#include <iostream>
#include "error.hpp"
#include <math.h>
#include <unordered_map>
#include <time.h>

namespace gisl
{
    static bool hasInit = false;
    static const std::regex specialChars = std::regex("(\\^|\\?|\\*|\\+|\\||\\(|\\))");
    time_t nowt = time(NULL);
    tm * now = gmtime(&nowt);
    std::unordered_map<std::string, double> variables = {
        {"@beta", 1},
        {"@version", 1e-3},
        {"@pi", double(M_PI)},
        {"@e", double(M_E)},
        {"@year", now->tm_year+1900},
        {"@mount", now->tm_mon},
        {"@day", now->tm_mday},
        {"@weekday", now->tm_wday},
        {"@hour", now->tm_hour},
        {"@minute", now->tm_min},
        {"@second", now->tm_sec},
    };
    static const std::regex setRegex = std::regex("((set)\\s*\\(\\w+,(\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\s*\\))|(\\(\\w+\\s*(=|<<|<=)\\s*(\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\s*\\),?)", std::regex::icase);
    static const std::regex getRegex = std::regex("((get)\\s*\\(\\s*@?[a-zA-Z_]\\w*\\s*\\))|(\\(\\s*@?[a-zA-Z_]\\w*\\s*\\))", std::regex::icase);
    static const std::regex loopRegex = std::regex("\\s*(loop)?\\s*\\[\\d+\\]\\s*\\{[^\\{\\}]*\\}", std::regex::icase);
    static const std::regex unhandledLoopRegex = std::regex("\\s*(loop)?\\s*\\[\\s*\\d+\\s*\\]\\s*\\{.*\\}", std::regex::icase);

    void clearLocalVars();
} // namespace gils

void gisl::init(){
    hasInit = true;
    srand(time(NULL));
}

void gisl::clearLocalVars(){
    static const std::regex localVarRegex = std::regex("_\\w+");
    for (auto && [key, value] : gisl::variables)
    {
        if(std::regex_match(key, localVarRegex)){
            value = 0;
        }
    }
    
}

std::vector<double>  gisl::getSet(std::string str, int maxSize ){
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
            if(set.size() == maxSize) break;
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
    static const std::regex modRegex = std::regex("\\s*(mod|\\%)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    double result = 0;

    auto numberSet = getSet(expression);

    if(std::regex_match(expression, addRegex)){
        result = 0;
        for (auto & value : numberSet){
            result+=value;
        }
    } else if(std::regex_match(expression, powRegex)){
        result = (numberSet.size()) ? numberSet[0] : 0;
        for(int i = 1 ; i < numberSet.size() ; i++)  result=pow(result, numberSet[i]);
    } else if(std::regex_match(expression, modRegex)){
        result = (numberSet.size()) ? numberSet[0] : 0;
        for(int i = 1 ; i < numberSet.size() ; i++)  result=(int)result%(int)numberSet[i];
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

std::string gisl::evaluateAtomicColorExpression(std::string expression){
    // static const std::regex rgbRegex = std::regex("\\s*(rgb)\\s*\\((\\s*\\d+(\\.\\d+?)?\\s*,?){3}\\)", std::regex::icase);
    // static const std::regex rgbaRegex = std::regex("\\s*(rgba)\\s*\\((\\s*\\d+(\\.\\d+?)?\\s*,?){4}\\)", std::regex::icase);

    auto numberSet = getSet(expression);

    std::string colorComponents;

    for(auto && number : numberSet){
        colorComponents +=  std::to_string(number/255.0) + " ";
    } return colorComponents;
}


bool gisl::evaluateAtomicLogicExpression(std::string expression){
    static const std::regex andRegex = std::regex("\\s*(and|&)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex orRegex = std::regex("\\s*(or|\\|)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex xorRegex = std::regex("\\s*(xor|\\^)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)", std::regex::icase);
    static const std::regex notRegex = std::regex("\\s*(not|\\~|\\!)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){1}\\)", std::regex::icase);

    bool result = false;
    
    const auto numberSet = getSet(expression);

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

double gisl::evaluateAtomicLibMathExpression(std::string expression){
    static const std::regex absRegex = std::regex("\\s*(abs)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);

    static const std::regex sinRegex = std::regex("\\s*(sin)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    static const std::regex asinRegex = std::regex("\\s*(asin)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    static const std::regex cosRegex = std::regex("\\s*(cos)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    static const std::regex acosRegex = std::regex("\\s*(acos)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    static const std::regex tanRegex = std::regex("\\s*(tan)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    static const std::regex tanhRegex = std::regex("\\s*(tanh)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    static const std::regex atanRegex = std::regex("\\s*(atan)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);

    static const std::regex coshRegex = std::regex("\\s*(cosh)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    
    static const std::regex ceilRegex = std::regex("\\s*(ceil)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    static const std::regex floorRegex = std::regex("\\s*(floor)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);

    static const std::regex expRegex = std::regex("\\s*(exp)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);

    static const std::regex sqrtRegex = std::regex("\\s*(sqrt)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);

    static const std::regex degRegex = std::regex("\\s*(deg)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);
    
    static const std::regex radRegex = std::regex("\\s*(rad)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);

    static const std::regex negRegex = std::regex("\\s*(neg|-)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)", std::regex::icase);

    static const std::regex percentRegex = std::regex("\\s*(\\s*\\d+(\\.\\d+?)?\\s*,?)\\%", std::regex::icase);

    const auto numberSet = getSet(expression, 1);

    double result = 0;
    if(std::regex_match(expression, absRegex)){
        result = numberSet[0] < 0 ? -numberSet[0] : numberSet[0];
    } else if(std::regex_match(expression, sinRegex)){
        result = sin(numberSet[0]);
    } else if(std::regex_match(expression, asinRegex)){
        result = asin(numberSet[0]);
    } else if(std::regex_match(expression, cosRegex)){
        result = cos(numberSet[0]);
    } else if(std::regex_match(expression, acosRegex)){
        result = acos(numberSet[0]);
    } else if(std::regex_match(expression, tanRegex)){
        result = tan(numberSet[0]);
    } else if(std::regex_match(expression, tanhRegex)){
        result = tanh(numberSet[0]);
    } else if(std::regex_match(expression, atanRegex)){
        result = atan(numberSet[0]);
    } else if(std::regex_match(expression, coshRegex)){
        result = cosh(numberSet[0]);
    } else if(std::regex_match(expression, ceilRegex)){
        result = ceil(numberSet[0]);
    } else if(std::regex_match(expression, floorRegex)){
        result = floor(numberSet[0]);
    } else if(std::regex_match(expression, expRegex)){
        result = exp(numberSet[0]);
    } else if(std::regex_match(expression, sqrtRegex)){
        result = sqrt(numberSet[0]);
    } else if(std::regex_match(expression, radRegex)){
        result = numberSet[0] * (M_PI/180.0);
    } else if(std::regex_match(expression, degRegex)){
        result = numberSet[0] / (M_PI/180.0);
    } else if(std::regex_match(expression, negRegex)){
        result = -numberSet[0];
    } else if(std::regex_match(expression, percentRegex)){
        result = numberSet[0]/100.0;
    } else {
        gmlWarn(__FUNCTION__,__FILE__,__LINE__, (std::string("Unknown libMath operation: ")+expression).c_str());    
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
    if(varName == "@rand"){
        return (double)(rand()%10000)/(double)10000;
    } for(auto [key, value] : gisl::variables){
        if(key == varName){
            return value;
        }
    } gmlWarn(__FUNCTION__,__FILE__,__LINE__, (std::string("Undefined variable in expression: ")+expression).c_str());    
    return 0;
}

std::string gisl::evaluateLoop(std::string expression){
    static const std::regex codeBlockRegex = std::regex("\\{[^\\{]*\\}", std::regex::icase);
    if(std::regex_match(expression, loopRegex)){
        const auto numberSet = getSet(expression, 1);
        std::smatch codeBlockSmatch;
        std::vector<double> codeBlockResult, codeBlockTempResult;
        if(std::regex_search(expression, codeBlockSmatch, codeBlockRegex)){
            for(int i = 0; i < numberSet[0] ; i++){
                gisl::variables["@i"] = i;
                codeBlockTempResult = evaluateExpression(codeBlockSmatch.str().substr(1,codeBlockSmatch.str().size()-2));
                for (auto &&value : codeBlockTempResult){
                    codeBlockResult.push_back(value);
                }
            } gisl::variables["@i"] = 0;
        } expression.clear();
        for (auto &&value : codeBlockResult) expression+=(" "+std::to_string(value)+" ");
    } return expression;
}

std::vector<double> gisl::evaluateExpression(std::string expression){
    if(!gisl::hasInit){
        gisl::init();
    }
    expression+='\0';
    static const std::regex arithExprRegex = std::regex("\\s*(add|sum|mod|\\%|pow|\\+|sub|-|mul|\\*|\\*\\*|div|/)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\)",std::regex::icase);
    static const std::regex logicExprRegex = std::regex("\\s*((and|&|or|\\||xor|\\^)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){2,}\\))|((not|\\~|\\!)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?){1}\\))",
    std::regex::icase);
    static const std::regex libMathExprRegex = std::regex("(\\s*(abs|sin|asin|cos|acos|tan|tanh|atan|cosh|ceil|floor|neg|-|exp|sqrt|rad|deg)\\s*\\((\\s*-?\\d+(\\.\\d+?)?\\s*,?)\\)|\\s*(\\s*\\d+(\\.\\d+?)?\\s*,?)\\%)", std::regex::icase);
    static const std::regex colorExprRegex = std::regex("(\\s*(rgb)\\s*\\((\\s*\\d+(\\.\\d+?)?\\s*,?){3}\\)|\\s*(rgba)\\s*\\((\\s*\\d+(\\.\\d+?)?\\s*,?){4}\\))");
    static const std::regex percentRegex = std::regex("\\s*(\\s*\\d+(\\.\\d+?)?\\s*,?)\\%"); 
    std::string targetStr, targetResulStr;
    double targetResult;
    std::regex targetRegex;
    std::smatch atomicExpression, extraAtomicExpressions[3];
    bool previousMatch = false, unhandledLoop = false;
    InstructionType instructionType = None;
    std::regex_constants::match_flag_type replace_flag;
    while(true){
        replace_flag = std::regex_constants::match_default;
        if(std::regex_search(expression, atomicExpression,libMathExprRegex)){
            instructionType = LibMath;
            // std::cout << "LibMath: " << atomicExpression.str() << std::endl;
            if(std::regex_match(atomicExpression.str(), percentRegex)){
                replace_flag = std::regex_constants::format_first_only;
            }
        } else if(std::regex_search(expression, atomicExpression,arithExprRegex)){
            instructionType = Arithmetic;
            // std::cout << "Arithmetic: " << atomicExpression.str() << std::endl;
        } else if(std::regex_search(expression, atomicExpression,colorExprRegex)){
            instructionType = Color;
            // std::cout << "Color: " << atomicExpression.str() << std::endl;
        } else if(std::regex_search(expression, atomicExpression,logicExprRegex)){
            instructionType = Logic;
            // std::cout << "Logic: " << atomicExpression.str() << std::endl;
        } else if (std::regex_search(expression,setRegex) || std::regex_search(expression,getRegex) || std::regex_search(expression,gisl::loopRegex)){
            if(std::regex_search(expression, atomicExpression, gisl::setRegex)){
                instructionType = Set;
                previousMatch = true;
            } if(std::regex_search(expression, extraAtomicExpressions[0], gisl::getRegex)){
                if((!previousMatch) || (previousMatch && atomicExpression.prefix().length() > extraAtomicExpressions[0].prefix().length())) {
                    atomicExpression = extraAtomicExpressions[0];
                    instructionType = Get;
                    previousMatch = true;
                }
            } 
            std::regex_search(expression, extraAtomicExpressions[2],gisl::unhandledLoopRegex);

            if(std::regex_search(expression, extraAtomicExpressions[1],gisl::loopRegex)){
                if((!previousMatch) || (previousMatch && (atomicExpression.prefix().length() > extraAtomicExpressions[1].prefix().length() && extraAtomicExpressions[0].prefix().length() > extraAtomicExpressions[1].prefix().length()) || extraAtomicExpressions[2].prefix().length() < extraAtomicExpressions[1].prefix().length())) {
                    atomicExpression = extraAtomicExpressions[1];
                    instructionType = Loop;
                }
            } 
            
            previousMatch = false;
        } else {
            break;
        }
        
        if(instructionType == Loop){
            expression = atomicExpression.prefix().str()+ " " + evaluateLoop(atomicExpression.str()) + " " +atomicExpression.suffix().str();
            
        } else {
            targetStr = atomicExpression.str();
            targetRegex = std::regex(std::regex_replace(targetStr, gisl::specialChars, "\\$&"));
            if(instructionType == Logic || instructionType == Arithmetic || instructionType == LibMath){
                switch (instructionType)
                {
                case Logic:
                    targetResult = evaluateAtomicLogicExpression(targetStr);
                    break;
                case Arithmetic:
                    targetResult = evaluateAtomicArithmeticExpression(targetStr);
                    break;
                case LibMath:
                    targetResult = evaluateAtomicLibMathExpression(targetStr);
                    break;
                default: break;
                }  expression = std::regex_replace(expression, targetRegex, " "+std::to_string(targetResult)+" ",replace_flag);
            } else if(instructionType == Color){
                targetResulStr = evaluateAtomicColorExpression(targetStr);
                expression = std::regex_replace(expression, targetRegex," "+targetResulStr+" ",std::regex_constants::format_first_only);
            } else if(instructionType == Set){
                evaluateSet(targetStr);
                expression = std::regex_replace(expression, targetRegex,"",std::regex_constants::format_first_only);
            } else if(instructionType == Get){
                expression = std::regex_replace(expression, targetRegex, " "+std::to_string(evaluateGet(targetStr))+" ", std::regex_constants::format_first_only);
            }
        }
        // std::cout << expression << std::endl;
    } 
    clearLocalVars();
    return getSet(expression);
}