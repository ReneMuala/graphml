#pragma once
#include <vector>
#include <regex>
/**
 * @brief Graphml immediate script language handler.
 * Used to handle gisl instructions passed to graphml properties. 
 * 
 */
namespace gisl {

    enum InstructionType {
        /**
         * @brief Arithmetic expressions.  
         * Supported operations: add, sub, mul, div.
         */
        Arithmetic,
        /**
         * @brief Logic expressions.
         * Supported operations: and, or, xor, not.
         */
        Logic,

        /**
         * @brief LobMath expressions.
         * Supported operations: abs, sin, asin, cos, acos, tan, tanh, atan, cosh, ceil, floor, exp, sqrt, deg.
         */
        LibMath,

        /**
         * @brief A loop routine.
         * [N]{ABC}
         * @see evaluateLoop
         */
        Loop,

        /**
         * @see evaluateSet
         */
        Set,

        /**
         * @see evaluateGet
         */
        Get,

        None,
    };
    void init();
    /**
     * @brief Returns the values of a simple set.
     * 
     * "xyz(0,1,2)" => [0,1,2]
     * 
     * @param expression the expression to analyze.
     * @param maxSize the maximum length allowed, -1 means infty.
     * @return std::vector<double> 
     */
    std::vector<double> getSet(std::string expression, int maxSize = -1);

    /**
     * @brief evaluates a single arithmetic expression.
     * 
     * Supported operations: add, sub, mul, div.
     * 
     * @param expression the expression to be analyzed.
     * @return double
     * 
     */
    double evaluateAtomicArithmeticExpression(std::string expression);

    /**
     * @brief evaluates a single logic expression.
     * 
     * Supported operations: and, or, xor, not
     * 
     * @param expression the expression to be analyzed.
     * @return double
     * 
     */
    bool evaluateAtomicLogicExpression(std::string expression);

        /**
     * @brief evaluates a single trigonometric expression.
     * 
     * Supported operations: sin, cos, tan,
     * 
     * @param expression the expression to be analyzed.
     * @return double
     * 
     */
    double evaluateAtomicLibMathExpression(std::string expression);

    /**
     * @brief evaluates a set expression.
     * 
     * A set operation can be used to set a gisl variable data.
     * 
     * @param expression the expression to be analyzed.
     * 
     */
    void evaluateSet(std::string expression);

    /**
     * @brief evaluates a get expression.
     * 
     * A get operation can be used to get a gisl variable data.
     * 
     * @param expression the expression to be analyzed.
     * @return double value
     * 
     */
    double evaluateGet(std::string expression);

    /**
     * @brief evaluates a loop expression.
     * 
     * A loop in gisl consists in a expression that is going to be repeated n times.
     * Syntax: "[N]{ABC}"
     * Where N is the number of repetitions and ABC is the code to repeat.
     * 
     * @param expression the expression to be analyzed.
     * @return std::string the new expression.
     * 
     */
    std::string evaluateLoop(std::string expression);
    
    /**
     * @brief evaluates a set of arithmetic expressions.
     * 
     * @param expression the arithmetic expression set.
     * @return std::vector<double> 
     */
    std::vector<double> evaluateExpression(std::string expression);
};