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
    std::string XML::stringifyXML(XNode *xNode)
    {
        std::string xmlResult;
        switch (xNode->getNodeType())
        {
        case XNodeType::prolog:
        {
            xmlResult = "<?xml version=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[0].value + "\"" +
                        " encoding=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[1].value + "\"" +
                        " standalone=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[2].value + "\"?>";
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).elements)
            {
                xmlResult += stringifyXML(element.get());
            }
            break;
        }
        case XNodeType::root:
        case XNodeType::element:
        {
            XNodeElement *xNodeElement = static_cast<XNodeElement *>(xNode);
            xmlResult += "<" + xNodeElement->name;
            for (auto attr : xNodeElement->attributes)
            {
                xmlResult += " " + attr.name + "=\"" + attr.value + "\"";
            }
            xmlResult += ">";
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).elements)
            {
                xmlResult += stringifyXML(element.get());
            }
            xmlResult += "</" + xNodeElement->name + ">";
            break;
        }
        case XNodeType::self:
        {
            XNodeElement *xNodeElement = static_cast<XNodeElement *>(xNode);
            xmlResult += "<" + xNodeElement->name;
            for (auto attr : xNodeElement->attributes)
            {
                xmlResult += " " + attr.name + "=\"" + attr.value + "\"";
            }
            xmlResult += "/>";
            break;
        }
        case XNodeType::comment:
        {
            XNodeComment *xNodeComment = static_cast<XNodeComment *>(xNode);
            xmlResult += "<!--" + xNodeComment->comment + "-->";
            break;
        }
        case XNodeType::content:
        {
            XNodeContent *xNodeContent = static_cast<XNodeContent *>(xNode);
            xmlResult += xNodeContent->content;
            break;
        }
        default:
            break;
        }
        return (xmlResult);
    }
} // namespace H4
