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
    static XAttribute defaultAtributes[3] = {{"version", "1.0"}, {"encoding", "UTF-8"}, {"standalone", "no"}};
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    std::string XML::convertCRLF(const std::string &xmlString)
    {
        std::string converted = xmlString;
        size_t pos = converted.find("\x0D\x0A");
        while (pos != std::string::npos)
        {
            converted.replace(pos, 2, "\x0A");
            pos = converted.find("\x0D\x0A", pos + 1);
        }
        return (converted);
    }
    inline bool XML::validChar(XChar ch)
    {
        return ((ch == 0x09) ||
                (ch == 0x0A) ||
                (ch == 0x0D) ||
                (ch >= 0x20 && ch <= 0xD7FF) ||
                (ch >= 0xE000 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0x10FFFF));
    }
    inline bool XML::validNameStartChar(XChar ch)
    {
        return ((ch == ':') ||
                (ch == '_') ||
                (ch >= 'A' && ch <= 'Z') ||
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 0xC0 && ch <= 0xD6) ||
                (ch >= 0xD8 && ch <= 0xF6) ||
                (ch >= 0xF8 && ch <= 0x2FF) ||
                (ch >= 0x370 && ch <= 0x37D) ||
                (ch >= 0x37F && ch <= 0x1FFF) ||
                (ch >= 0x200C && ch <= 0x200D) ||
                (ch >= 0x2070 && ch <= 0x218F) ||
                (ch >= 0x2C00 && ch <= 0x2FEF) ||
                (ch >= 0x3001 && ch <= 0xD7FF) ||
                (ch >= 0xF900 && ch <= 0xFDCF) ||
                (ch >= 0xFDF0 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0xEFFFF));
    }
    inline bool XML::validNameChar(XChar ch)
    {
        return (validNameStartChar(ch) ||
                (ch == '-') ||
                (ch == '.') ||
                (ch >= '0' && ch <= '9') ||
                (ch == 0xB7) ||
                (ch >= 0x0300 && ch <= 0x036F) ||
                (ch >= 0x203F && ch <= 0x2040));
    }
    inline bool XML::attributePresent(std::vector<XAttribute> attributes, const XString &name)
    {
        return (std::find_if(attributes.begin(), attributes.end(),
                             [&name, this](const XAttribute &attr) { return (attr.name == m_toFromUTF8.to_bytes(name)); }) != attributes.end());
    }
    inline bool XML::validateAttributeName(XString attributeName)
    {
        if (!validNameStartChar(attributeName[0]))
        {
            return (false);
        }
        for (auto it = attributeName.begin() + 1; it != attributeName.end(); it++)
        {
            if (!validNameChar(*it))
            {
                return (false);
            }
        }
        return (true);
    }
    inline bool XML::validateTagName(XString tagName)
    {
        std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
        if (tagName.substr(0, 3) == U"xml")
        {
            return (false);
        }
        return (validateAttributeName(tagName));
    }
    bool XML::validateDeclaration(XNodeElement *xNodeElement)
    {
        // Syntax error if no version present
        if (!attributePresent(xNodeElement->attributes, U"version"))
        {
            return (false);
        }
        // Fill in gaps with default if missing attributes
        std::vector<XAttribute> validatedAttributes;
        long currentAttribute = 0;
        for (auto attrIndex = 0; attrIndex < 3; attrIndex++)
        {
            if ((currentAttribute < (int)xNodeElement->attributes.size()) &&
                (xNodeElement->attributes[currentAttribute].name == defaultAtributes[attrIndex].name))
            {
                validatedAttributes.push_back(xNodeElement->attributes[currentAttribute]);
                currentAttribute++;
            }
            else
            {
                validatedAttributes.push_back(defaultAtributes[attrIndex]);
            }
        }
        // Order not version, encoding, standalone == syntax error
        if (currentAttribute != (long)xNodeElement->attributes.size())
        {
            return (false);
        }
        // Encoding all upper case
        std::transform(validatedAttributes[1].value.begin(), validatedAttributes[1].value.end(),
                       validatedAttributes[1].value.begin(), [](unsigned int c) { return std::toupper(c); });
        // Check valid declaration values
        if ((validatedAttributes[0].value != "1.0") ||
            ((validatedAttributes[1].value != "UTF-8") && (validatedAttributes[1].value != "UTF-16")) ||
            ((validatedAttributes[2].value) != "yes" && (validatedAttributes[2].value != "no")))
        {
            return (false);
        }
        xNodeElement->attributes = validatedAttributes;
        return (true);
    }
    void XML::parseTagName(ISource &source, XNodeElement *xNodeElement)
    {
        XString tagName;
        source.next();
        source.ignoreWS();
        while (source.more() &&
               source.current() != '/' &&
               source.current() != '>' &&
               !std::iswspace(source.current()))
        {
            tagName += source.current();
            source.next();
        }
        if (!validateTagName(tagName))
        {
            throw XML::SyntaxError();
        }
        source.ignoreWS();
        xNodeElement->name = m_toFromUTF8.to_bytes(tagName);
    }
    XChar XML::parseCharacter(ISource &source)
    {
        XChar ch;
        if (source.current() == '&')
        {
            ch = parseCharacterEntities(source);
        }
        else
        {
            ch = source.current();
            source.next();
        }
        return (ch);
    }
    XString XML::parseAttributeValue(ISource &source)
    {
        source.next();
        source.ignoreWS();
        if ((source.current() == '\'') || ((source.current() == '"')))
        {
            XString attributeValue;
            XChar quote = source.current();
            source.next();
            while (source.more() &&
                   source.current() != quote)
            {
                attributeValue += parseCharacter(source);
            }
            source.next();
            source.ignoreWS();
            return (attributeValue);
        }
        throw XML::SyntaxError();
    }
    XString XML::parseAttributeName(ISource &source)
    {
        XString attributeName;
        while (source.more() &&
               source.current() != '=' &&
               !std::iswspace(source.current()))
        {
            attributeName += source.current();
            source.next();
        }
        if (!validateAttributeName(attributeName))
        {
            throw XML::SyntaxError();
        }
        source.ignoreWS();
        return (attributeName);
    }
    XChar XML::parseCharacterEntities(ISource &source)
    {
        XString entityName;
        source.next();
        while (source.current() && source.current() != ';')
        {
            entityName += source.current();
            source.next();
        }
        source.next();
        if (entityName == U"amp")
        {
            return ('&');
        }
        else if (entityName == U"quot")
        {
            return ('"');
        }
        else if (entityName == U"apos")
        {
            return ('\'');
        }
        else if (entityName == U"lt")
        {
            return ('<');
        }
        else if (entityName == U"gt")
        {
            return ('>');
        }
        else if (entityName[0] == 'x')
        {
            char *end;
            std::string value = m_toFromUTF8.to_bytes(entityName.substr(1));
            int ref = std::strtol(value.c_str(), &end, 16);
            if (*end == '\0')
            {
                return (ref);
            }
        }
        else if (entityName[0] >= '0' && entityName[0] <= '9')
        {
            char *end;
            std::string value = m_toFromUTF8.to_bytes(entityName);
            int ref = std::strtol(value.c_str(), &end, 10);
            if (*end == '\0')
            {
                return (ref);
            }
        }
        throw XML::SyntaxError();
    }
    void XML::parseComment(ISource &source, XNodeElement *xNodeElement)
    {
        XNodeComment xNodeComment;
        while (source.more() && !source.match(U"--"))
        {
            xNodeComment.comment += m_toFromUTF8.to_bytes(source.current());
            source.next();
        }
        if (source.current() != '>')
        {
            throw SyntaxError();
        }
        source.next();
        xNodeElement->elements.emplace_back(std::make_unique<XNodeComment>(xNodeComment));
    }
    void XML::parsePI(ISource &source, XNodeElement *xNodeElement)
    {
        XNodePI xNodePI;
        while (source.more() && !std::iswspace(source.current()))
        {
            xNodePI.name += m_toFromUTF8.to_bytes(source.current());
            source.next();
        }
        source.ignoreWS();
        while (source.more() && !source.match(U"?>"))
        {
            xNodePI.parameters += m_toFromUTF8.to_bytes(source.current());
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
                throw XML::SyntaxError();
            }
            xNodeElement->contents += m_toFromUTF8.to_bytes(source.current());
            xNodeCDATA.cdata += m_toFromUTF8.to_bytes(source.current());
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
            XString attributeName = parseAttributeName(source);
            if (source.current() != '=')
            {
                throw XML::SyntaxError();
            }
            XString attributeValue = parseAttributeValue(source);
            if (!attributePresent(xNodeElement->attributes, attributeName))
            {
                xNodeElement->attributes.emplace_back(m_toFromUTF8.to_bytes(attributeName), m_toFromUTF8.to_bytes(attributeValue));
            }
            else
            {
                throw XML::SyntaxError();
            }
        }
    }
    void XML::parseProlog(ISource &source, XNodeElement *xNodeElement)
    {
        source.ignoreWS();
        if (source.match(U"<?xml"))
        {
            source.ignoreWS();
            parseAttributes(source, xNodeElement);
            if (!source.match(U"?>") || !validateDeclaration(xNodeElement))
            {
                throw XML::SyntaxError();
            }
        }
        source.ignoreWS();
        while (source.more())
        {
            if (source.match(U"<!--"))
            {
                parseComment(source, xNodeElement);
            }
            else if (source.match(U"<?"))
            {
                parsePI(source, xNodeElement);
            }
            else
            {
                break;
            }
            source.ignoreWS();
        }
    }
    void XML::parseChildElement(ISource &source, XNodeElement *xNodeElement)
    {
        std::unique_ptr<XNode> xNodeChildElement = std::make_unique<XNodeElement>();
        parseElement(source, static_cast<XNodeElement *>(xNodeChildElement.get()));
        xNodeElement->elements.push_back(std::move(xNodeChildElement));
    }
    void XML::parseContents(ISource &source, XNodeElement *xNodeElement)
    {
        if (source.match(U"]]>"))
        {
            throw XML::SyntaxError();
        }
        else if (source.match(U"<!--"))
        {
            parseComment(source, xNodeElement);
        }
        else if (source.match(U"<?"))
        {
            parsePI(source, xNodeElement);
        }
        else if (source.match(U"<![CDATA["))
        {
            parseCDATA(source, xNodeElement);
        }
        else if (source.current() == '<')
        {
            parseChildElement(source, xNodeElement);
        }
        else
        {
            xNodeElement->contents += m_toFromUTF8.to_bytes(parseCharacter(source));
        }
    }
    void XML::parseElement(ISource &source, XNodeElement *xNodeElement)
    {
        parseTagName(source, xNodeElement);
        parseAttributes(source, xNodeElement);
        XString closingTag = U"</" + m_toFromUTF8.from_bytes(xNodeElement->name) + U">";
        if (source.match(U"/>") || source.match(U">"))
        {
            while (source.more() && !source.match(closingTag))
            {
                parseContents(source, xNodeElement);
            }
        }
        else
        {
            throw XML::SyntaxError();
        }
    }
    std::unique_ptr<XNode> XML::parseXML(ISource &source)
    {
        XNodeElement xNodeRoot;
        parseProlog(source, &xNodeRoot);
        xNodeRoot.elements.emplace_back(std::make_unique<XNodeElement>());
        parseElement(source, static_cast<XNodeElement *>(xNodeRoot.elements.back().get()));
        return (std::make_unique<XNodeElement>(std::move(xNodeRoot)));
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    std::unique_ptr<XNode> XML::parse(const std::string &xmlToParse)
    {
        BufferSource xml(m_toFromUTF8.from_bytes(convertCRLF(xmlToParse)));
        return (parseXML(xml));
    }
} // namespace H4
