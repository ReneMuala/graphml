#include "error.hpp"
#include <stdio.h>
#include <assert.h>
#include <string>
#include <exception>
#include <iostream>
#include <string>

static std::ostream * warnStream = &std::cerr;

void gmlError(const char * function, const char * file, const int line, const char * msg){
    throw std::runtime_error("Graphml Error in "+std::string(function)+"(...), file: \""+std::string(file)+"\" line: "+std::to_string(line)+" desc: "+std::string(msg));
}

void gmlWarn(const char * function, const char * file, const int line, const char * msg){
    (*warnStream) << ("Graphml Warning in "+std::string(function)+"(...), file: \""+std::string(file)+"\" line: "+std::to_string(line)+" desc: "+std::string(msg)) << "\n";
}

void setGmlWarnStream(std::ostream & stream){
    warnStream = &stream;
}

void gmlCheck(bool condition,
                void (*callback)(const char * function, const char * file, const int line, const char * msg),
                const char * function,
                const char * file,
                const int line,
                const char * msg){
    if(!condition){
        callback(function, file, line, msg);
    }
}

void gmlCheckOr(bool condition,
                void (*callback)(const char * function, const char * file, const int line, const char * msg),
                const char * function,
                const char * file,
                const int line,
                const char * msg,
                void (*orCallback)(void*),
                void*orCallbackArg)
    {
    if(!condition){
        callback(function, file, line, msg);
    } else {
        orCallback(orCallbackArg);
    }
}