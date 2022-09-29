#pragma once

#include <libxml2/libxml/xmlIO.h>
#include <libxml2/libxml/xpath.h>
#include <unordered_map>
#include <string>

/**
 * @brief Graphml file parser
 * 
 */
class Parser {
public:
    enum NodeLink {
        CHILDREN,
        PARENT,
        NEXT,
        PREV,
    };

    /**
     * @brief The graphml document
     * Stores the xml parsed document.
     */
    xmlDoc * document;
    /**
     * @brief Current node.
     * Used to manually walk through the document, finding and resolving the elements.
     */
    xmlNode * cur;

    /**
     * @brief Version of the graphml interpreter.
     */
    std::string version = "latest";

    /**
     * @brief The engine to be used.
     */
    std::string engine = "cairo";

    /**
     * @brief coordinates mode.
     * may be "relative" for values in [0;1]
     * or "pixel" for pixel values.
     */
    std::string mode = "pixel";
    /**
     * @brief Construct a new Parser object
     * 
     * @param filename the filename of the document
     */
    Parser(const std::string filename);

    /**
     * @brief Destroys a Parser
     */
    ~Parser();

    /**
     * @brief returns an unordered map containing of the Xml Node Attributes
     * 
     * @param node the node to read 
     * @param document the gml document
     * @return std::unordered_map<std::string, std::string 
     */
    static std::unordered_map<std::string, std::string> getXmlNodeAttributes(xmlDoc * document, xmlNode * node);
    
    /**
     * @brief returns an unordered map containing of the current Xml Node Attributes
     * 
     * @return std::unordered_map<std::string, std::string> 
     * 
     */
    std::unordered_map<std::string, std::string> getXmlNodeAttributes();

    /**
     * @brief tries to return a required attribute from an attribute set.
     * 
     * @param elementName the node name
     * @param elementLine the node line
     * @param attributeName the attribute name 
     * @param attributeSet an attribute set
     * @param optional to avoid errors when the attribute is not found
     * @return std::string the required value
     */
    std::string getRequiredAttribute(std::string elementName, long elementLine, std::string attributeName, std::unordered_map<std::string, std::string> attributeSet, bool optional = false);
    
    /**
     * @brief returns the name of the @a node
     * 
     * @param node the node
     * @return std::string the name
     */
    static std::string getXmlNodeName(xmlNode * node);

    /**
     * @brief returns the name of the current node
     * 
     * @return std::string the name
     */
    std::string getXmlNodeName();

        /**
     * @brief returns the line of the @a node
     * 
     * @param node the node
     * @return long the line
     */
    static long getXmlNodeLine(xmlNode * node);

    /**
     * @brief returns the line of the current node
     * 
     * @return std::string the line
     */
    long getXmlNodeLine();

    /**
     * @brief Check if the @a node has children
     * 
     * @param node the node to check 
     * @param nodeLink the link type
     * @return true if the node has children
     * @return false if the node has not children
     */
    static bool nodeHasLink(xmlNode * node, NodeLink nodeLink);

    /**
     * @brief Check if the current node has @a NodeLink.
     * 
     */
    bool nodeHasLink(NodeLink nodeLink);
    
    /**
     * @brief Get the @a NodeLink of the node
     * 
     * @param node the node
     * @param nodeLink the link type
     * @return xmlNode* children node
     */
    static xmlNode * getNodeLink(xmlNode * node, NodeLink nodeLink);

    /**
     * @brief tries do change the current node to its @a NodeLink.
     */
    void getNodeLink(NodeLink nodeLink);
private:
    /**
     * @brief Reads the graphml element attributes if possible, called in the constructor.
     * 
     */
    void readGraphmlInfo();
};