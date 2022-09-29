#include "parser.hpp"
#include "error.hpp"
#include "operators.hpp"
#include <string>

Parser::Parser(const std::string filename){
    document = xmlParseFile(filename.c_str());
    gmlCheck(document, gmlError, __FUNCTION__, __FILE__, __LINE__, "libxml failed to parse the document");
    cur = xmlDocGetRootElement(document);
    gmlCheck(document, gmlError, __FUNCTION__, __FILE__, __LINE__, "libxml failed to get the document root element");
    gmlCheckOr(!xmlStrcasecmp(cur->name, (const xmlChar *)"graphml"), gmlWarn, __FUNCTION__, __FILE__, __LINE__, "the input file doesn't have a <graphml> root tag", 
    [](void*obj){
        gmlCheck(obj, gmlError, __FUNCTION__, __FILE__, __LINE__, "Parser was not initialized before use");
        ((Parser*)obj)->readGraphmlInfo();
        ((Parser*)obj)->getNodeLink(NodeLink::CHILDREN);
    }, this);
}

Parser::~Parser(){
    if(document){
        xmlFreeDoc(document);
    }
}

std::unordered_map<std::string, std::string> Parser::getXmlNodeAttributes(xmlDoc * document, xmlNode * node){
    std::unordered_map<std::string, std::string> umap;

    xmlAttr * attribute = node->properties;

    while(attribute){
        umap[std::string((const char *)attribute->name)] = std::string((const char *)xmlNodeListGetString(document,attribute->children, 1));
        attribute=attribute->next;
    }
    return umap;
}

std::unordered_map<std::string, std::string> Parser::getXmlNodeAttributes(){
    return getXmlNodeAttributes(document, cur);
}


std::string Parser::getRequiredAttribute(std::string elementName, long elementLine, std::string attributeName, std::unordered_map<std::string, std::string> attributeSet, bool optional){
    for (auto [key, value] : attributeSet){
        if(key == attributeName){
            return value;
        }
    } 
    gmlCheck(optional, gmlWarn,__FUNCTION__,__FILE__, __LINE__, ("Unable to get required attribute " + std::string(attributeName) + " for element: " + std::string(elementName) + " graphml line:" + std::to_string(elementLine)).c_str());
    return "";
}


std::string Parser::getXmlNodeName(xmlNode * node){
    gmlCheck(node, gmlError, __FUNCTION__, __FILE__, __LINE__, "node is NULL");
    return std::string((const char *)node->name);
}

std::string Parser::getXmlNodeName(){
    return getXmlNodeName(cur);
}

long Parser::getXmlNodeLine(xmlNode * node){
    gmlCheck(node, gmlError, __FUNCTION__, __FILE__, __LINE__, "node is NULL");
    return xmlGetLineNo(node);
}

long Parser::getXmlNodeLine(){
    return getXmlNodeLine(cur);
}

bool Parser::nodeHasLink(xmlNode * node, Parser::NodeLink nodeLink){
    gmlCheck(node, gmlError, __FUNCTION__, __FILE__, __LINE__, "node is NULL");
    switch (nodeLink)
    {
    case CHILDREN:
        return node->children;
    case PARENT:
        return node->parent;
    case NEXT:
        return node->next;
    case PREV:
        return node->prev;
    }
}

bool Parser::nodeHasLink(Parser::NodeLink nodeLink){
    return nodeHasLink(cur, nodeLink);
}

xmlNode * Parser::getNodeLink(xmlNode * node, Parser::NodeLink nodeLink){
    gmlCheck(node, gmlError, __FUNCTION__, __FILE__, __LINE__, "node is NULL");
    switch (nodeLink)
    {
    case CHILDREN:
        gmlCheck(node->children, gmlError, __FUNCTION__, __FILE__, __LINE__, "node->children is NULL");
        return node->children;
    case PARENT:
        gmlCheck(node->parent, gmlError, __FUNCTION__, __FILE__, __LINE__, "node->parent is NULL");
        return node->parent;
    case NEXT:
        gmlCheck(node->next, gmlError, __FUNCTION__, __FILE__, __LINE__, "node->next is NULL");
        return node->next;
    case PREV:
        gmlCheck(node->prev, gmlError, __FUNCTION__, __FILE__, __LINE__, "node->prev is NULL");
        return node->prev;
    }
}

void Parser::getNodeLink(NodeLink nodeLink){
    cur = getNodeLink(cur, nodeLink);
}

void Parser::readGraphmlInfo(){
    const static std::vector<std::string> supportedEngines = {"cairo", "default"};
    const static std::vector<std::string> supportedModes = {"relative", "pixel"};
    const static std::vector<std::string> supportedVersions = {"0.0.1", "latest"};

    auto info = getXmlNodeAttributes(document, cur);
    for(auto [key, value] : info){

        if(key == "version"){
            if(value == supportedVersions){
                version = value;
            } else {
                gmlWarn(__FUNCTION__,__FILE__, __LINE__, "unsupported version, \"latest\" will be used.");
            }
        } else if(key == "mode"){
            if(value == supportedModes){
                engine = value;
            } else {
                gmlWarn(__FUNCTION__,__FILE__, __LINE__, "unsupported mode, \"pixel\" will be used.");
            }
        } else if(key == "engine"){
            if(value == supportedEngines){
                engine = value;
            } else {
                gmlWarn(__FUNCTION__,__FILE__, __LINE__, "unsupported engine, \"cairo\" will be used.");
            }
        }
    }
} 
