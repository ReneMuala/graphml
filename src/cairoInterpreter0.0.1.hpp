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
    std::unordered_map<std::string, cairo_pattern_t *> gradients;

    public:

    enum GradientType {
        Linear,
        Radial,
        Unknown,
    };
    
    CairoInterpreter001(Parser & parser);

    void addGradient(std::string id, cairo_pattern_t * gradient);
    cairo_pattern_t * getGradientById(std::string id);
    void addImage(std::string id, std::pair<cairo_t *, cairo_surface_t *> image);
    void imageMain(cairo_t * cr);
    void handleImageRect(cairo_t * cr);
    void handleImageArc(cairo_t * cr);
    void handleImageFill(cairo_t * cr);
    void handleImageStroke(cairo_t * cr);
    void handleImageExport(cairo_t * cr);
    virtual void main() override;
    virtual void handleGradient() override;
    virtual void handlePattern() override {}
    virtual void handleImage() override;
    virtual void handleFont() override {}
    
    virtual void pushGroup() override {}
    virtual void pushPop() override {}
};