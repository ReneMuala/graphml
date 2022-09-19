#pragma once

#include "cairoInterpreter.hpp"
#include <cairo/cairo.h>
#include <utility>
#include <unordered_map>

/**
 * @brief First generation cairo interpreter.
 * 
 */
class CairoInterpreter001: public CairoInterpreter {

    /**
     * @brief images
     * 
     */
    std::unordered_map<std::string, std::pair<cairo_t *, cairo_surface_t *>> images;

    public:
    
    CairoInterpreter001(Parser & parser);

    

    virtual double parseNumericExpression(std::string) override;

    virtual void main() override;
    virtual void handleGradient() override;
    virtual void handleLinearGradient() override {}
    virtual void handleRadialGradient() override {}
    virtual void handlePattern() override {}
    virtual void handleImage() override {}
    virtual void handleFont() override {}
    
    virtual void pushGroup() override {}
    virtual void pushPop() override {}
};