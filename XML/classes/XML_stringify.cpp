//
// Class: XML
//
// Description:  Code to take an XMLObject and produce the XML for
// it; currently as UTF-8 encoded text but this may change in future.
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
    /// Recursively parse XNode root passed in to produce XML output on an XML
    /// destination stream in UTF-8 encoding.
    /// </summary>
    /// <param name="xNodeRoot">XML XNode root to convert into XML.</param>
    /// <returns></returns>
    void XML::stringifyElements(XNode *xNode, IDestination &xmlDestination)
    {
        switch (xNode->getNodeType())
        {
        // XML prolog
        case XNodeType::prolog:
        {
            xmlDestination.add("<?xml version=\"" + XNodeRef<XNodeElement>((*xNode)).getAttribute("version").value.unparsed + "\"" +
                               " encoding=\"" + XNodeRef<XNodeElement>((*xNode)).getAttribute("encoding").value.unparsed + "\"" +
                               " standalone=\"" + XNodeRef<XNodeElement>((*xNode)).getAttribute("standalone").value.unparsed + "\"?>");
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).children)
            {
                stringifyElements(element.get(), xmlDestination);
            }
            break;
        }
        // XML root or child elements
        case XNodeType::root:
        case XNodeType::element:
        {
            XNodeElement *xNodeElement = static_cast<XNodeElement *>(xNode);
            xmlDestination.add("<" + xNodeElement->name);
            for (auto attr : xNodeElement->getAttributeList())
            {
                xmlDestination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
            }
            xmlDestination.add(">");
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).children)
            {
                stringifyElements(element.get(), xmlDestination);
            }
            xmlDestination.add("</" + xNodeElement->name + ">");
            break;
        }
        // Self closing element
        case XNodeType::self:
        {
            XNodeElement *xNodeElement = static_cast<XNodeElement *>(xNode);
            xmlDestination.add("<" + xNodeElement->name);
            for (auto attr : xNodeElement->getAttributeList())
            {
                xmlDestination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
            }
            xmlDestination.add("/>");
            break;
        }
        // XML comments
        case XNodeType::comment:
        {
            XNodeComment *xNodeComment = static_cast<XNodeComment *>(xNode);
            xmlDestination.add("<!--" + xNodeComment->comment + "-->");
            break;
        }
        // XML element content
        case XNodeType::content:
        {
            XNodeContent *xNodeContent = static_cast<XNodeContent *>(xNode);
            xmlDestination.add(xNodeContent->content);
            break;
        }
        // XML character entity
        case XNodeType::entity:
        {
            XNodeEntityReference *xNodeEntity = static_cast<XNodeEntityReference *>(xNode);
            xmlDestination.add(xNodeEntity->value.unparsed);
            break;
        }
        // XML processing instruction
        case XNodeType::pi:
        {
            XNodePI *xNodePI = static_cast<XNodePI *>(xNode);
            xmlDestination.add("<?" + xNodePI->name + " " + xNodePI->parameters + "?>");
            break;
        }
        // XML CDATA section
        case XNodeType::cdata:
        {
            XNodeCDATA *xNodeCDATA = static_cast<XNodeCDATA *>(xNode);
            xmlDestination.add("<![CDATA[" + xNodeCDATA->cdata + "]]>");
            break;
        }
        // XML DTD
        case XNodeType::dtd:
        {
            XNodeDTD *xNodeDTD = static_cast<XNodeDTD *>(xNode);
            xmlDestination.add(xNodeDTD->unparsed);
            break;
        }
        default:
            throw std::runtime_error("Invalid XNode encountered during stringify.");
        }
    }
    /// <summary>
    /// Recursively parse XNode root passed in to produce XML output on an XML
    /// destination stream in UTF-8 encoding.
    /// </summary>
    /// <param name="xNodeRoot">XML XNode root to convert into XML.</param>
    /// <returns></returns>
    void XML::stringifyXML(IDestination &xmlDestination)
    {
        stringifyElements(&m_prolog, xmlDestination);
    }
} // namespace H4
