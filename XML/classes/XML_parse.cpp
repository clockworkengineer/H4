//
// Class: XML
//
// Description: XML parser code.
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
    /// Check whether a attribute exists in list of attributes.
    /// </summary>
    /// <param name="attributeList">Vector of attribute name/value pairs.</param>
    /// <param name="attributeName">Attribute name to check for in list.</param>
    /// <returns>true if attribute name is in list.</returns>
    bool XML::isAttributePresent(std::vector<XAttribute> attributeList, const std::string &attributeName)
    {
        return (std::find_if(attributeList.rbegin(), attributeList.rend(),
                             [&attributeName](const XAttribute &attr) { return (attr.name == attributeName); }) != attributeList.rend());
    }
    /// <summary>
    /// Find first non-whitespace character or linfeed on source stream
    /// </summary>
    /// <param name="xmlSource">Source stream.</param>
    /// <returns></returns>
    void XML::moveToNextLineFeed(ISource &xmlSource)
    {
        while (xmlSource.more() && std::iswspace(xmlSource.current()))
        {
            if (xmlSource.current() == kLineFeed)
            {
                break;
            }
            xmlSource.next();
        }
    }
    /// <summary>
    /// Create content XNode and add to XNode elements from current element content
    /// buffer before it is cleared ready to recieve more content or a different
    //  type of XNode. This keeps XML order for stringification by using sequential 
    /// list of XNodes to reconstruct elements.
    /// </summary>
    /// <param name="xNodeElement">Current XML element.</param>
    /// <returns></returns>
    void XML::addCurrentXNodeContent(XNodeElement *xNodeElement)
    {
        if (!xNodeElement->content.empty())
        {
            XNodeContent xNodeContent;
            xNodeContent.content = xNodeElement->content;
            xNodeElement->elements.emplace_back(std::make_unique<XNodeContent>(std::move(xNodeContent)));
            xNodeElement->content.clear();
        }
    }
    /// <summary>
    /// Parse a character reference returning its value.
    /// </summary>
    /// <param name="xmlSource">Source stream.</param>
    /// <returns>Character reference value.</returns>
    long XML::parseCharacterReference(ISource &xmlSource, XString reference)
    {
        char *end;
        long result = 10;
        if (reference[0] == 'x')
        {
            reference = reference.substr(1);
            result = 16;
        }
        result = std::strtol(xmlSource.to_bytes(reference).c_str(), &end, result);
        if (*end == '\0')
        {
            return (result);
        }
        throw SyntaxError(xmlSource, "Cannot convert character reference.");
    }
    /// <summary>
    /// Parse XML string value representing a character reference or entity.
    /// </summary>
    /// <param name="xmlSource">Source stream.</param>
    /// <returns>Value for reference or enity.</returns>
    XValue XML::parseReferenceOrEntity(ISource &xmlSource)
    {
        XValue entityReference;
        XString unparsedEntityReference = U"&";
        XString parsedEntityReference;
        xmlSource.next();
        while (xmlSource.more() && xmlSource.current() != ';')
        {
            unparsedEntityReference += xmlSource.current();
            xmlSource.next();
        }
        unparsedEntityReference += ';';
        xmlSource.next();
        if (unparsedEntityReference[1] == '#')
        {
            parsedEntityReference = XString(1, parseCharacterReference(xmlSource, unparsedEntityReference.substr(2, unparsedEntityReference.size() - 3)));
        }
        else if (m_entityMapping.count(unparsedEntityReference) > 0)
        {
            parsedEntityReference = m_entityMapping[unparsedEntityReference];
        }
        else
        {
            throw SyntaxError(xmlSource, "Invalidly formed  character reference or entity.");
        }
        for (auto ch : parsedEntityReference)
        {
            if (!validChar(ch))
            {
                throw SyntaxError(xmlSource, "Invalid character value encountered.");
            }
        }
        entityReference.unparsed = xmlSource.to_bytes(unparsedEntityReference);
        entityReference.parsed = xmlSource.to_bytes(parsedEntityReference);
        return (entityReference);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    XValue XML::parseCharacter(ISource &xmlSource)
    {
        XValue character;
        if (xmlSource.current() == '&')
        {
            character = parseReferenceOrEntity(xmlSource);
        }
        else if (validChar(xmlSource.current()))
        {
            character.parsed = xmlSource.to_bytes(xmlSource.current());
            xmlSource.next();
        }
        else
        {
            throw SyntaxError(xmlSource, "Invalid character value encountered.");
        }
        return (character);
    }
    XValue XML::parseValue(ISource &xmlSource)
    {
        if ((xmlSource.current() == '\'') || ((xmlSource.current() == '"')))
        {
            XValue value;
            XChar quote = xmlSource.current();
            xmlSource.next();
            while (xmlSource.more() && xmlSource.current() != quote)
            {
                XValue character = parseCharacter(xmlSource);
                if (character.unparsed == "")
                {
                    value.unparsed += character.parsed;
                }
                else
                {
                    value.unparsed += character.unparsed;
                }
                value.parsed += character.parsed;
            }
            xmlSource.next();
            xmlSource.ignoreWS();
            return (value);
        }
        throw SyntaxError(xmlSource, "Invalid attribute value.");
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    std::string XML::parseName(ISource &xmlSource)
    {
        XString name;
        while (xmlSource.more() && validNameChar(xmlSource.current()))
        {
            name += xmlSource.current();
            xmlSource.next();
        }
        xmlSource.ignoreWS();
        if (!validateName(name))
        {
            throw SyntaxError(xmlSource, "Invalid name encountered.");
        }
        return (xmlSource.to_bytes(name));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseTagName(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        xNodeElement->name = parseName(xmlSource);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseComment(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodeComment xNodeComment;
        while (xmlSource.more() && !xmlSource.match(U"--"))
        {
            xNodeComment.comment += xmlSource.to_bytes(xmlSource.current());
            xmlSource.next();
        }
        if (xmlSource.current() != '>')
        {
            throw SyntaxError(xmlSource, "Missing closing '>' for comment line.");
        }
        xmlSource.next();
        xNodeElement->elements.emplace_back(std::make_unique<XNodeComment>(xNodeComment));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parsePI(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodePI xNodePI;
        xNodePI.name = parseName(xmlSource);
        while (xmlSource.more() && !xmlSource.match(U"?>"))
        {
            xNodePI.parameters += xmlSource.to_bytes(xmlSource.current());
            xmlSource.next();
        }
        xNodeElement->elements.emplace_back(std::make_unique<XNodePI>(xNodePI));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseCDATA(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodeCDATA xNodeCDATA;
        while (xmlSource.more() && !xmlSource.match(U"]]>"))
        {
            if (xmlSource.match(U"<![CDATA["))
            {
                throw SyntaxError(xmlSource);
            }
            xNodeCDATA.cdata += xmlSource.to_bytes(xmlSource.current());
            xmlSource.next();
        }
        xNodeElement->elements.emplace_back(std::make_unique<XNodeCDATA>(xNodeCDATA));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseAttributes(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        while (xmlSource.current() != '?' &&
               xmlSource.current() != '/' &&
               xmlSource.current() != '>')
        {
            std::string attributeName = parseName(xmlSource);
            if (xmlSource.current() != '=')
            {
                throw SyntaxError(xmlSource, "Missing '=' between attribute name and value.");
            }
            xmlSource.next();
            xmlSource.ignoreWS();
            XValue attributeValue = parseValue(xmlSource);
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
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseProlog(ISource &xmlSource, XNodeElement *xNodeProlog)
    {
        xmlSource.ignoreWS();
        if (xmlSource.match(U"<?xml"))
        {
            xmlSource.ignoreWS();
            parseAttributes(xmlSource, xNodeProlog);
            if (!xmlSource.match(U"?>") || !validateXMLDeclaration(xNodeProlog))
            {
                throw SyntaxError(xmlSource, "Declaration invalid or end tag not found.");
            }
        }
        moveToNextLineFeed(xmlSource);
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
                parseDTD(xmlSource, xNodeProlog);
            }
            else if (xmlSource.current() == kLineFeed)
            {
                XNodeContent linefeed;
                linefeed.content += kLineFeed;
                xNodeProlog->elements.emplace_back(std::make_unique<XNodeContent>(std::move(linefeed)));
                xmlSource.next();
            }
            else
            {
                break;
            }
            moveToNextLineFeed(xmlSource);
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseChildElement(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        XNodeElement xNodeChildElement;
        xNodeChildElement.namespaces = xNodeElement->namespaces;
        parseElement(xmlSource, &xNodeChildElement);
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
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDefault(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        if (xmlSource.match(U"]]>"))
        {
            throw SyntaxError(xmlSource, "']]>' invalid in element content area.");
        }
        XValue entityReference = parseCharacter(xmlSource);
        if (entityReference.unparsed != "")
        {
            addCurrentXNodeContent(xNodeElement);
            XNodeEntityReference xNodeEntityReference(entityReference);
            xNodeElement->elements.emplace_back(std::make_unique<XNodeEntityReference>(std::move(xNodeEntityReference)));
        }
        else
        {
            xNodeElement->content += entityReference.parsed;
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseElementContents(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        if (xmlSource.match(U"<!--"))
        {
            addCurrentXNodeContent(xNodeElement);
            parseComment(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"<?"))
        {
            addCurrentXNodeContent(xNodeElement);
            parsePI(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"<![CDATA["))
        {
            addCurrentXNodeContent(xNodeElement);
            parseCDATA(xmlSource, xNodeElement);
        }
        else if (xmlSource.match(U"<"))
        {
            addCurrentXNodeContent(xNodeElement);
            parseChildElement(xmlSource, xNodeElement);
        }
        else
        {
            parseDefault(xmlSource, xNodeElement);
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseElement(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        parseTagName(xmlSource, xNodeElement);
        parseAttributes(xmlSource, xNodeElement);
        if (xmlSource.match(U">"))
        {
            XString closingTag = U"</" + xmlSource.from_bytes(xNodeElement->name) + U">";
            while (xmlSource.more() && !xmlSource.match(closingTag))
            {
                parseElementContents(xmlSource, xNodeElement);
            }
            addCurrentXNodeContent(xNodeElement);
        }
        else if (xmlSource.match(U"/>"))
        {
            xNodeElement->setNodeType(XNodeType::self);
        }
        else
        {
            throw SyntaxError(xmlSource, "Missing '/>' for closing tag.");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    XMLObject XML::parseXML(ISource &xmlSource)
    {
        XMLObject xObject;
        parseProlog(xmlSource, &xObject.prolog);
        if (xmlSource.current() == '<')
        {
            xmlSource.next();
            xObject.prolog.elements.emplace_back(std::make_unique<XNodeElement>(XNodeElement(XNodeType::root)));
            parseElement(xmlSource, static_cast<XNodeElement *>(xObject.prolog.elements.back().get()));
            if (xmlSource.current() == kLineFeed)
            {
                XNodeContent linefeed;
                linefeed.content += kLineFeed;
                xObject.prolog.elements.emplace_back(std::make_unique<XNodeContent>(std::move(linefeed)));
            }
        }
        else
        {
            throw SyntaxError(xmlSource, "Missing declaration or root element.");
        }
        return (xObject);
    }
} // namespace H4
