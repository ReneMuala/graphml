#include "cairoInterpreter0.0.1.hpp"
#include <iostream>
#include <regex>
#include <vector>
#include "operators.hpp"
#include "gisl.hpp"
#include "error.hpp"

static const std::regex gradientRegex = std::regex("(gradient)|(grad)", std::regex::icase);

bool getVerifiedSingleRelativeValue(std::string valueName, double & value,  std::vector<double> valueSet, Parser & parser){
    bool statusOk = true;

    if (valueSet.size() != 1) {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect number of "+valueName+" arguments, expected 1. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    } else {
        if (valueSet[0] < 0 || valueSet[0] > 1) {
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> value for "+valueName+" argument, is not in [0,1]. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        } 
        value = valueSet[0];
    }
    
    return statusOk;
}

bool verifyArcCenter(std::vector<double> center, Parser & parser){
    if(center.size() != 2){
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (
            std::string("\n--> incorrect number of arc:center arguments, expected 2 \"x,y\"") + std::string(" at line: ") + std::to_string( parser.getXmlNodeLine())
            ).c_str());
        return false;
    } for(auto & value : center){
        if(value < 0 || value > 1){
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> value for arc:center argument, is not in [0,1]. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        }
    }
    return true;
}

bool verifyBeginArgNum(int argNum,  CairoInterpreter001::GradientType type, Parser & parser){
    const int expectedArgNum = (type == CairoInterpreter001::GradientType::Linear) ? 2 : 3;
    if(argNum != expectedArgNum){
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (std::string("\n--> incorrect number of begin arguments, expected") + ((argNum == 2)?" 2 \"x,y\"" : " 3 \"x0,y0,r0\"") + std::string(" at line: ") + std::to_string( parser.getXmlNodeLine())).c_str());
        return false;
    } return true;
}

bool verifyEndArgNum(int argNum,  CairoInterpreter001::GradientType type, Parser & parser){
    const int expectedArgNum = (type == CairoInterpreter001::GradientType::Linear) ? 2 : 3;
    if(argNum != expectedArgNum){
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (std::string("\n--> incorrect number of end arguments, expected") + ((argNum == 2)?" 2 \"w,h\"" : " 3 \"x1,y1,r1\"") + std::string(" at line: ") + std::to_string( parser.getXmlNodeLine())).c_str());
        return false;
    } return true;
}

bool verifyColorStopStep(std::vector<double> colorStopStep,  Parser & parser){
    bool statusOk = true;
    if (colorStopStep.size() != 1) {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect number of colorStop:Step arguments, expected 1. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    } else {
        if (colorStopStep.at(0) < 0 || colorStopStep.at(0) > 1){
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect value for colorStop:Step arguments, the value must to be in [0,1] line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
            statusOk =  false;
        }    
    } return statusOk;
}

bool verifyColor(std::vector<double> color,  Parser & parser){
    bool statusOk = true;
     if (color.size() < 3 || color.size() > 4) {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect number of *:Color arguments, expected 3 (for rgb) or 4 for (rgba) line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    } for(auto & value : color){
        if(value < 0 || value > 1){
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect value for *:Color arguments, the value must to be in [0,1] line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
            statusOk = false;
        }
    } return statusOk;
}

CairoInterpreter001::CairoInterpreter001(Parser & parser): CairoInterpreter(parser){}

void CairoInterpreter001::addGradient(std::string id, cairo_pattern_t * gradient){
    if(gradients.count(id)){
        cairo_pattern_destroy(gradients[id]);
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, ("\n--> repeated gradient id \"" + id + "\", att: the old instance will be deleted.").c_str());
    } gradients[id] = gradient; 
}

cairo_pattern_t * CairoInterpreter001::getGradientById(std::string id){
    if(gradients.count(id)){
        return gradients[id];
    } else {
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, ("\nunable to a gradient with the id \"" + id + "\".").c_str());
    } return NULL;
}

void CairoInterpreter001::addImage(std::string id, std::pair<cairo_t *, cairo_surface_t *> image){
    if(images.count(id)){
        auto && [cr, surf] = images[id];
        cairo_surface_destroy(surf);
        cairo_destroy(cr);
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, ("\n--> repeated image id \"" + id + "\", att: the old instance will be deleted.").c_str());
    } images[id] = image;
}

void CairoInterpreter001::imageMain(cairo_t * cr){
     while(parser.nodeHasLink(Parser::NodeLink::NEXT)){
        // std::cout << parser.getXmlNodeName() << std::endl;
        
        if(!strcasecmp(parser.getXmlNodeName().c_str(), "rect")){
            handleImageRect(cr);
        }  else if(!strcasecmp(parser.getXmlNodeName().c_str(), "arc")){
            handleImageArc(cr);
        }  else if(!strcasecmp(parser.getXmlNodeName().c_str(), "fill")){
            handleImageFill(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "stroke")){
            handleImageStroke(cr);
        }  else if(!strcasecmp(parser.getXmlNodeName().c_str(), "export")){
            handleImageExport(cr);
        } parser.getNodeLink(Parser::NodeLink::NEXT);
    }
}

void CairoInterpreter001::handleImageRect(cairo_t * cr){
    bool statusOk = true;
    double x,y,w,h;
    std::vector<double> valueSet; 
    auto attributeSet = parser.getXmlNodeAttributes();

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "x", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("img:rect:x", x, valueSet, parser)) ? statusOk : false;
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "y", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("img:rect:y", y, valueSet, parser)) ? statusOk  : false;
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "w", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("img:rect:w", w, valueSet, parser)) ? statusOk  : false;
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "h", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("img:rect:h", h, valueSet, parser)) ? statusOk  : false;

    if(statusOk){
        cairo_rectangle(cr,x,y,w,h);
        cairo_push_group(cr);
        if(parser.nodeHasLink(Parser::NodeLink::CHILDREN)){
            parser.getNodeLink(Parser::NodeLink::CHILDREN);
            imageMain(cr);
            parser.getNodeLink(Parser::NodeLink::PARENT);
        } 
        cairo_pop_group_to_source(cr);
        cairo_rectangle(cr,x,y,w,h);
        cairo_fill(cr);
    }
}

void CairoInterpreter001::handleImageArc(cairo_t * cr){
    bool statusOk = true;
    double radius,begin,end;
    std::vector<double> center, valueSet; 
    auto attributeSet = parser.getXmlNodeAttributes();

    center = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "center", attributeSet));
    statusOk = (verifyArcCenter(center, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "radius", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("img:arc:radius", radius, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "begin", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("img:arc:begin", begin, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "end", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("img:arc:end", end, valueSet, parser)) ? statusOk : false;

    if(statusOk){
        cairo_arc(cr, center[0], center[1], radius, begin, end);
        cairo_push_group(cr);
        if(parser.nodeHasLink(Parser::NodeLink::CHILDREN)){
            parser.getNodeLink(Parser::NodeLink::CHILDREN);
            imageMain(cr);
            parser.getNodeLink(Parser::NodeLink::PARENT);
        } 
        cairo_pop_group_to_source(cr);
        cairo_arc(cr, center[0], center[1], radius, begin, end);
        cairo_fill(cr);
    }
}

void CairoInterpreter001::handleImageFill(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();
    std::string gradientId = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "gradient", attributeSet, true);
    
    if(gradientId.empty()){
        std::vector<double> color = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "color", attributeSet));
        if(verifyColor(color,parser)){
            cairo_set_source_rgba(cr, color[0], color[1], color[2], color.size() == 3 ? 1.0 : color[3]);
        }
    } else {
        cairo_pattern_t * gradient;
        if((gradient = getGradientById(gradientId))){
            cairo_set_source(cr, gradient);
        }
    }

    std::string mode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "mode", attributeSet, true);

    if (!strcmp(mode.c_str(), "normal") || mode.empty()){
        cairo_fill(cr);
    } else if (!strcmp(mode.c_str(), "preserve")){
        cairo_fill_preserve(cr);
    } else {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, ("\n--> unknown fill:mode \"" + mode + "\" expected \"normal\" or \"preserve\". graphml line: " + std::to_string(parser.getXmlNodeLine())).c_str());
    }
}

void CairoInterpreter001::handleImageStroke(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();
    std::string gradientId = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "gradient", attributeSet, true);
    double width = 1;
    std::vector<double> valueSet;

    if(gradientId.empty()){
        std::vector<double> color = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "color", attributeSet));
        if(verifyColor(color,parser)){
            cairo_set_source_rgba(cr, color[0], color[1], color[2], color.size() == 3 ? 1.0 : color[3]);
        }
    } else {
        cairo_pattern_t * gradient;
        if((gradient = getGradientById(gradientId))){
            cairo_set_source(cr, gradient);
        }
    }

    std::string mode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "mode", attributeSet, true);
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "width", attributeSet));
    getVerifiedSingleRelativeValue("stroke::width",width, valueSet, parser);
    cairo_set_line_width(cr, width);
    if (!strcmp(mode.c_str(), "normal") || mode.empty()){
        cairo_stroke(cr);
    } else if (!strcmp(mode.c_str(), "preserve")){
        cairo_stroke_preserve(cr);
    } else {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, ("\n--> unknown stroke:mode \"" + mode + "\" expected \"normal\" or \"preserve\". graphml line: " + std::to_string(parser.getXmlNodeLine())).c_str());
    }
}

void CairoInterpreter001::handleImageExport(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();

    std::string filename = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "filename", attributeSet);

    if(!filename.empty()){
        cairo_surface_write_to_png(cairo_get_target(cr), filename.c_str());
    }
}

void CairoInterpreter001::main() {
    while(parser.nodeHasLink(Parser::NodeLink::NEXT)){
        // std::cout << parser.getXmlNodeName() << std::endl;
        if(std::regex_match(parser.getXmlNodeName(), gradientRegex)){
            handleGradient();
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "img")){
            handleImage();
        } 
        parser.getNodeLink(Parser::NodeLink::NEXT);
    }
}

CairoInterpreter001::GradientType getGradientType(std::string typeStr){
        if(!strcasecmp(typeStr.c_str(), "linear")){
            return CairoInterpreter001::GradientType::Linear;
        } else if (!strcasecmp(typeStr.c_str(), "radial")){
            return CairoInterpreter001::GradientType::Radial;
        } else {
            return CairoInterpreter001::GradientType::Unknown;
        }
}

void CairoInterpreter001::handleGradient() {
    std::string id, typeStr;
    GradientType type;
    cairo_pattern_t * gradient;
    bool statusOk = true;
    auto attributeSet = parser.getXmlNodeAttributes();
    std::vector<double> begin, end, colorStopColors, colorStopStep;

    // get gradient id
    id = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "id", attributeSet);

    if(!id.empty()){
        
        typeStr = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "type", attributeSet);
        begin = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "begin", attributeSet));
        end = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "end", attributeSet));

        type = getGradientType(typeStr);
        statusOk = (type != Unknown) ? statusOk : false;

        statusOk = (statusOk) ? verifyBeginArgNum(begin.size(), type, parser) : statusOk;
        statusOk = (statusOk) ? verifyEndArgNum(end.size(), type, parser) : statusOk;

        if(statusOk) {
                switch (type) {
                    case Linear: gradient = cairo_pattern_create_linear(begin[0], begin[1], end[0], end[1]);break;
                    case Radial: gradient = cairo_pattern_create_radial(begin[0], begin[1],begin[2],end[0], end[1],end[2]);break;
                    default:break;
                }

                gmlCheck(gradient, gmlError, __FUNCTION__, __FILE__, __LINE__, "\n--> unable to allocate gradient");

                addGradient(id,gradient);

                parser.getNodeLink(Parser::NodeLink::CHILDREN);
                while(parser.nodeHasLink(Parser::NodeLink::NEXT)){
                    if(!strcasecmp(parser.getXmlNodeName().c_str(), "colorStop")){
                        attributeSet = parser.getXmlNodeAttributes();
                        colorStopStep = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "step", attributeSet));
                        colorStopColors = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "color", attributeSet));
                        if(verifyColorStopStep(colorStopStep, parser) && verifyColor(colorStopColors, parser)) {
                            cairo_pattern_add_color_stop_rgba(gradient, colorStopStep[0], colorStopColors[0],colorStopColors[1],colorStopColors[2], colorStopColors.size() == 4 ? colorStopColors[4] : 1.0);
                        }
                    }
                    parser.getNodeLink(Parser::NodeLink::NEXT);
                }
                parser.getNodeLink(Parser::NodeLink::PARENT); 
            }
    } else {
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (std::string("\n--> unidentified gradients are ignored... line: ")+std::to_string( parser.getXmlNodeLine())).c_str());
    }
}

bool getVerifiedImageSide(const char * sideName, double & side,  std::vector<double> attributeSet, Parser & parser){
    bool statusOk = true;

    if (attributeSet.size() != 1) {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect number of img:"+std::string(sideName)+" arguments, expected 1. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    } else if (attributeSet[0] < 0) {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect value for img:"+std::string(sideName)+" argument, the value must to be greater than 0. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    } 
    side = attributeSet[0];
    return statusOk;
}

void CairoInterpreter001::handleImage(){
    auto attributeSet = parser.getXmlNodeAttributes();
    std::vector<double> attributeValues;
    double w, h;
    bool statusOk = true;
    std::string id;
    cairo_surface_t * image;
    cairo_t * cr;

    id = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "id", attributeSet);

    attributeValues = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "w", attributeSet));
    statusOk = (statusOk) ? getVerifiedImageSide("w", w, attributeValues, parser) : false;
    attributeValues = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "h", attributeSet));
    statusOk = (statusOk) ? getVerifiedImageSide("h", h, attributeValues, parser) : false;

    if(statusOk){
        image = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,w,h);
        gmlCheck(image, gmlError, __FUNCTION__, __FILE__, __LINE__, "\n--> unable to allocate image_surface");
        cr = cairo_create(image);
        gmlCheck(image, gmlError, __FUNCTION__, __FILE__, __LINE__, "\n--> unable to allocate cairo");
        cairo_scale(cr, w, w);

        if(parser.nodeHasLink(Parser::NodeLink::CHILDREN)){
            parser.getNodeLink(Parser::NodeLink::CHILDREN);
            imageMain(cr);
            parser.getNodeLink(Parser::NodeLink::PARENT);
        } 

        if(!id.empty()) {
            addImage(id, std::__1::pair<cairo_t *, cairo_surface_t *> {cr, image});
        }
    }
}