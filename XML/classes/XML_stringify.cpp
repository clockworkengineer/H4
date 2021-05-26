//
// Class: XML
//
// Description:  Code to take an XML internal object description and produce 
// the XML text for it; currently as UTF-8 encoded text but this may change in 
// future.
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
    /// Recursively parse XMLNode passed in to produce XML output on an XML
    /// destination stream in UTF-8 encoding.
    /// </summary>
    /// <param name="xmlNode">XMLNode to convert into XML.</param>
    /// <param name="xmlDestination">XML destination stream.</param>
    void XML::stringifyElements(XMLNode *xmlNode, IDestination &xmlDestination)
    {
        switch (xmlNode->getNodeType())
        {
        // XML prolog
        case XMLNodeType::prolog:
        {
            xmlDestination.add("<?xml version=\"" + XMLNodeRef<XMLNodeElement>((*xmlNode)).getAttribute("version").value.unparsed + "\"" +
                               " encoding=\"" + XMLNodeRef<XMLNodeElement>((*xmlNode)).getAttribute("encoding").value.unparsed + "\"" +
                               " standalone=\"" + XMLNodeRef<XMLNodeElement>((*xmlNode)).getAttribute("standalone").value.unparsed + "\"?>");
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                stringifyElements(element.get(), xmlDestination);
            }
            break;
        }
        // XML root or child elements
        case XMLNodeType::root:
        case XMLNodeType::element:
        {
            XMLNodeElement *xmlNodeElement = static_cast<XMLNodeElement *>(xmlNode);
            xmlDestination.add("<" + xmlNodeElement->name);
            for (auto attr : xmlNodeElement->getAttributeList())
            {
                xmlDestination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
            }
            xmlDestination.add(">");
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                stringifyElements(element.get(), xmlDestination);
            }
            xmlDestination.add("</" + xmlNodeElement->name + ">");
            break;
        }
        // Self closing element
        case XMLNodeType::self:
        {
            XMLNodeElement *xmlNodeElement = static_cast<XMLNodeElement *>(xmlNode);
            xmlDestination.add("<" + xmlNodeElement->name);
            for (auto attr : xmlNodeElement->getAttributeList())
            {
                xmlDestination.add(" " + attr.name + "=\"" + attr.value.unparsed + "\"");
            }
            xmlDestination.add("/>");
            break;
        }
        // XML comments
        case XMLNodeType::comment:
        {
            XMLNodeComment *xmlNodeComment = static_cast<XMLNodeComment *>(xmlNode);
            xmlDestination.add("<!--" + xmlNodeComment->comment + "-->");
            break;
        }
        // XML element content
        case XMLNodeType::content:
        {
            XMLNodeContent *xmlNodeContent = static_cast<XMLNodeContent *>(xmlNode);
            xmlDestination.add(xmlNodeContent->content);
            break;
        }
        // XML character entity
        case XMLNodeType::entity:
        {
            XMLNodeEntityReference *xmlNodeEntity = static_cast<XMLNodeEntityReference *>(xmlNode);
            xmlDestination.add(xmlNodeEntity->value.unparsed);
            break;
        }
        // XML processing instruction
        case XMLNodeType::pi:
        {
            XMLNodePI *xmlNodePI = static_cast<XMLNodePI *>(xmlNode);
            xmlDestination.add("<?" + xmlNodePI->name + " " + xmlNodePI->parameters + "?>");
            break;
        }
        // XML CDATA section
        case XMLNodeType::cdata:
        {
            XMLNodeCDATA *xmlNodeCDATA = static_cast<XMLNodeCDATA *>(xmlNode);
            xmlDestination.add("<![CDATA[" + xmlNodeCDATA->cdata + "]]>");
            break;
        }
        // XML DTD
        case XMLNodeType::dtd:
        {
            m_dtd.stringify(xmlDestination);
            break;
        }
        default:
            throw std::runtime_error("Invalid XMLNode encountered during stringify.");
        }
    }
    /// <summary>
    /// Recursively parse XMLNode passed in to produce XML output on an XML
    /// destination stream in UTF-8 encoding.
    /// </summary>
    /// <param name="xmlDestination">XML destination stream.</param>
    void XML::stringifyXML(IDestination &xmlDestination)
    {
        stringifyElements(&m_prolog, xmlDestination);
    }
} // namespace H4
