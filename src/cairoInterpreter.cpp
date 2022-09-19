#include "cairoInterpreter.hpp"
#include "error.hpp"
#include "parser.hpp"

const std::regex CairoInterpreter::gradientRegex = std::regex("(gradient)|(grad)", std::regex::icase);

CairoInterpreter::CairoInterpreter(Parser & parser): parser(parser){
    gmlCheck(parser.engine == "cairo", gmlError, __FUNCTION__, __FILE__, __LINE__, "parser.engine is not cairo");
}