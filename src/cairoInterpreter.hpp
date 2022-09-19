#pragma once
#include <string>
#include "parser.hpp"
#include <regex>
#include <vector>

/**
 * @brief Cairo interpreters abstract class 
 * 
 */
class CairoInterpreter {
    protected:
    Parser & parser;

    static const std::regex gradientRegex;
    static const std::regex linearGradientRegex;
    static const std::regex radialGradientRegex;

    public:
    /**
     * @brief Constructs a new Cairo interpreter
     * 
     * @param parser the parser
     */
    CairoInterpreter(Parser & parser);

    virtual double parseNumericExpression(std::string) = 0;

    virtual void main() = 0;
    virtual void handleGradient() = 0;
    virtual void handleLinearGradient() = 0;
    virtual void handleRadialGradient() = 0;
    virtual void handlePattern() = 0;
    virtual void handleImage() = 0;
    virtual void handleFont() = 0;
    
    virtual void pushGroup() = 0;
    virtual void pushPop() = 0;
};