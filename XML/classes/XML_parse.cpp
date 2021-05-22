//
// Class: XML
//
// Description: XML parser code. All parsing of characters takes place having
// converted the characters to UTF-32 to make the process easier.
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
// #include <filesystem>
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
    void XML::parseAddElementContent(XNodeElement *xNodeElement, const std::string &content)
    {
        // Make sure there is a content node to recieve characters
        if (xNodeElement->children.empty() ||
            xNodeElement->children.back()->getNodeType() != XNodeType::content)
        {
            bool isWWhitespace = true;
            if (!xNodeElement->children.empty())
            {
                if ((xNodeElement->children.back()->getNodeType() == XNodeType::cdata) ||
                    (xNodeElement->children.back()->getNodeType() == XNodeType::entity))
                {
                    isWWhitespace = false;
                }
            }
            xNodeElement->children.emplace_back(std::make_unique<XNodeContent>());
            XNodeRef<XNodeContent>(*xNodeElement->children.back()).isWhiteSpace = isWWhitespace;
        }
        if (XNodeRef<XNodeContent>(*xNodeElement->children.back()).isWhiteSpace)
        {
            for (auto ch : content)
            {
                if (!std::iswspace(ch))
                {
                    XNodeRef<XNodeContent>(*xNodeElement->children.back()).isWhiteSpace = false;
                    break;
                }
            }
        }
        XNodeRef<XNodeContent>(*xNodeElement->children.back()).content += content;
    }
    /// <summary>
    /// Parse a element tag name and set its value in current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseTagName(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        xNodeElement->name = parseName(xmlSource);
    }
    /// <summary>
    /// Parse a XML comment, create a XNodeComment for it and add to list
    /// of elements for the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseComment(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodeComment xNodeComment;
        while (xmlSource.more() && !xmlSource.match(U"--"))
        {
            xNodeComment.comment += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        if (!xmlSource.match(U">"))
        {
            throw SyntaxError(xmlSource, "Missing closing '>' for comment line.");
        }
        xNodeElement->children.emplace_back(std::make_unique<XNodeComment>(std::move(xNodeComment)));
    }
    /// <summary>
    /// Parse a XML process instruction, create an XNode for it and add it to
    /// the list of elements under the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parsePI(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodePI xNodePI;
        xNodePI.name = parseName(xmlSource);
        while (xmlSource.more() && !xmlSource.match(U"?>"))
        {
            xNodePI.parameters += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        xNodeElement->children.emplace_back(std::make_unique<XNodePI>(std::move(xNodePI)));
    }
    /// <summary>
    /// Parse an XML CDATA section create an XNode for it and add it to
    /// the list of elements under the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseCDATA(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodeCDATA xNodeCDATA;
        while (xmlSource.more() && !xmlSource.match(U"]]>"))
        {
            if (xmlSource.match(U"<![CDATA["))
            {
                throw SyntaxError(xmlSource, "Nesting of CDATA sections is not allowed.");
            }
            xNodeCDATA.cdata += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        if (!xNodeElement->children.empty())
        {
            if (xNodeElement->children.back()->getNodeType() == XNodeType::content)
            {
                XNodeRef<XNodeContent>(*xNodeElement->children.back()).isWhiteSpace = false;
            }
        }
        xNodeElement->children.emplace_back(std::make_unique<XNodeCDATA>(std::move(xNodeCDATA)));
    }
    /// <summary>
    /// Parse list of attributes (name/value pairs) that exist in a tag and add them to
    /// the list of attributes associated with the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseAttributes(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        while (xmlSource.more() &&
               xmlSource.current() != '?' &&
               xmlSource.current() != '/' &&
               xmlSource.current() != '>')
        {
            std::string attributeName = parseName(xmlSource);
            if (!xmlSource.match(U"="))
            {
                throw SyntaxError(xmlSource, "Missing '=' between attribute name and value.");
            }
            xmlSource.ignoreWS();
            XValue attributeValue = parseValue(xmlSource, m_dtd.m_entityMapping);
            if (!validAttributeValue(attributeValue))
            {
                throw SyntaxError(xmlSource, "Attribute value contains invalid character '<', '\"', ''' or '&'.");
            }
            if (!xNodeElement->isAttributePresent(attributeName))
            {
                xNodeElement->addAttribute(attributeName, attributeValue);
            }
            else
            {
                throw SyntaxError(xmlSource, "Attribute defined more than once within start tag.");
            }
        }
        for (auto attribute : xNodeElement->getAttributeList())
        {
            if (attribute.name.starts_with("xmlns"))
            {
                attribute.name = (attribute.name.size() > 5) ? attribute.name.substr(6) : ":";
                xNodeElement->addNameSpace(attribute.name, attribute.value);
            }
        }
    }
    /// <summary>
    /// Recursively parse any child elements of the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseChildElement(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodeElement xNodeChildElement;
        for (auto &ns : xNodeElement->getNameSpaceList())
        {
            xNodeChildElement.addNameSpace(ns.name, ns.value);
        }
        parseElement(xmlSource, &xNodeChildElement);
        if (auto pos = xNodeChildElement.name.find(':'); pos != std::string::npos)
        {
            if (!xNodeChildElement.isNameSpacePresent(xNodeChildElement.name.substr(0, pos)))
            {
                throw SyntaxError(xmlSource, "Namespace used but not defined.");
            }
        }
        xNodeElement->children.push_back(std::make_unique<XNodeElement>(std::move(xNodeChildElement)));
    }
    /// <summary>
    /// Parse any element content that is found.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDefault(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XValue entityReference = parseCharacter(xmlSource, m_dtd.m_entityMapping);
        if (entityReference.unparsed.starts_with("&") && entityReference.unparsed.ends_with(";"))
        {
            parseEntityMappingContents(xNodeElement, entityReference);
        }
        else
        {
            parseAddElementContent(xNodeElement, entityReference.parsed);
        }
    }
    /// <summary>
    /// Parse element content area generating any XNodes and adding them
    /// to the list of the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XMl source stream.</param>
    /// <returns></returns>
    void XML::parseElementContents(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        if (xmlSource.match(U"<!--"))
        {
            parseComment(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"<?"))
        {
            parsePI(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"<![CDATA["))
        {
            parseCDATA(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"</"))
        {
            throw SyntaxError(xmlSource, "Missing closing tag.");
        }
        else if (xmlSource.match(U"<"))
        {
            parseChildElement(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"]]>"))
        {
            throw SyntaxError(xmlSource, "']]>' invalid in element content area.");
        }
        else
        {
            parseDefault(xmlSource, xNodeElement);
        }
    }
    /// <summary>
    /// Parse current XML element found.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseElement(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        parseTagName(xmlSource, xNodeElement);
        parseAttributes(xmlSource, xNodeElement);
        if (xmlSource.match(U">"))
        {
            while (xmlSource.more() && !xmlSource.match(U"</"))
            {
                parseElementContents(xmlSource, xNodeElement);
            }
            if (!xmlSource.match(xmlSource.from_bytes(xNodeElement->name) + U">"))
            {
                throw SyntaxError(xmlSource, "Missing closing tag.");
            }
        }
        else if (xmlSource.match(U"/>"))
        {
            // Self closing element tag
            xNodeElement->setNodeType(XNodeType::self);
        }
    }
    /// <summary>
    /// Parse XML prolog and create the necessary XNodeElements for it. Valid
    /// parts of the prolog include delaration (first line if present),
    /// processing instructions, comments, whitespace content and XML
    /// document Type Definition (DTD).
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseProlog(ISource &xmlSource, XNodeElement *xNodeProlog)
    {
        xNodeProlog->setNodeType(XNodeType::prolog);
        xmlSource.ignoreWS();
        if (xmlSource.match(U"<?xml"))
        {
            xmlSource.ignoreWS();
            parseAttributes(xmlSource, xNodeProlog);
            if (!xmlSource.match(U"?>"))
            {
                throw SyntaxError(xmlSource, "Declaration end tag not found.");
            }
            validXMLDeclaration(xmlSource, xNodeProlog);
        }
        while (xmlSource.more())
        {
            if (xmlSource.match(U"<!--"))
            {
                parseComment(xmlSource, xNodeProlog);
            }
            else if (xmlSource.match(U"<?"))
            {
                parsePI(xmlSource, xNodeProlog);
            }
            else if (xmlSource.match(U"<!DOCTYPE"))
            {
                m_dtd.parse(xmlSource, xNodeProlog);
            }
            else if (xmlSource.current() == '<')
            {
                // Break out as root element detected
                break;
            }
            else if (std::iswspace(xmlSource.current()))
            {
                parseAddElementContent(xNodeProlog, std::string(1, xmlSource.current()));
                xmlSource.next();
            }
            else
            {
                throw SyntaxError(xmlSource, "Content detected before root element.");
            }
        }
    }
    /// <summary>
    /// Parse XML source stream.
    /// </summary>
    /// <returns></returns>
    void XML::parseXML()
    {
        parseProlog(m_xmlSource, &m_prolog);
        if (m_xmlSource.match(U"<"))
        {
            m_prolog.children.emplace_back(std::make_unique<XNodeElement>(XNodeElement(XNodeType::root)));
            parseElement(m_xmlSource, static_cast<XNodeElement *>(m_prolog.children.back().get()));
            while (m_xmlSource.more())
            {
                if (m_xmlSource.match(U"<!--"))
                {
                    parseComment(m_xmlSource, (&m_prolog));
                }
                else if (std::iswspace(m_xmlSource.current()))
                {
                    parseAddElementContent(&m_prolog, std::string(1, m_xmlSource.current()));
                    m_xmlSource.next();
                }
                else
                {
                    throw SyntaxError(m_xmlSource, "Extra content at the end of document.");
                }
            }
        }
        else
        {
            throw SyntaxError(m_xmlSource, "Missing root element.");
        }
    }
} // namespace H4
