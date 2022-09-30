#include "cairoInterpreter0.0.1.hpp"
#include <iostream>
#include <regex>
#include <vector>
#include "operators.hpp"
#include "gisl.hpp"
#include "error.hpp"
#include <optional>

static const std::regex gradientRegex = std::regex("(gradient)|(grad)", std::regex::icase);

bool getVerifiedSingleRelativeValue(std::string valueName, double & value,  std::vector<double> valueSet, Parser & parser, bool checkValue = true){
    bool statusOk = true;
    if (valueSet.size() != 1) {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect number of "+valueName+" arguments, expected 1. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    } else  {
        if (checkValue && (valueSet[0] < 0 || valueSet[0] > 1)) {
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> value for "+valueName+" argument, is not in [0,1]. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        }  value = valueSet[0];
    }
    return statusOk;
}

bool verifyPathMode(std::string mode, Parser& parser){
    if(strcasecmp(mode.c_str(), "normal") && strcasecmp(mode.c_str(), "relative")){
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> incorrect value for path:mode argument, expected normal or relative. line: ") + std::to_string(parser.getXmlNodeLine())).c_str()); return false;
    } return true;
}

bool getVerifiedSingleRelativePoint(std::string pointName, std::vector<double> & point, std::vector<double> valueSet, Parser & parser){
    if(valueSet.size() != 2){
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (
            std::string("\n--> incorrect number of ")+pointName+std::string(" arguments, expected 2 \"x,y\"") + std::string(" at line: ") + std::to_string( parser.getXmlNodeLine())
            ).c_str());
        return false;
    } for(auto & value : valueSet){
        if(value < 0 || value > 1){
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> value for arc:center argument, is not in [0,1]. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        }
    }
    point = valueSet;
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

CairoInterpreter001::CairoInterpreter001(Parser & parser): CairoInterpreter(parser),embeddedApplyStackSize(0),current_device_width(0), current_device_height(0), first_path_element(true),apply_main_show_logs(false){}

CairoInterpreter001::~CairoInterpreter001(){
    for(auto && [id, imagePair] : images){
        if(imagePair.first)cairo_destroy(imagePair.first);
        if(imagePair.second)cairo_surface_destroy(imagePair.second);
    }
    for(auto && [id, gradient] : gradients){
        if(gradient)cairo_pattern_destroy(gradient);
    }
}

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
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, ("\n--> unable to get gradient with id \"" + id + "\".").c_str());
    } return NULL;
}

std::pair<cairo_t *, cairo_surface_t *>* CairoInterpreter001::getImageByid(std::string id){
    if(images.count(id)){
        return &images[id];
    } else {
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, ("\n-->unable to get a image with id \"" + id + "\".").c_str());
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

void CairoInterpreter001::addPath(std::string id, xmlNode* node){
    if(paths.count(id)){
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, ("\n--> repeated path id \"" + id + "\", att: the old instance will be deleted.").c_str());
    } paths[id] = node;
}

void CairoInterpreter001::applyMove(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();
    double x, y;
    std::string mode;
    bool statusOk = true;

    std::vector<double> valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "x", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("move:x",x, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "y", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("move:y",y, valueSet, parser)) ? statusOk : false;

    mode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "mode", attributeSet, true);
    mode = (mode.empty()) ? "normal" : mode;
    if(statusOk && verifyPathMode(mode, parser)){
        if(!strcasecmp(mode.c_str(), "normal")){
            cairo_move_to(cr, x, y);
            first_path_element = false;
        } else {
            cairo_rel_move_to(cr, x, y);
        }
    }
}

bool CairoInterpreter001::getTextData(double & x, double & y, double & size, cairo_font_slant_t & slant, cairo_font_weight_t & weight, std::string & value, std::string & font){
    bool statusOk = true;
    std::vector<double> valueSet; 
    auto attributeSet = parser.getXmlNodeAttributes();
    std::string slantStr, weightStr;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "x", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("text:x", x, valueSet, parser)) ? statusOk : false;
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "y", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("text:y", y, valueSet, parser)) ? statusOk  : false;
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "size", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("text:size", size, valueSet, parser)) ? statusOk  : false;
    
    slantStr = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "slant", attributeSet, true);
    slantStr = (slantStr.empty()) ? "normal" : slantStr;

    weightStr = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "weight", attributeSet, true);
    weightStr = (weightStr.empty()) ? "normal" : weightStr;

    if(!strcasecmp(slantStr.c_str(), "normal")){
        slant = CAIRO_FONT_SLANT_NORMAL;
    } else if(!strcasecmp(slantStr.c_str(), "oblique")){
        slant = CAIRO_FONT_SLANT_OBLIQUE;
    } else if(!strcasecmp(slantStr.c_str(), "italic")){
        slant = CAIRO_FONT_SLANT_ITALIC;
    } else {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> unknown value for text:slant argument, expected normal, oblique or italic. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    }

    if(!strcasecmp(weightStr.c_str(), "normal")){
        weight = CAIRO_FONT_WEIGHT_NORMAL;
    } else if(!strcasecmp(weightStr.c_str(), "bold")){
        weight = CAIRO_FONT_WEIGHT_BOLD;
    } else {
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> unknown value for text:weight argument, expected normal or bold. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    }

    value = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "value", attributeSet);
    if(value.empty()){
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> value for text:value argument, not provided. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    }

    font = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "font", attributeSet);
    if(font.empty()){
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> value for text:font argument, not provided. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
        statusOk = false;
    }

    return statusOk;
}

bool CairoInterpreter001::getRectCoords(double & x, double & y, double &  w, double  & h){
    bool statusOk = true;
    std::vector<double> valueSet; 
    auto attributeSet = parser.getXmlNodeAttributes();

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "x", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("rect:x", x, valueSet, parser)) ? statusOk : false;
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "y", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("rect:y", y, valueSet, parser)) ? statusOk  : false;
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "w", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("rect:w", w, valueSet, parser)) ? statusOk  : false;
    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "h", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("rect:h", h, valueSet, parser)) ? statusOk  : false;
    return statusOk;
}


bool CairoInterpreter001::getArcCoords(std::vector<double>  & center, double & radius, double  & begin, double & end){
    bool statusOk = true;
    auto attributeSet = parser.getXmlNodeAttributes();

    auto valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "center", attributeSet));
    statusOk = (getVerifiedSingleRelativePoint("img:arc:center", center, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "radius", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("arc:radius", radius, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "begin", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("arc:begin", begin, valueSet, parser, false)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "end", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("arc:end", end, valueSet, parser, false)) ? statusOk : false;
    return statusOk;
}

void CairoInterpreter001::applyClose(cairo_t * cr){
    cairo_close_path(cr);
}

void CairoInterpreter001::applyCurve(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();
    std::vector<double> begin, middle, end;
    std::string mode;
    bool statusOk = true;

    std::vector<double> valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "begin", attributeSet));
    statusOk = (getVerifiedSingleRelativePoint("curve:begin",begin, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "middle", attributeSet));
    statusOk = (getVerifiedSingleRelativePoint("curve:middle",middle, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "end", attributeSet));
    statusOk = (getVerifiedSingleRelativePoint("curve:end",end, valueSet, parser)) ? statusOk : false;

    mode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "mode", attributeSet, true);
    mode = (mode.empty()) ? "normal" : mode;

    if(statusOk && verifyPathMode(mode, parser)){
        if(!strcasecmp(mode.c_str(), "normal")){
            cairo_curve_to(cr, begin[0], begin[1], middle[0], middle[1], end[0], end[1]);
            first_path_element = false;
        } else {
            if(first_path_element)
                gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> using a relative path element \"") + parser.getXmlNodeName() + std::string("\" without a previous not relative. unsafe operation line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
            else
                cairo_rel_curve_to(cr, begin[0], begin[1], middle[0], middle[1], end[0], end[1]);
        }
    }
}

void CairoInterpreter001::applyLine(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();
    double x, y;
    std::string mode;
    bool statusOk = true;

    std::vector<double> valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "x", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("line:x",x, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "y", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("line:y",y, valueSet, parser)) ? statusOk : false;

    mode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "mode", attributeSet, true);
    mode = (mode.empty()) ? "normal" : mode;

    if(statusOk && verifyPathMode(mode, parser)){
        if(!strcasecmp(mode.c_str(), "normal")){
            cairo_line_to(cr, x, y);
            first_path_element = false;
        } else {
            if(first_path_element)
                gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> using a relative path element \"") + parser.getXmlNodeName() + std::string("\" without a previous not relative. unsafe operation line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
            else
                cairo_rel_line_to(cr, x, y);       
        }
    }
}

void CairoInterpreter001::applyRect(cairo_t * cr){
    double x,y,w,h;
    if(getRectCoords(x,y,w,h)) {
        cairo_rectangle(cr,x,y,w,h);
        first_path_element = false;
    }
}

void CairoInterpreter001::applyArc(cairo_t * cr){
    double radius,begin,end;
    std::vector<double> center;
    if(getArcCoords(center,radius, begin,end)) {
        cairo_arc(cr, center[0], center[1], radius, begin, end);
        first_path_element = false;
    }
}

void CairoInterpreter001::applyMain(cairo_t * cr, xmlNode* cur){
    std::vector<double> valueSet, args;
    xmlNode* curAux = parser.cur;
    parser.cur = cur;
    first_path_element = true;
    bool statusDone = true;
    if(parser.nodeHasLink(Parser::CHILDREN)){
        parser.getNodeLink(Parser::CHILDREN);
        while(parser.nodeHasLink(Parser::NEXT)){
            parser.getNodeLink(Parser::NEXT);
            if(!strcasecmp(parser.getXmlNodeName().c_str(), "move")){
                applyMove(cr);
            } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "line")){
                applyLine(cr);
            } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "curve")){
                applyCurve(cr);
            } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "rect")){
                applyRect(cr);
            } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "arc")){
                applyArc(cr);
            } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "close")){
                applyClose(cr);
            }
        } parser.getNodeLink(Parser::PARENT);
    }
    parser.cur = curAux;
}

bool CairoInterpreter001::strokeSetLineCap(cairo_t * cr, cairo_line_cap_t & cap, std::unordered_map<std::string, std::string> &attributeSet){
    bool statusOk = true;
    std::string capMode;
    static const std::regex supportedCapModes = std::regex("(|BUTT|ROUND|SQUARE)", std::regex::icase);

    capMode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "cap", attributeSet, true);

    statusOk = (std::regex_match(capMode, supportedCapModes)) ? statusOk : false;

    if(statusOk){
        if(!capMode.empty()) {
            if(!strcasecmp(capMode.c_str(), "butt")){
                cairo_set_line_cap(cr, (cap=CAIRO_LINE_CAP_BUTT));
            } else if(!strcasecmp(capMode.c_str(), "round")){
                cairo_set_line_cap(cr, (cap=CAIRO_LINE_CAP_ROUND));
            } else if(!strcasecmp(capMode.c_str(), "square")){
                cairo_set_line_cap(cr, (cap=CAIRO_LINE_CAP_SQUARE));
            }
        }
    } else {
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (std::string("\n--> unknown stroke cap \"" )+ capMode + std::string("\", expected BUTT, ROUND or SQUARE. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
    } return statusOk;
}

bool CairoInterpreter001::strokeSetLineJoin(cairo_t * cr,  cairo_line_join_t & join, std::unordered_map<std::string, std::string> &attributeSet){
    bool statusOk = true;
    std::string joinMode;
    static const std::regex supportedJoinModes = std::regex("(|BEVEL|ROUND|MITER)", std::regex::icase);

    joinMode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "join", attributeSet, true);

    statusOk = (std::regex_match(joinMode, supportedJoinModes)) ? statusOk : false;

    if(statusOk ){
        if(!joinMode.empty()){
            if(!strcasecmp(joinMode.c_str(), "bevel")){
                cairo_set_line_join(cr, (join = CAIRO_LINE_JOIN_BEVEL));
            } else if(!strcasecmp(joinMode.c_str(), "round")){
                cairo_set_line_join(cr, (join = CAIRO_LINE_JOIN_ROUND));
            } else if(!strcasecmp(joinMode.c_str(), "miter")){
                cairo_set_line_join(cr, (join = CAIRO_LINE_JOIN_MITER));
            }
        }
    } else {
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (std::string("\n--> unknown stroke join \"") + joinMode + std::string("\", expected BEVEL, ROUND or MITER. line: ") + std::to_string(parser.getXmlNodeLine())).c_str());
    } return statusOk;
}

void CairoInterpreter001::clearDrawEventStack(){
    while(drawEventStack.size()){
        drawEventStack.pop();
    }
}

void CairoInterpreter001::imageMain(cairo_t * cr){
     while(parser.nodeHasLink(Parser::NodeLink::NEXT)){
        if(!embeddedApplyStackSize) clearDrawEventStack();
        if(!strcasecmp(parser.getXmlNodeName().c_str(), "export")){
            handleImageExport(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "import")){
            handleImport();
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "apply")){
            handleImageApply(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "rotate")){
            handleImageRotate(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "scale")){
            handleImageScale(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "translate")){
            handleImageTranslate(cr);
        }  else if(!strcasecmp(parser.getXmlNodeName().c_str(), "rect")){
            handleImageRect(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "arc")){
            handleImageArc(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "fill")){
            handleImageFill(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "paint")){
            handleImagePaint(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "textstr")){
            handleImageText(cr);
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "stroke")){
            handleImageStroke(cr);
        } parser.getNodeLink(Parser::NodeLink::NEXT);
    }
}

void CairoInterpreter001::handleImageText(cairo_t * cr){
    double x,y, size;
    std::string value, font;
    cairo_font_slant_t slant;
    cairo_font_weight_t weight;
    
    if(getTextData(x, y, size,slant, weight, value, font)){
        cairo_set_font_size(cr, size);
        cairo_select_font_face(cr, font.c_str(), slant, weight);
        cairo_move_to(cr,x,y);
        cairo_text_path(cr,value.c_str());
        cairo_push_group(cr);
        if(parser.nodeHasLink(Parser::NodeLink::CHILDREN)){
            parser.getNodeLink(Parser::NodeLink::CHILDREN);
            imageMain(cr);
            parser.getNodeLink(Parser::NodeLink::PARENT);
        } 
        cairo_pop_group_to_source(cr);
        cairo_set_font_size(cr, size);
        cairo_select_font_face(cr,font.c_str(), slant, weight);
        cairo_move_to(cr,x,y);
        cairo_text_path(cr,value.c_str());
        cairo_fill(cr);
    }
}

void CairoInterpreter001::handleImageTranslate(cairo_t * cr){
    double x,y;
    auto attributeSet = parser.getXmlNodeAttributes();
    bool statusOk = true;

    std::vector<double> valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "x", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("translate:x",x, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "y", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("translate:y",y, valueSet, parser)) ? statusOk : false;

    if(statusOk) {
        cairo_translate(cr, x,y);
        cairo_push_group(cr);
        if(parser.nodeHasLink(Parser::NodeLink::CHILDREN)){
            parser.getNodeLink(Parser::NodeLink::CHILDREN);
            imageMain(cr);
            parser.getNodeLink(Parser::NodeLink::PARENT);
        } 
        cairo_pop_group_to_source(cr);
        cairo_translate(cr, -x,-y);
        cairo_paint(cr);
    }
}

void CairoInterpreter001::handleImageRotate(cairo_t * cr){
    double rad;
    auto attributeSet = parser.getXmlNodeAttributes();
    bool statusOk = true;

    std::vector<double> valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "rad", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("rotate:rad",rad, valueSet, parser, false)) ? statusOk : false;

    if(statusOk) {
        cairo_rotate(cr, rad);
        cairo_push_group(cr);
        if(parser.nodeHasLink(Parser::NodeLink::CHILDREN)){
            parser.getNodeLink(Parser::NodeLink::CHILDREN);
            imageMain(cr);
            parser.getNodeLink(Parser::NodeLink::PARENT);
        } 
        cairo_pop_group_to_source(cr);
        cairo_rotate(cr, -rad);
        cairo_paint(cr);
    } 
}

void CairoInterpreter001::handleImageScale(cairo_t * cr){
    double x,y;
    auto attributeSet = parser.getXmlNodeAttributes();
    bool statusOk = true;

    std::vector<double> valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "x", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("scale:x",x, valueSet, parser)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "y", attributeSet));
    statusOk = (getVerifiedSingleRelativeValue("scale:y",y, valueSet, parser)) ? statusOk : false;

    if(statusOk && (!x || !y)){
        statusOk = false;
        gmlWarn(__FUNCTION__, __FILE__, __LINE__, ("\n--> scale receiving  zero for x or y. unsafe operation. graphml line: " + std::to_string(parser.getXmlNodeLine())).c_str());
    }

    if(statusOk) {
        cairo_scale(cr, x,y);
        cairo_push_group(cr);
        if(parser.nodeHasLink(Parser::NodeLink::CHILDREN)){
            parser.getNodeLink(Parser::NodeLink::CHILDREN);
            imageMain(cr);
            parser.getNodeLink(Parser::NodeLink::PARENT);
        } 
        cairo_pop_group_to_source(cr);
        cairo_scale(cr, pow(x,-1),pow(y,-1));
        cairo_paint(cr);
    }
}

void CairoInterpreter001::handleImageRect(cairo_t * cr){
    double x,y,w,h;
    if(getRectCoords(x,y,w,h)){
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
    double radius,begin,end;
    std::vector<double> center;

    if(getArcCoords(center,radius, begin,end)){
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

bool CairoInterpreter001::handleImageDrawArguments(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();
    std::string gradientId = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "gradient", attributeSet, true);
    std::string imageId = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "image", attributeSet, true);
    bool statusOk = false;    
    if(!imageId.empty()){
        std::pair<cairo_t *, cairo_surface_t *>* imageData;
        if((imageData = getImageByid(imageId))){
            statusOk = true;
            cairo_surface_set_device_scale(imageData->second, current_device_width, current_device_height);
            cairo_set_source_surface(cr, imageData->second,0, 0);
        }
    } else if(!gradientId.empty()){
        cairo_pattern_t * gradient;
        if((gradient = getGradientById(gradientId))){
            statusOk = true;
            cairo_set_source(cr, gradient);
        }
    } else {
        std::vector<double> color = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "color", attributeSet));
        if(verifyColor(color,parser)){
            statusOk = true;
            cairo_set_source_rgba(cr, color[0], color[1], color[2], color.size() == 3 ? 1.0 : color[3]);
        }
    } return statusOk;
}

void CairoInterpreter001::handleImageFill(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();

    std::string mode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "mode", attributeSet, true);

    if(handleImageDrawArguments(cr)){
        if (!strcmp(mode.c_str(), "normal") || mode.empty()){
            drawEventStack.push(std::pair<DrawEvent, std::optional<StrokeProperties>>{Fill, {}});
            cairo_fill(cr);
        } else if (!strcmp(mode.c_str(), "preserve")){
            drawEventStack.push(std::pair<DrawEvent, std::optional<StrokeProperties>>{FillPreserve, {}});        
            cairo_fill_preserve(cr);
        } else {
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, ("\n--> unknown fill:mode \"" + mode + "\" expected \"normal\" or \"preserve\". graphml line: " + std::to_string(parser.getXmlNodeLine())).c_str());
        }
    }
}

void CairoInterpreter001::handleImagePaint(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();

    auto valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "alpha", attributeSet, true));
    double alpha = 1;
    bool statusOk = true;
    if(valueSet.size()){
        statusOk = getVerifiedSingleRelativeValue("paint::alpha",alpha, valueSet, parser) ? statusOk : false;
    }

    if(statusOk && handleImageDrawArguments(cr)) cairo_paint_with_alpha(cr, alpha);
}

void CairoInterpreter001::handleImageStroke(cairo_t * cr){
    std::vector<double> valueSet;
    std::string mode, capMode, joinMode;
    cairo_line_cap_t cap;
    cairo_line_join_t join;
    cairo_pattern_t * gradient;
    bool statusOk = true;
    double width = 1;
    auto attributeSet = parser.getXmlNodeAttributes();

    statusOk = (handleImageDrawArguments(cr)) ? statusOk : false;

    valueSet = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "width", attributeSet));
    getVerifiedSingleRelativeValue("stroke::width",width, valueSet, parser);
    cairo_set_line_width(cr, width);

    mode = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "mode", attributeSet, true);

    statusOk = (strokeSetLineCap(cr, cap, attributeSet) && strokeSetLineJoin(cr, join, attributeSet)) ? statusOk : false;

    if(statusOk){
        if (!strcmp(mode.c_str(), "normal") || mode.empty()){
            drawEventStack.push(std::pair<DrawEvent, StrokeProperties>{Stroke, StrokeProperties(width, cap, join)});
            cairo_stroke(cr);
        } else if (!strcmp(mode.c_str(), "preserve")){
            drawEventStack.push(std::pair<DrawEvent, StrokeProperties>{StrokePreserve, StrokeProperties(width, cap, join)});
            cairo_stroke_preserve(cr);
        } else {
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, ("\n--> unknown stroke:mode \"" + mode + "\" expected \"normal\" or \"preserve\". graphml line: " + std::to_string(parser.getXmlNodeLine())).c_str());
        }
    }
}

void CairoInterpreter001::handleImageExport(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();

    std::string filename = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "filename", attributeSet);

    if(!filename.empty()){
        cairo_surface_write_to_png(cairo_get_target(cr), filename.c_str());
    }
}

void CairoInterpreter001::handleImport(){
    cairo_surface_t * surfc;
    cairo_status_t surfc_status;
    std::string filename;
    bool statusOk = false;
    std::string errorCause;
    auto attributeSet = parser.getXmlNodeAttributes();
    std::string id = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "id", attributeSet);
    if(!id.empty()){
        filename = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "filename", attributeSet);
        if(!filename.empty() && !id.empty()){
        surfc = cairo_image_surface_create_from_png(filename.c_str());
        surfc_status = cairo_surface_status(surfc);
        switch (surfc_status){
        case CAIRO_STATUS_NO_MEMORY: errorCause = "CAIRO_STATUS_NO_MEMORY"; break;
        case CAIRO_STATUS_FILE_NOT_FOUND: errorCause = "CAIRO_STATUS_FILE_NOT_FOUND"; break;
        case CAIRO_STATUS_READ_ERROR: errorCause = "CAIRO_STATUS_READ_ERROR"; break;
        case CAIRO_STATUS_PNG_ERROR: errorCause = "CAIRO_STATUS_PNG_ERROR"; break;
        default: statusOk = true;break;
        }
        if(!statusOk){
            gmlWarn(__FUNCTION__, __FILE__, __LINE__, (std::string("\n--> unable to import file :")+filename+" graphml line: " + std::to_string(parser.getXmlNodeLine()) + ", cause: " + errorCause).c_str());
        } else {
            addImage(id,{NULL, surfc});
        }
    }
    } else {
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (std::string("\n--> unidentified image imports are ignored... line: ")+std::to_string( parser.getXmlNodeLine())).c_str());
    }
}

void CairoInterpreter001::handleImageApply(cairo_t * cr){
    auto attributeSet = parser.getXmlNodeAttributes();
    std::string path = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "path", attributeSet);
    xmlNode* pathCur;
    DrawEvent firstDrawEvent;
    if(!path.empty() && paths.count(path) && (pathCur = paths[path])){
        apply_main_show_logs = true;
        applyMain(cr, pathCur);
        apply_main_show_logs = false;
        cairo_push_group(cr);
        if(parser.nodeHasLink(Parser::NodeLink::CHILDREN)){
            parser.getNodeLink(Parser::NodeLink::CHILDREN);
            embeddedApplyStackSize++;
            imageMain(cr);
            embeddedApplyStackSize--;
            parser.getNodeLink(Parser::NodeLink::PARENT);
        }
        cairo_pop_group_to_source(cr);
        applyMain(cr, pathCur);
        while(drawEventStack.size() > 1) drawEventStack.pop();
        if(drawEventStack.size() == 1){
            firstDrawEvent = drawEventStack.top().first;
            switch (firstDrawEvent)
            {
            case FillPreserve:
            case Fill:
                cairo_fill(cr);
                break;
            case StrokePreserve:
            case Stroke:
                if(drawEventStack.top().second.has_value()){
                    cairo_set_line_join(cr, drawEventStack.top().second->join);
                    cairo_set_line_cap(cr, drawEventStack.top().second->cap);
                    cairo_set_line_width(cr, drawEventStack.top().second->lineWidth);
                }
                cairo_stroke(cr);
                break;
            }
        }
    } else {
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (std::string("\n--> failed to apply path \"")+path+std::string("\"... line: ")+std::to_string( parser.getXmlNodeLine())).c_str());
    }
}


void CairoInterpreter001::main() {
    while(parser.nodeHasLink(Parser::NodeLink::NEXT)){
        if(std::regex_match(parser.getXmlNodeName(), gradientRegex)){
            handleGradient();
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "img")){
            handleImage();
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "path")){
            handlePath();
        } else if(!strcasecmp(parser.getXmlNodeName().c_str(), "import")){
            handleImport();
        } parser.getNodeLink(Parser::NodeLink::NEXT);
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

void CairoInterpreter001::handlePath(){
    std::string id;
    auto attributeSet = parser.getXmlNodeAttributes();

    id = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "id", attributeSet);

    if(!id.empty()){
        addPath(id, parser.cur);
    } else {
        gmlWarn(__FUNCTION__,__FILE__, __LINE__, (std::string("\n--> unidentified paths are ignored... line: ")+std::to_string( parser.getXmlNodeLine())).c_str());
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

    id = parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "id", attributeSet, true);

    attributeValues = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "w", attributeSet));
    statusOk = (statusOk) ? getVerifiedImageSide("w", w, attributeValues, parser) : false;
    attributeValues = gisl::evaluateExpression(parser.getRequiredAttribute(parser.getXmlNodeName(), parser.getXmlNodeLine(), "h", attributeSet));
    statusOk = (statusOk) ? getVerifiedImageSide("h", h, attributeValues, parser) : false;

    if(statusOk){
        current_device_width = w;
        current_device_height = h;
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
            addImage(id, std::pair<cairo_t *, cairo_surface_t *> {cr, image});
        }
    }
}