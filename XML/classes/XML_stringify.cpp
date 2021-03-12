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
    void XML::stringifyXML(XNode *xNode, IDestination &xmlDestination)
    {
        switch (xNode->getNodeType())
        {
        case XNodeType::prolog:
        {
            xmlDestination.add("<?xml version=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[0].value.unparsed + "\"" +
                        " encoding=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[1].value.unparsed + "\"" +
                        " standalone=\"" + XNodeRef<XNodeElement>((*xNode)).attributes[2].value.unparsed + "\"?>");
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).elements)
            {
                stringifyXML(element.get(), xmlDestination);
            }
            break;
        }
        case XNodeType::root:
        case XNodeType::element:
        {
            XNodeElement *xNodeElement = static_cast<XNodeElement *>(xNode);
            xmlDestination.add("<" + xNodeElement->name);
            for (auto attr : xNodeElement->attributes)
            {
                xmlDestination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
            }
            xmlDestination.add(">");
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).elements)
            {
                stringifyXML(element.get(), xmlDestination);
            }
            xmlDestination.add("</" + xNodeElement->name + ">");
            break;
        }
        case XNodeType::self:
        {
            XNodeElement *xNodeElement = static_cast<XNodeElement *>(xNode);
            xmlDestination.add("<" + xNodeElement->name);
            for (auto attr : xNodeElement->attributes)
            {
                xmlDestination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
            }
            xmlDestination.add("/>");
            break;
        }
        case XNodeType::comment:
        {
            XNodeComment *xNodeComment = static_cast<XNodeComment *>(xNode);
            xmlDestination.add("<!--" + xNodeComment->comment + "-->");
            break;
        }
        case XNodeType::content:
        {
            XNodeContent *xNodeContent = static_cast<XNodeContent *>(xNode);
            xmlDestination.add(xNodeContent->content);
            break;
        }
        case XNodeType::entity:
        {
            XNodeEntityReference *xNodeEntity = static_cast<XNodeEntityReference *>(xNode);
            xmlDestination.add(xNodeEntity->value.unparsed);
            break;
        }
        case XNodeType::pi:
        {
            XNodePI *xNodePI = static_cast<XNodePI *>(xNode);
            xmlDestination.add("<?" + xNodePI->name + " " + xNodePI->parameters + "?>");
            break;
        }
        case XNodeType::cdata:
        {
            XNodeCDATA *xNodeCDATA = static_cast<XNodeCDATA *>(xNode);
            xmlDestination.add("<![CDATA[" + xNodeCDATA->cdata + "]]>");
            break;
        }
        case XNodeType::dtd:
        {
            XNodeDTD *xNodeDTD = static_cast<XNodeDTD *>(xNode);
            xmlDestination.add(xNodeDTD->unparsed);
            break;
        }
        default:
            break;
        }
    }
} // namespace H4
