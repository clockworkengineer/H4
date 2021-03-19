//
// Class: XML
//
// Description: XML Document Type Definition parsing.
//
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
    void XML::dtdValidateElement(ISource &xmlSource, XNodeDTD *dtd, XNodeElement *xNodeElement) 
    {
        if ((dtd==nullptr)||(dtd->elements.empty())) {
            return;
        }
        if (dtd->elements.count(xNodeElement->name)==0) {
             throw SyntaxError(xmlSource, "Undefined element '"+xNodeElement->name+"' found.");
        }
        if (dtd->elements[xNodeElement->name].content=="(#PCDATA)") {
            return;
        }
    }
} // namespace H4
