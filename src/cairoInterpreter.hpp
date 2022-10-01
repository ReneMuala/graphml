#pragma once
#include <string>
#include "parser.hpp"
#include <regex>
#include <vector>
#include <string>

/**
 * @brief Cairo interpreters abstract class 
 * 
 */
class CairoInterpreter {
    protected:
    Parser & parser;

    public:
    /**
     * @brief Constructs a new Cairo interpreter
     * 
     * @param parser the parser
     */
    CairoInterpreter(Parser & parser);

    virtual void main() = 0;
    virtual void handleGradient() = 0;
    virtual void handlePattern() = 0;
    virtual void handlePath() = 0;
    virtual void handleImage() = 0;
    virtual void handleFont() = 0;
};