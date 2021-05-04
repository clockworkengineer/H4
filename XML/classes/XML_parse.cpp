//
// Class: XML
//
// Description: XML parser code. All parsing of characrters takes place having
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
        if (xNodeElement->elements.empty() ||
            xNodeElement->elements.back()->getNodeType() != XNodeType::content)
        {
            bool isWWhitespace = true;
            if (!xNodeElement->elements.empty())
            {
                if ((xNodeElement->elements.back()->getNodeType() == XNodeType::cdata) ||
                    (xNodeElement->elements.back()->getNodeType() == XNodeType::entity))
                {
                    isWWhitespace = false;
                }
            }
            xNodeElement->elements.emplace_back(std::make_unique<XNodeContent>());
            XNodeRef<XNodeContent>(*xNodeElement->elements.back()).isWhiteSpace = isWWhitespace;
        }
        if (XNodeRef<XNodeContent>(*xNodeElement->elements.back()).isWhiteSpace)
        {
            for (auto ch : content)
            {
                if (!std::iswspace(ch))
                {
                    XNodeRef<XNodeContent>(*xNodeElement->elements.back()).isWhiteSpace = false;
                    break;
                }
            }
        }
        XNodeRef<XNodeContent>(*xNodeElement->elements.back()).content += content;
    }
    /// <summary>
    /// Parse a character reference returning its value.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Character reference value.</returns>
    std::string XML::xmlParseCharacterReference(ISource &xmlSource, std::string reference)
    {
        char *end;
        long result = 10;
        if (reference[0] == 'x')
        {
            reference = reference.substr(1);
            result = 16;
        }
        result = std::strtol(reference.c_str(), &end, result);
        if (*end == '\0')
        {
            if (!validChar(result))
            {
                throw SyntaxError(xmlSource, "Character reference invalid character.");
            }
            return (xmlSource.to_bytes(result));
        }
        throw SyntaxError(xmlSource, "Cannot convert character reference.");
    }
    /// <summary>
    /// Parse XML string value representing a character reference or entity.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Value for reference or enity.</returns>
    XValue XML::xmlParseReferenceOrEntity(ISource &xmlSource)
    {
        XValue entityReference;
        while (xmlSource.more() && xmlSource.current() != ';')
        {
            entityReference.unparsed += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        if (xmlSource.current()!=';')
        {
            throw SyntaxError(xmlSource, "Invalidly formed  character reference or entity.");
        }
        entityReference.unparsed += ';';
        xmlSource.next();
        if (entityReference.unparsed[1] == '#')
        {
            entityReference.parsed = xmlParseCharacterReference(xmlSource, entityReference.unparsed.substr(2, entityReference.unparsed.size() - 3));
        }
        else if (m_entityMapping.count(entityReference.unparsed) > 0)
        {
            entityReference.parsed = m_entityMapping[entityReference.unparsed].internal;
        }
        return (entityReference);
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
                    character.unparsed.starts_with("&#") ||
                    character.unparsed.starts_with("&x"))
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
        xNodeElement->elements.emplace_back(std::make_unique<XNodeComment>(xNodeComment));
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
        xNodeElement->elements.emplace_back(std::make_unique<XNodePI>(xNodePI));
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
        if (!xNodeElement->elements.empty())
        {
            if (xNodeElement->elements.back()->getNodeType() == XNodeType::content)
            {
                XNodeRef<XNodeContent>(*xNodeElement->elements.back()).isWhiteSpace = false;
            }
        }
        xNodeElement->elements.emplace_back(std::make_unique<XNodeCDATA>(xNodeCDATA));
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
            if (!isAttributePresent(xNodeElement->attributes, attributeName))
            {
                xNodeElement->attributes.emplace_back(attributeName, attributeValue);
            }
            else
            {
                throw SyntaxError(xmlSource, "Attribute defined more than once within start tag.");
            }
        }
        for (auto attribute : xNodeElement->attributes)
        {
            if (attribute.name.starts_with("xmlns"))
            {
                attribute.name = (attribute.name.size() > 5) ? attribute.name.substr(6) : ":";
                xNodeElement->namespaces.emplace_back(attribute);
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
        xNodeChildElement.namespaces = xNodeElement->namespaces;
        xmlParseElement(xmlSource, &xNodeChildElement);
        if (auto pos = xNodeChildElement.name.find(':'); pos != std::string::npos)
        {
            if (!isAttributePresent(xNodeChildElement.namespaces,
                                    xNodeChildElement.name.substr(0, pos)))
            {
                throw SyntaxError(xmlSource, "Namespace used but not defined.");
            }
        }
        xNodeElement->elements.push_back(std::make_unique<XNodeElement>(std::move(xNodeChildElement)));
    }
    /// <summary>
    /// Parse any element content that is found.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::xmlParseDefault(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XValue entityReference = xmlParseCharacter(xmlSource);
        if (entityReference.unparsed.starts_with("&") && entityReference.unparsed.ends_with(";"))
        {
            XNodeEntityReference xNodeEntityReference(entityReference);
            if ((entityReference.parsed != "&") &&
                (entityReference.parsed != "\'") &&
                (entityReference.parsed != "\"") &&
                (entityReference.parsed != ">") &&
                (entityReference.parsed != "<"))
            {
                XNodeElement entityElement;
                BufferSource entitySource(entityReference.parsed);
                while (entitySource.more())
                {
                    xmlParseElementContents(entitySource, &entityElement);
                }
                xNodeEntityReference.elements = std::move(entityElement.elements);

            }
            if (!xNodeElement->elements.empty())
            {
                if (xNodeElement->elements.back()->getNodeType() == XNodeType::content)
                {
                    XNodeRef<XNodeContent>(*xNodeElement->elements.back()).isWhiteSpace = false;
                }
            }
            xNodeElement->elements.emplace_back(std::make_unique<XNodeEntityReference>(std::move(xNodeEntityReference)));
        }
        else
        {
            xmlParseAddElementContent(xNodeElement, entityReference.parsed);
        }
    }
    /// <summary>
    /// Parse element content area generating any XNodes and adding them to the list
    /// of the current XNodeElement.
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
    /// Parse XML prolog and create the necessary XNodeElements for it.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::xmlParseProlog(ISource &xmlSource, XNodeElement *xNodeProlog)
    {
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
    /// Start parsing XML source stream.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    XMLObject XML::xmlParse(ISource &xmlSource)
    {
        XMLObject xObject;
        xmlParseProlog(xmlSource, &xObject.prolog);
        if (xmlSource.match(U"<"))
        {
            xObject.prolog.elements.emplace_back(std::make_unique<XNodeElement>(XNodeElement(XNodeType::root)));
            xmlParseElement(xmlSource, static_cast<XNodeElement *>(xObject.prolog.elements.back().get()));
            while (xmlSource.more())
            {
                if (xmlSource.match(U"<!--"))
                {
                    xmlParseComment(xmlSource, (&xObject.prolog));
                }
                else if (std::iswspace(xmlSource.current()))
                {
                    xmlParseAddElementContent(&xObject.prolog, std::string(1, xmlSource.current()));
                    xmlSource.next();
                }
                else
                {
                    throw SyntaxError(xmlSource, "Extra content at the end of document.");
                }
            }
        }
        else
        {
            throw SyntaxError(xmlSource, "Missing root element.");
        }
        return (xObject);
    }
} // namespace H4
