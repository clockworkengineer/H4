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
    // ========================;
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
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
    long XML::parseCharacterReference(ISource &source, XString reference)
    {
        char *end;
        long temp = 10;
        if (reference[0] == 'x')
        {
            reference = reference.substr(1);
            temp = 16;
        }
        temp = std::strtol(XML::m_UTF8.to_bytes(reference).c_str(), &end, temp);
        if (*end == '\0')
        {
            return (temp);
        }
        throw SyntaxError(source, "Cannot convert character reference.");
    }
    XValue XML::parseReferenceOrEntity(ISource &source)
    {
        XValue entityReference;
        XString unparsedEntityReference = U"&";
        XString parsedEntityReference;
        source.next();
        while (source.more() && source.current() != ';')
        {
            unparsedEntityReference += source.current();
            source.next();
        }
        unparsedEntityReference += ';';
        source.next();
        if (unparsedEntityReference[1] == '#')
        {
            parsedEntityReference = XString(1, parseCharacterReference(source, unparsedEntityReference.substr(2, unparsedEntityReference.size()-3)));
        }
        else if (m_entityMapping.count(unparsedEntityReference) > 0)
        {
            parsedEntityReference = m_entityMapping[unparsedEntityReference];
        }
        else
        {
            throw SyntaxError(source, "Invalidly formed  character reference or entity.");
        }
        for (auto ch : parsedEntityReference)
        {
            if (!validChar(ch))
            {
                throw SyntaxError(source, "Invalid character value encountered.");
            }
        }
        entityReference.unparsed = m_UTF8.to_bytes(unparsedEntityReference);
        entityReference.parsed = m_UTF8.to_bytes(parsedEntityReference);
        return (entityReference);
    }
    XValue XML::parseCharacter(ISource &source)
    {
        XValue character;
        if (source.current() == '&')
        {

            character = parseReferenceOrEntity(source);
        }
        else if (validChar(source.current()))
        {
            character.unparsed = "";
            character.parsed = m_UTF8.to_bytes(source.current());
            source.next();
        }
        else
        {
            throw SyntaxError(source, "Invalid character value encountered.");
        }
        return (character);
    }
    XValue XML::parseValue(ISource &source)
    {
        if ((source.current() == '\'') || ((source.current() == '"')))
        {
            XValue value;
            XChar quote = source.current();
            source.next();
            while (source.more() && source.current() != quote)
            {
                XValue character = parseCharacter(source);
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
            source.next();
            source.ignoreWS();
            return (value);
        }
        throw SyntaxError(source, "Invalid attribute value.");
    }
    std::string XML::parseName(ISource &source)
    {
        XString name;
        while (source.more() && validNameChar(source.current()))
        {
            name += source.current();
            source.next();
        }
        source.ignoreWS();
        if (!validateName(name))
        {
            throw SyntaxError(source, "Invalid name encountered.");
        }
        return (XML::m_UTF8.to_bytes(name));
    }
    void XML::parseTagName(ISource &source, XNodeElement *xNodeElement)
    {
        xNodeElement->name = parseName(source);
    }
    void XML::parseComment(ISource &source, XNodeElement *xNodeElement)
    {
        XNodeComment xNodeComment;
        while (source.more() && !source.match(U"--"))
        {
            xNodeComment.comment += XML::m_UTF8.to_bytes(source.current());
            source.next();
        }
        if (source.current() != '>')
        {
            throw SyntaxError(source, "Missing closing '>' for comment line.");
        }
        source.next();
        xNodeElement->elements.emplace_back(std::make_unique<XNodeComment>(xNodeComment));
    }
    void XML::parsePI(ISource &source, XNodeElement *xNodeElement)
    {
        XNodePI xNodePI;
        xNodePI.name = parseName(source);
        while (source.more() && !source.match(U"?>"))
        {
            xNodePI.parameters += XML::m_UTF8.to_bytes(source.current());
            source.next();
        }
        xNodeElement->elements.emplace_back(std::make_unique<XNodePI>(xNodePI));
    }
    void XML::parseCDATA(ISource &source, XNodeElement *xNodeElement)
    {
        XNodeCDATA xNodeCDATA;
        while (source.more() && !source.match(U"]]>"))
        {
            if (source.match(U"<![CDATA["))
            {
                throw SyntaxError(source);
            }
            xNodeCDATA.cdata += XML::m_UTF8.to_bytes(source.current());
            source.next();
        }
        xNodeElement->elements.emplace_back(std::make_unique<XNodeCDATA>(xNodeCDATA));
    }
    void XML::parseAttributes(ISource &source, XNodeElement *xNodeElement)
    {
        while (source.current() != '?' &&
               source.current() != '/' &&
               source.current() != '>')
        {
            std::string attributeName = parseName(source);
            if (source.current() != '=')
            {
                throw SyntaxError(source, "Missing '=' between attribute name and value.");
            }
            source.next();
            source.ignoreWS();
            XValue attributeValue = parseValue(source);
            if (!isAttributePresent(xNodeElement->attributes, attributeName))
            {
                xNodeElement->attributes.emplace_back(attributeName, attributeValue.parsed, attributeValue.unparsed);
            }
            else
            {
                throw SyntaxError(source, "Attribute defined more than once within start tag.");
            }
        }
        addNamespacesToList(xNodeElement);
    }
    void XML::parseProlog(ISource &source, XNodeElement *xNodeProlog)
    {
        source.ignoreWS();
        if (source.match(U"<?xml"))
        {
            source.ignoreWS();
            parseAttributes(source, xNodeProlog);
            if (!source.match(U"?>") || !validateXMLDeclaration(xNodeProlog))
            {
                throw SyntaxError(source, "Declaration invalid or end tag not found.");
            }
        }
        else
        {
            for (auto attr : XML::m_defaultAtributes)
            {
                xNodeProlog->attributes.emplace_back(attr.name, attr.value);
            }
        }
        while (source.more() && std::iswspace(source.current()))
        {
            if (source.current() == 0x0A)
            {
                break;
            }
            source.next();
        }
        while (source.more())
        {
            if (source.match(U"<!--"))
            {
                parseComment(source, xNodeProlog);
            }
            else if (source.match(U"<?"))
            {
                parsePI(source, xNodeProlog);
            }
            else if (source.match(U"<!DOCTYPE"))
            {
                parseDTD(source, xNodeProlog);
            }
            else if (source.current() == 0x0A)
            {
                XNodeContent linefeed;
                linefeed.content += 0x0A;
                xNodeProlog->elements.emplace_back(std::make_unique<XNodeContent>(std::move(linefeed)));
                source.next();
            }
            else
            {
                break;
            }
            while (source.more() && std::iswspace(source.current()))
            {
                if (source.current() == 0x0A)
                {
                    break;
                }
                source.next();
            }
        }
    }
    void XML::parseChildElement(ISource &source, XNodeElement *xNodeElement)
    {
        XNodeElement xNodeChildElement;
        xNodeChildElement.namespaces = xNodeElement->namespaces;
        parseElement(source, &xNodeChildElement);
        if (auto pos = xNodeChildElement.name.find(':'); pos != std::string::npos)
        {
            if (!isAttributePresent(xNodeChildElement.namespaces,
                                    xNodeChildElement.name.substr(0, pos)))
            {
                throw SyntaxError(source, "Namespace used but not defined.");
            }
        }
        xNodeElement->elements.push_back(std::make_unique<XNodeElement>(std::move(xNodeChildElement)));
    }
    void XML::parseDefault(ISource &source, XNodeElement *xNodeElement)
    {
        if (source.match(U"]]>"))
        {
            throw SyntaxError(source, "']]>' invalid in element content area.");
        }
        XValue entityReference = parseCharacter(source);
        if (entityReference.unparsed != "")
        {
            addCurrentXNodeContent(xNodeElement);
            XNodeEntityReference xNodeEntityReference;
            xNodeEntityReference.unparsed = entityReference.unparsed;
            xNodeEntityReference.parsed = entityReference.parsed;
            xNodeElement->elements.emplace_back(std::make_unique<XNodeEntityReference>(std::move(xNodeEntityReference)));
        }
        else
        {
            xNodeElement->content += entityReference.parsed;
        }
    }
    void XML::parseElementContents(ISource &source, XNodeElement *xNodeElement)
    {
        if (source.match(U"<!--"))
        {
            addCurrentXNodeContent(xNodeElement);
            parseComment(source, xNodeElement);
        }
        else if (source.match(U"<?"))
        {
            addCurrentXNodeContent(xNodeElement);
            parsePI(source, xNodeElement);
        }
        else if (source.match(U"<![CDATA["))
        {
            addCurrentXNodeContent(xNodeElement);
            parseCDATA(source, xNodeElement);
        }
        else if (source.match(U"<"))
        {
            addCurrentXNodeContent(xNodeElement);
            parseChildElement(source, xNodeElement);
        }
        else
        {
            parseDefault(source, xNodeElement);
        }
    }
    void XML::parseElement(ISource &source, XNodeElement *xNodeElement)
    {
        parseTagName(source, xNodeElement);
        parseAttributes(source, xNodeElement);
        if (source.match(U">"))
        {
            XString closingTag = U"</" + XML::m_UTF8.from_bytes(xNodeElement->name) + U">";
            while (source.more() && !source.match(closingTag))
            {
                parseElementContents(source, xNodeElement);
            }
            addCurrentXNodeContent(xNodeElement);
        }
        else if (source.match(U"/>"))
        {
            xNodeElement->setNodeType(XNodeType::self);
        }
        else
        {
            throw SyntaxError(source, "Missing '/>' for closing tag.");
        }
    }
    std::unique_ptr<XNode> XML::parseXML(ISource &source)
    {
        XNodeElement xNodeProlog(XNodeType::prolog);
        parseProlog(source, &xNodeProlog);
        if (source.current() == '<')
        {
            source.next();
            xNodeProlog.elements.emplace_back(std::make_unique<XNodeElement>(XNodeElement(XNodeType::root)));
            parseElement(source, static_cast<XNodeElement *>(xNodeProlog.elements.back().get()));
            if (source.current() == 0x0A)
            {
                XNodeContent linefeed;
                linefeed.content += 0x0A;
                xNodeProlog.elements.emplace_back(std::make_unique<XNodeContent>(std::move(linefeed)));
            }
        }
        else
        {
            throw SyntaxError(source, "Missing declaration or root element.");
        }
        return (std::make_unique<XNodeElement>(std::move(xNodeProlog)));
    }
} // namespace H4
