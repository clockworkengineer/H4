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
#include <codecvt>
#include <locale>
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
    static XAttribute defaultAtributes[3] = {{u"version", u"1.0"}, {u"encoding", u"UTF-8"}, {u"standalone", u"no"}};
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    inline bool validTagNameChar(XChar c)
    {
        return (std::isalpha(c) || std::isdigit(c) || c == '-' || c == '_' || c == '.');
    }
    inline bool validAttributeNameChar(XChar c)
    {
        return (std::isalpha(c) || std::isdigit(c) || c == '_');
    }
    inline XString XML::toUpper(XString str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned int c) { return std::toupper(c); });
        return str;
    }
    inline bool XML::attributePresent(std::vector<XAttribute> attributes, const XString &name)
    {
        return (std::find_if(attributes.begin(), attributes.end(),
                             [&name](const XAttribute &attr) { return (attr.name == name); }) != attributes.end());
    }
    inline bool XML::validTagName(XString tagName)
    {
        std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
        return (!(std::isdigit(tagName[0]) || tagName[0] == '-' || tagName[0] == '.' || tagName.substr(0, 3) == u"xml"));
    }
    inline bool XML::validAtttributeName(XString attributeName)
    {
        return (!std::isdigit(attributeName[0]));
    }
    std::vector<XAttribute> XML::validateDeclaration(const std::vector<XAttribute> &attributes)
    {
        // Syntax error if no version present
        if (!attributePresent(attributes, u"version"))
        {
            throw XML::SyntaxError();
        }
        // Fill in gaps with default if missing attributes
        std::vector<XAttribute> validatedAttributes;
        long currentAttribute = 0;
        for (auto attrIndex = 0; attrIndex < 3; attrIndex++)
        {
            if ((currentAttribute < (int)attributes.size()) &&
                (attributes[currentAttribute].name == defaultAtributes[attrIndex].name))
            {
                validatedAttributes.push_back(attributes[currentAttribute]);
                currentAttribute++;
            }
            else
            {
                validatedAttributes.push_back(defaultAtributes[attrIndex]);
            }
        }
        // Order not version, encoding, standalone == syntax error
        if (currentAttribute != (long)attributes.size())
        {
            throw XML::SyntaxError();
        }
        // Check valid declaration values
        validatedAttributes[1].value = toUpper(validatedAttributes[1].value); // encoding to upper case
        if ((validatedAttributes[0].value != u"1.0") ||
            ((validatedAttributes[1].value != u"UTF-8") && (validatedAttributes[1].value != u"UTF-16")) ||
            ((validatedAttributes[2].value) != u"yes" && (validatedAttributes[2].value != u"no")))
        {
            throw XML::SyntaxError();
        }
        return (validatedAttributes);
    }
    XString XML::extractTagName(ISource &source)
    {
        m_workBuffer.clear();
        while (source.bytesToParse() && validTagNameChar(source.currentByte()))
        {
            m_workBuffer += source.currentByte();
            source.moveToNextByte();
        }
        if (!source.bytesToParse() || !validTagName(m_workBuffer))
        {
            throw XML::SyntaxError();
        }
        return (m_workBuffer);
    }
    XString XML::extractAttributeValue(ISource &source)
    {
        if ((source.currentByte() == '\'') || ((source.currentByte() == '"')))
        {
            m_workBuffer.clear();
            XChar quote = source.currentByte();
            source.moveToNextByte();
            while (source.bytesToParse() && (source.currentByte() != quote))
            {
                m_workBuffer += source.currentByte();
                source.moveToNextByte();
            }
            if (!source.bytesToParse())
            {
                throw XML::SyntaxError();
            }
            source.moveToNextByte();
            return (m_workBuffer);
        }
        throw XML::SyntaxError();
    }
    XString XML::extractAttributeName(ISource &source)
    {
        m_workBuffer.clear();
        while (source.bytesToParse() && validAttributeNameChar(source.currentByte()))
        {
            m_workBuffer += source.currentByte();
            source.moveToNextByte();
        }
        if (!source.bytesToParse() || !validAtttributeName(m_workBuffer))
        {
            throw XML::SyntaxError();
        }
        return (m_workBuffer);
    }
    void XML::parseComment(ISource &source)
    {
        while (source.bytesToParse() && !source.findString(u"-->"))
        {
            source.moveToNextByte();
        }
    }
    std::vector<XAttribute> XML::parseAttributes(ISource &source)
    {
        std::vector<XAttribute> attributes;
        while (source.currentByte() != '?' && source.currentByte() != '/' && source.currentByte() != '>')
        {
            XString name = extractAttributeName(source);
            source.ignoreWhiteSpace();
            if (source.currentByte() != '=')
            {
                throw XML::SyntaxError();
            }
            source.moveToNextByte();
            source.ignoreWhiteSpace();
            XString value = extractAttributeValue(source);
            source.ignoreWhiteSpace();
            if (!attributePresent(attributes, name))
            {
                attributes.emplace_back(name, value);
            }
            else
            {
                throw XML::SyntaxError();
            }
        }
        return (attributes);
    }
    XNodeRoot XML::parseProlog(ISource &source)
    {
        XNodeRoot xNodeRoot;
        source.ignoreWhiteSpace();
        if (source.findString(u"<?xml"))
        {
            source.ignoreWhiteSpace();
            std::vector<XAttribute> attributes = validateDeclaration(parseAttributes(source));
            if (source.findString(u"?>"))
            {
                xNodeRoot.version = attributes[0].value;
                xNodeRoot.encoding = attributes[1].value;
                xNodeRoot.standalone = attributes[2].value;
            }
            else
            {
                throw XML::SyntaxError();
            }
        }
        return (xNodeRoot);
    }
    XNodeElement XML::parseElement(ISource &source)
    {
        if (source.currentByte() != '<')
        {
            throw XML::SyntaxError();
        }
        XNodeElement xNodeElement;
        source.moveToNextByte();
        source.ignoreWhiteSpace();
        xNodeElement.name = extractTagName(source);
        source.ignoreWhiteSpace();
        xNodeElement.attributes = parseAttributes(source);
        if (source.findString(u"/>") || source.findString(u">"))
        {
            while (source.bytesToParse() && !source.findString(u"</" + xNodeElement.name + u">"))
            {
                if (source.currentByte() != '<')
                {
                    xNodeElement.contents += source.currentByte();
                    source.moveToNextByte();
                }
                else if (source.findString(u"<!--"))
                {
                    parseComment(source);
                }
                else
                {
                    xNodeElement.elements.emplace_back(parseElement(source));
                }
            }
        }
        else
        {
            throw XML::SyntaxError();
        }
        return (xNodeElement);
    }
    void XML::parseRootElement(ISource &source, XNodeRoot &xNodeRoot)
    {
        source.ignoreWhiteSpace();
        if (source.findString(u"<!--"))
        {
            parseComment(source);
            source.ignoreWhiteSpace();
        }
        XNodeElement xNodeElement = parseElement(source);
        xNodeRoot.name = xNodeElement.name;
        xNodeRoot.attributes = xNodeElement.attributes;
        xNodeRoot.contents = xNodeElement.contents;
        xNodeRoot.elements = std::move(xNodeElement.elements);
    }
    XNodeRoot XML::parseXML(ISource &source)
    {
        XNodeRoot xNodeRoot = parseProlog(source);
        parseRootElement(source, xNodeRoot);
        return (xNodeRoot);
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    XNodeRoot XML::parse(const std::string &xmlToParse)
    {
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_utf8Toutf16;
        BufferSource xml(m_utf8Toutf16.from_bytes(xmlToParse));
        return (parseXML(xml));
    }
} // namespace H4
