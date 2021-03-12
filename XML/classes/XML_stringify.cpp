//
// Class: XML
//
// Description: XMLObject strinigification code.
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
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    std::string XML::stringifyXML(XNode *xNode)
    {
        std::string xmlResult;
        switch (xNode->getNodeType())
        {
        case XNodeType::prolog:
        {
            xmlResult = "<?xml version=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[0].value.unparsed + "\"" +
                        " encoding=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[1].value.unparsed + "\"" +
                        " standalone=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[2].value.unparsed + "\"?>";
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
                xmlResult += " " + attr.name + "=\"" + attr.value.unparsed + "\"";
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
                xmlResult += " " + attr.name + "=\"" + attr.value.unparsed + "\"";
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
        case XNodeType::entity:
        {
            XNodeEntityReference *xNodeEntity = static_cast<XNodeEntityReference *>(xNode);
            xmlResult += xNodeEntity->value.unparsed;
            break;
        }
        case XNodeType::pi:
        {
            XNodePI *xNodePI = static_cast<XNodePI *>(xNode);
            xmlResult += "<?" + xNodePI->name + " " + xNodePI->parameters + "?>";
            break;
        }
        case XNodeType::cdata:
        {
            XNodeCDATA *xNodeCDATA = static_cast<XNodeCDATA *>(xNode);
            xmlResult += "<![CDATA[" + xNodeCDATA->cdata + "]]>";
            break;
        }
        case XNodeType::dtd:
        {
            XNodeDTD *xNodeDTD = static_cast<XNodeDTD *>(xNode);
            xmlResult += xNodeDTD->unparsed;
            break;
        }
        default:
            break;
        }
        return (xmlResult);
    }
} // namespace H4
