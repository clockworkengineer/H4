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
#include <filesystem>
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
    void XML::xmlParseAddElementContent(XNodeElement *xNodeElement, const std::string &content)
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
    /// Parse character value which can be either be a plain character
    /// or character reference/entity string that maps to a string of
    /// characters.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Character value.</returns>
    XValue XML::xmlParseCharacter(ISource &xmlSource)
    {
        XValue character;
        if (xmlSource.current() == '&')
        {
            character = xmlParseReferenceOrEntity(xmlSource);
        }
        else if (validChar(xmlSource.current()))
        {
            character.parsed = xmlSource.current_to_bytes();
            character.unparsed = character.parsed;
            xmlSource.next();
        }
        else
        {
            throw SyntaxError(xmlSource, "Invalid character value encountered.");
        }
        return (character);
    }
    /// <summary>
    /// Parse string value from XML source stream and return it. Note: The
    /// string may be encased in either single or double quotes.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>String value.</returns>
    XValue XML::xmlParseValue(ISource &xmlSource, bool translateEntity)
    {
        if ((xmlSource.current() == '\'') || ((xmlSource.current() == '"')))
        {
            XValue value;
            XChar quote = xmlSource.current();
            xmlSource.next();
            while (xmlSource.more() && xmlSource.current() != quote)
            {
                XValue character = xmlParseCharacter(xmlSource);
                if (translateEntity ||
                    character.unparsed.starts_with("&#"))
                {
                    value.parsed += character.parsed;
                }
                else
                {
                    value.parsed += character.unparsed;
                }
                value.unparsed += character.unparsed;
            }
            xmlSource.next();
            xmlSource.ignoreWS();
            return (value);
        }
        throw SyntaxError(xmlSource, "Invalid attribute value.");
    }
    /// <summary>
    /// Parse an XML name and return it.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Valid XML name.</returns>
    std::string XML::xmlParseName(ISource &xmlSource)
    {
        XString name;
        while (xmlSource.more() && validNameChar(xmlSource.current()))
        {
            name += xmlSource.current();
            xmlSource.next();
        }
        xmlSource.ignoreWS();
        if (!validName(name))
        {
            throw SyntaxError(xmlSource, "Invalid name '" + xmlSource.to_bytes(name) + "' encountered.");
        }
        return (xmlSource.to_bytes(name));
    }
    /// <summary>
    /// Parse a element tag name and set its value in current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::xmlParseTagName(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        xNodeElement->name = xmlParseName(xmlSource);
    }
    /// <summary>
    /// Parse a XML comment, create a XNodeComment for it and add to list
    /// of elements for the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::xmlParseComment(ISource &xmlSource, XNodeElement *xNodeElement)
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
    void XML::xmlParsePI(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodePI xNodePI;
        xNodePI.name = xmlParseName(xmlSource);
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
    void XML::xmlParseCDATA(ISource &xmlSource, XNodeElement *xNodeElement)
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
    void XML::xmlParseAttributes(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        while (xmlSource.more() &&
               xmlSource.current() != '?' &&
               xmlSource.current() != '/' &&
               xmlSource.current() != '>')
        {
            std::string attributeName = xmlParseName(xmlSource);
            if (!xmlSource.match(U"="))
            {
                throw SyntaxError(xmlSource, "Missing '=' between attribute name and value.");
            }
            xmlSource.ignoreWS();
            XValue attributeValue = xmlParseValue(xmlSource);
            if (!validAttributeValue(attributeValue)) {
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
    void XML::xmlParseChildElement(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodeElement xNodeChildElement;
        for (auto &ns : xNodeElement->getNameSpaceList()) {
            xNodeChildElement.addNameSpace(ns.name, ns.value);
        }
        xmlParseElement(xmlSource, &xNodeChildElement);
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
    void XML::xmlParseDefault(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XValue entityReference = xmlParseCharacter(xmlSource);
        if (!xmlEntityMappingParse(xNodeElement, entityReference))
        {
            xmlParseAddElementContent(xNodeElement, entityReference.parsed);
        }
    }
    /// <summary>
    /// Parse element content area generating any XNodes and adding them 
    /// to the list of the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XMl source stream.</param>
    /// <returns></returns>
    void XML::xmlParseElementContents(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        if (xmlSource.match(U"<!--"))
        {
            xmlParseComment(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"<?"))
        {
            xmlParsePI(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"<![CDATA["))
        {
            xmlParseCDATA(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"</"))
        {
            throw SyntaxError(xmlSource, "Missing closing tag.");
        }
        else if (xmlSource.match(U"<"))
        {
            xmlParseChildElement(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"]]>"))
        {
            throw SyntaxError(xmlSource, "']]>' invalid in element content area.");
        }
        else
        {
            xmlParseDefault(xmlSource, xNodeElement);
        }
    }
    /// <summary>
    /// Parse current XML element found.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::xmlParseElement(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        xmlParseTagName(xmlSource, xNodeElement);
        xmlParseAttributes(xmlSource, xNodeElement);
        if (xmlSource.match(U">"))
        {
            while (xmlSource.more() && !xmlSource.match(U"</"))
            {
                xmlParseElementContents(xmlSource, xNodeElement);
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
    void XML::xmlParseProlog(ISource &xmlSource, XNodeElement *xNodeProlog)
    {
        xNodeProlog->setNodeType(XNodeType::prolog);
        xmlSource.ignoreWS();
        if (xmlSource.match(U"<?xml"))
        {
            xmlSource.ignoreWS();
            xmlParseAttributes(xmlSource, xNodeProlog);
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
                xmlParseComment(xmlSource, xNodeProlog);
            }
            else if (xmlSource.match(U"<?"))
            {
                xmlParsePI(xmlSource, xNodeProlog);
            }
            else if (xmlSource.match(U"<!DOCTYPE"))
            {
                dtdParse(xmlSource, xNodeProlog);
            }
            else if (xmlSource.current() == '<')
            {
                // Break out as root element detected
                break;
            }
            else if (std::iswspace(xmlSource.current()))
            {
                xmlParseAddElementContent(xNodeProlog, std::string(1, xmlSource.current()));
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
    void XML::xmlParse()
    {
        xmlParseProlog(m_xmlSource, &m_prolog);
        if (m_xmlSource.match(U"<"))
        {
            m_prolog.children.emplace_back(std::make_unique<XNodeElement>(XNodeElement(XNodeType::root)));
            xmlParseElement(m_xmlSource, static_cast<XNodeElement *>(m_prolog.children.back().get()));
            while (m_xmlSource.more())
            {
                if (m_xmlSource.match(U"<!--"))
                {
                    xmlParseComment(m_xmlSource, (&m_prolog));
                }
                else if (std::iswspace(m_xmlSource.current()))
                {
                    xmlParseAddElementContent(&m_prolog, std::string(1, m_xmlSource.current()));
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
