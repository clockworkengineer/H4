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
    static XAttribute defaultAtributes[3] = {{U"version", U"1.0"}, {U"encoding", U"UTF-8"}, {U"standalone", U"no"}};
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    inline bool validNameStartChar(XChar c)
    {
        return ((c == ':') ||
                (c == '_') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c >= 0xC0 && c <= 0xD6) ||
                (c >= 0xD8 && c <= 0xF6) ||
                (c >= 0xF8 && c <= 0x2FF) ||
                (c >= 0x370 && c <= 0x37D) ||
                (c >= 0x37F && c <= 0x1FFF) ||
                (c >= 0x200C && c <= 0x200D) ||
                (c >= 0x2070 && c <= 0x218F) ||
                (c >= 0x2C00 && c <= 0x2FEF) ||
                (c >= 0x3001 && c <= 0xD7FF) ||
                (c >= 0xF900 && c <= 0xFDCF) ||
                (c >= 0xFDF0 && c <= 0xFFFD) ||
                (c >= 0x10000 && c <= 0xEFFFF));
    }
    inline bool validNameChar(XChar c)
    {
        return (validNameStartChar(c) ||
                (c == '-') ||
                (c == '.') ||
                (c >= '0' && c <= '9') ||
                (c == 0xB7) ||
                (c >= 0x0300 && c <= 0x036F) ||
                (c >= 0x203F && c <= 0x02040));
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
    inline bool XML::validAttributeName(XString name)
    {
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
    inline bool XML::validTagName(XString name)
    {
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.substr(0, 3) == U"xml")
        {
            return (false);
        }
        return (validAttributeName(name));
    }
    std::vector<XAttribute> XML::validateDeclaration(const std::vector<XAttribute> &attributes)
    {
        // Syntax error if no version present
        if (!attributePresent(attributes, U"version"))
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
        if ((validatedAttributes[0].value != U"1.0") ||
            ((validatedAttributes[1].value != U"UTF-8") && (validatedAttributes[1].value != U"UTF-16")) ||
            ((validatedAttributes[2].value) != U"yes" && (validatedAttributes[2].value != U"no")))
        {
            throw XML::SyntaxError();
        }
        return (validatedAttributes);
    }
    XString XML::extractTagName(ISource &source)
    {
        m_workBuffer.clear();
        while (source.bytesToParse() && validNameChar(source.currentByte()))
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
        while (source.bytesToParse() && validNameChar(source.currentByte()))
        {
            m_workBuffer += source.currentByte();
            source.moveToNextByte();
        }
        if (!source.bytesToParse() || !validAttributeName(m_workBuffer))
        {
            throw XML::SyntaxError();
        }
        return (m_workBuffer);
    }
    void XML::parseComment(ISource &source)
    {
        while (source.bytesToParse() && !source.findString(U"-->"))
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
        if (source.findString(U"<?xml"))
        {
            source.ignoreWhiteSpace();
            std::vector<XAttribute> attributes = validateDeclaration(parseAttributes(source));
            if (source.findString(U"?>"))
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
        if (source.findString(U"/>") || source.findString(U">"))
        {
            while (source.bytesToParse() && !source.findString(U"</" + xNodeElement.name + U">"))
            {
                if (source.currentByte() != '<')
                {
                    xNodeElement.contents += source.currentByte();
                    source.moveToNextByte();
                }
                else if (source.findString(U"<!--"))
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
        if (source.findString(U"<!--"))
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
        std::wstring_convert<std::codecvt_utf8_utf16<XString::value_type>, XString::value_type> toFromUTF8;
        BufferSource xml(toFromUTF8.from_bytes(xmlToParse));
        return (parseXML(xml));
    }
} // namespace H4
