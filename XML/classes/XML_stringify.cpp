//
// Class: XML
//
// Description:
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XMLConfig.hpp"
#include "XML.hpp"
#include "XMLSources.hpp"
#include "XMLDestinations.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
//
// C++ STL
//
#include <vector>
#include <cstring>
#include <algorithm>
// =========
// NAMESPACE
// =========
namespace H4
{
    // ===========================
    // PRIVATE TYPES AND CONSTANTS
    // ===========================
    // ==========================
    // PUBLIC TYPES AND CONSTANTS
    // ==========================
    // ========================
    // PRIVATE STATIC VARIABLES
    // ========================
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    std::string XML::stringifyXML(XNode* xNodeRoot) {
        std::string xmlResult;
        if (xNodeRoot->getNodeType()==XNodeType::prolog) {
            xmlResult =  "<?xml version = \""+XNodeRef<XNodeElement>((*xNodeRoot)).attributes[0].value+"\""+
            " encoding = \""+XNodeRef<XNodeElement>((*xNodeRoot)).attributes[1].value+"\""+
            " standalone = \""+XNodeRef<XNodeElement>((*xNodeRoot)).attributes[2].value+"\"?>"; 
        }
        return(xmlResult);
    }

} // namespace H4
