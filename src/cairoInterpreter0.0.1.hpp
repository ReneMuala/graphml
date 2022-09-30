#pragma once

#include "cairoInterpreter.hpp"
#include <cairo/cairo.h>
#include <utility>
#include <unordered_map>
#include <vector>
#include <stack>

/**
 * @brief First generation cairo interpreter.
 * 
 */
class CairoInterpreter001: public CairoInterpreter {
public:

    enum GradientType {
        Linear,
        Radial,
        Unknown,
    };

    enum DrawEvent {
        Fill,
        FillPreserve,
        Stroke,
        StrokePreserve,
    };

    struct StrokeProperties 
    {
        double lineWidth;
        cairo_line_cap_t cap;
        cairo_line_join_t join;
        StrokeProperties(double lineWidth, cairo_line_cap_t cap,cairo_line_join_t join): lineWidth(lineWidth), cap(cap), join(join){}
    };
    
private:

    /**
     * @brief images
     * 
     */
    std::unordered_map<std::string /* id */, std::pair<cairo_t *, cairo_surface_t *>> images;
    std::unordered_map<std::string /* id */, cairo_pattern_t *> gradients;
    std::unordered_map<std::string /* id */, xmlNode*> paths;
    std::stack<std::pair<DrawEvent, std::optional<StrokeProperties> /* line width ? >*/>> drawEventStack;

    double current_device_width , current_device_height;

    bool first_path_element, apply_main_show_logs;

    /**
     * @brief Stores the apply current apply stack size;
     */
    size_t embeddedApplyStackSize;

    public:
    
    CairoInterpreter001(Parser & parser);
    ~CairoInterpreter001();

    void addGradient(std::string id, cairo_pattern_t * gradient);
    cairo_pattern_t * getGradientById(std::string id);
    std::pair<cairo_t *, cairo_surface_t *>* getImageByid(std::string id);
    void addImage(std::string id, std::pair<cairo_t *, cairo_surface_t *> image);
    void addPath(std::string id, xmlNode* node);
    bool getTextData(double & x, double & y, double & size, cairo_font_slant_t & slant, cairo_font_weight_t & weight, std::string & value, std::string & font);
    bool getRectCoords(double & x, double & y, double &  w, double  & h);
    bool getArcCoords(std::vector<double>  & center, double & radius, double  & begin, double & end);

    void applyClose(cairo_t * cr);
    void applyCurve(cairo_t * cr);
    void applyLine(cairo_t * cr);
    void applyMove(cairo_t * cr);
    void applyRect(cairo_t * cr);
    void applyArc(cairo_t * cr);
    void applyMain(cairo_t * cr, xmlNode* cur);

    bool strokeSetLineCap(cairo_t * cr,  cairo_line_cap_t & cap,std::unordered_map<std::string, std::string> &attributeSet);
    bool strokeSetLineJoin(cairo_t * cr,  cairo_line_join_t & join, std::unordered_map<std::string, std::string> &attributeSet);
    void clearDrawEventStack();

    void imageMain(cairo_t * cr);
    void handleImageTranslate(cairo_t * cr);
    void handleImageRotate(cairo_t * cr);
    void handleImageScale(cairo_t * cr);
    void handleImageRect(cairo_t * cr);
    void handleImageText(cairo_t * cr);
    void handleImageArc(cairo_t * cr);

    // fill, stroke, paint common arguments
    bool handleImageDrawArguments(cairo_t * cr);

    void handleImageFill(cairo_t * cr);
    void handleImagePaint(cairo_t * cr);
    void handleImageStroke(cairo_t * cr);
    void handleImageExport(cairo_t * cr);
    void handleImport(); 
    void handleImageApply(cairo_t * cr);

    virtual void main() override;
    virtual void handleGradient() override;
    virtual void handlePath() override;
    virtual void handlePattern() override {}
    virtual void handleImage() override;
    virtual void handleFont() override {}
};