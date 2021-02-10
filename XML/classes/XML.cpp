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
    void XML::initialiseTables()
    {
        m_entityToCharacter[U"amp"] = U"&";
        m_entityToCharacter[U"quot"] = U"\"";
        m_entityToCharacter[U"apos"] = U"\'";
        m_entityToCharacter[U"lt"] = U"<";
        m_entityToCharacter[U"gt"] = U">";
    }
    bool XML::namePresentInAttributeList(std::vector<XAttribute> attributes, const XString &name)
    {
        return (std::find_if(attributes.rbegin(), attributes.rend(),
                             [&name, this](const XAttribute &attr) { return (attr.name == m_toFromUTF8.to_bytes(name)); }) != attributes.rend());
    }
    long XML::calculatecharacterReference(XString reference)
    {
        char *end;
        long temp = 10;
        if (reference[0] == 'x')
        {
            reference = reference.substr(1);
            temp = 16;
        }
        temp = std::strtol(m_toFromUTF8.to_bytes(reference).c_str(), &end, temp);
        if (*end == '\0')
        {
            return (temp);
        }
        throw XML::SyntaxError();
    }
    std::string XML::convertCRLFToLF(const std::string &xmlString)
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
    void XML::addNameValuePairToList(std::vector<XAttribute> &attributes, const XString &attributeName, const XString &attributeValue)
    {
        if (!namePresentInAttributeList(attributes, attributeName))
        {
            attributes.emplace_back(m_toFromUTF8.to_bytes(attributeName), m_toFromUTF8.to_bytes(attributeValue));
        }
        else
        {
            throw XML::SyntaxError();
        }
    }
    bool XML::validChar(XChar ch)
    {
        return ((ch == 0x09) ||
                (ch == 0x0A) ||
                (ch == 0x0D) ||
                (ch >= 0x20 && ch <= 0xD7FF) ||
                (ch >= 0xE000 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0x10FFFF));
    }
    bool XML::validNameStartChar(XChar ch)
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
    bool XML::validNameChar(XChar ch)
    {
        return (validNameStartChar(ch) ||
                (ch == '-') ||
                (ch == '.') ||
                (ch >= '0' && ch <= '9') ||
                (ch == 0xB7) ||
                (ch >= 0x0300 && ch <= 0x036F) ||
                (ch >= 0x203F && ch <= 0x2040));
    }
    bool XML::validReservedName(const XString &name)
    {
        return (name.starts_with(U"xmlns"));
    }
    bool XML::validateName(XString name)
    {
        if (name.empty())
        {
            return (false);
        }
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.starts_with(U"xml") && !(validReservedName(name)))
        {
            return (false);
        }
        if (!validNameStartChar(name[0]))
        {
            return (false);
        }
        for (auto it = name.begin() + 1; it != name.end(); it++)
        {
            if (!validNameChar(*it))
            {
                return (false);
            }
        }
        return (true);
    }
    bool XML::validateXMLDeclaration(XNodeElement *xNodeElement)
    {
        // Syntax error if no version present
        if (!namePresentInAttributeList(xNodeElement->attributes, U"version"))
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
    XString XML::parseName(XML::ISource &source)
    {
        XString name;
        while (source.more() && validNameChar(source.current()))
        {
            name += source.current();
            source.next();
        }
        source.ignoreWS();
        return (name);
    }
    void XML::parseTagName(ISource &source, XNodeElement *xNodeElement)
    {
        XString tagName = parseName(source);
        if (!validateName(tagName))
        {
            throw XML::SyntaxError();
        }
        xNodeElement->name = m_toFromUTF8.to_bytes(tagName);
    }
    XString XML::parseEncodedCharacter(ISource &source)
    {
        XString characters;
        if (source.current() == '&')
        {
            source.next();
            characters = parseReferenceOrEntity(source);
        }
        else
        {
            characters = source.current();
            source.next();
        }
        for (auto ch : characters)
        {
            if (!validChar(ch))
            {
                throw XML::SyntaxError();
            }
        }
        return (characters);
    }
    XString XML::parseAttributeValue(ISource &source)
    {
        if ((source.current() == '\'') || ((source.current() == '"')))
        {
            XString attributeValue;
            XChar quote = source.current();
            source.next();
            while (source.more() && source.current() != quote)
            {
                attributeValue += parseEncodedCharacter(source);
            }
            source.next();
            source.ignoreWS();
            return (attributeValue);
        }
        throw XML::SyntaxError();
    }
    XString XML::parseAttributeName(ISource &source)
    {
        XString attributeName = parseName(source);
        if (!validateName(attributeName))
        {
            throw XML::SyntaxError();
        }
        return (attributeName);
    }
    XString XML::parseReferenceOrEntity(ISource &source)
    {
        XString entityName;
        while (source.current() && source.current() != ';')
        {
            entityName += source.current();
            source.next();
        }
        source.next();
        if (entityName[0] == '#')
        {
            return (XString(1, calculatecharacterReference(entityName.substr(1))));
        }
        else if (m_entityToCharacter.count(entityName) > 0)
        {
            return (m_entityToCharacter[entityName]);
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
        xNodePI.name = m_toFromUTF8.to_bytes(parseName(source));
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
        std::vector<XAttribute> namespaces;
        while (source.current() != '?' &&
               source.current() != '/' &&
               source.current() != '>')
        {
            XString attributeName = parseAttributeName(source);
            if (source.current() != '=')
            {
                throw XML::SyntaxError();
            }
            source.next();
            source.ignoreWS();
            XString attributeValue = parseAttributeValue(source);
            if (attributeName.starts_with(U"xmlns"))
            {
                attributeName = (attributeName.size() > 5) ? attributeName.substr(6) : U":";
                addNameValuePairToList(namespaces, attributeName, attributeValue);
                xNodeElement->namespaces.emplace_back(namespaces.back());
            }
            else
            {
                addNameValuePairToList(xNodeElement->attributes, attributeName, attributeValue);
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
            if (!source.match(U"?>") || !validateXMLDeclaration(xNodeElement))
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
            else if (source.match(U"<!DOCTYPE"))
            {
                parseDTD(source, xNodeElement);
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
        XNodeElement xNodeChildElement;
        xNodeChildElement.namespaces = xNodeElement->namespaces;
        parseElement(source, &xNodeChildElement);
        if (auto pos = xNodeChildElement.name.find(':') != std::string::npos)
        {
            if (!namePresentInAttributeList(xNodeChildElement.namespaces, m_toFromUTF8.from_bytes(xNodeChildElement.name.substr(0, pos))))
            {
                throw XML::SyntaxError();
            }
        }
        xNodeElement->elements.push_back(std::make_unique<XNodeElement>(std::move(xNodeChildElement)));
    }
    void XML::parseDefault(ISource &source, XNodeElement *xNodeElement)
    {
        if (source.match(U"]]>"))
        {
            throw XML::SyntaxError();
        }
        xNodeElement->contents += m_toFromUTF8.to_bytes(parseEncodedCharacter(source));
    }
    void XML::parseElementContents(ISource &source, XNodeElement *xNodeElement)
    {
        if (source.match(U"<!--"))
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
        else if (source.match(U"<"))
        {
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
        if (source.match(U"/>") || source.match(U">"))
        {
            XString closingTag = U"</" + m_toFromUTF8.from_bytes(xNodeElement->name) + U">";
            while (source.more() && !source.match(closingTag))
            {
                parseElementContents(source, xNodeElement);
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
        if (source.current() == '<')
        {
            source.next();
            xNodeRoot.elements.emplace_back(std::make_unique<XNodeElement>());
            parseElement(source, static_cast<XNodeElement *>(xNodeRoot.elements.back().get()));
        }
        else
        {
            throw XML::SyntaxError();
        }
        return (std::make_unique<XNodeElement>(std::move(xNodeRoot)));
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    std::unique_ptr<XNode> XML::parse(const std::string &xmlToParse)
    {
        BufferSource xml(m_toFromUTF8.from_bytes(convertCRLFToLF(xmlToParse)));
        return (parseXML(xml));
    }
    std::unique_ptr<XNode> XML::parse(const std::u16string &xmlToParse)
    {
        std::u16string utf16xml{xmlToParse};
        if (!xmlToParse.starts_with(u"<?xml"))
        {
            for (char16_t &ch : utf16xml)
            {
                ch = (static_cast<u_int16_t>(ch) >> 8) | (static_cast<u_int16_t>(ch) << 8);
            }
        }
        return (parse(m_toFromUTF16.to_bytes(utf16xml)));
    }
} // namespace H4
