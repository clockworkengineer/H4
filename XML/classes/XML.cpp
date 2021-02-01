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
    inline bool XML::validNameStartChar(XChar c)
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
    inline bool XML::validNameChar(XChar c)
    {
        return (validNameStartChar(c) ||
                (c == '-') ||
                (c == '.') ||
                (c >= '0' && c <= '9') ||
                (c == 0xB7) ||
                (c >= 0x0300 && c <= 0x036F) ||
                (c >= 0x203F && c <= 0x2040));
    }
    inline bool XML::attributePresent(std::vector<XAttribute> attributes, const XString &name)
    {
        return (std::find_if(attributes.begin(), attributes.end(),
        [&name, this](const XAttribute &attr) { return (attr.name == m_toFromUTF8.to_bytes(name)); }) != attributes.end());
    }
    inline bool XML::validateAttributeName(XString name)
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
    inline bool XML::validateTagName(XString name)
    {
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.substr(0, 3) == U"xml")
        {
            return (false);
        }
        return (validateAttributeName(name));
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
        // Encoding all upper case
        std::transform(validatedAttributes[1].value.begin(), validatedAttributes[1].value.end(),
                       validatedAttributes[1].value.begin(), [](unsigned int c) { return std::toupper(c); });
        // Check valid declaration values
        if ((validatedAttributes[0].value != "1.0") ||
            ((validatedAttributes[1].value != "UTF-8") && (validatedAttributes[1].value != "UTF-16")) ||
            ((validatedAttributes[2].value) != "yes" && (validatedAttributes[2].value != "no")))
        {
            throw XML::SyntaxError();
        }
        return (validatedAttributes);
    }
    XString XML::extractTagName(ISource &source)
    {
        m_workBuffer.clear();
        while (source.charactersToParse() && validNameChar(source.currentCharacter()))
        {
            m_workBuffer += source.currentCharacter();
            source.moveToNextCharacter();
        }
        if (!source.charactersToParse() || !validateTagName(m_workBuffer))
        {
            throw XML::SyntaxError();
        }
        return (m_workBuffer);
    }
    XString XML::extractAttributeValue(ISource &source)
    {
        if ((source.currentCharacter() == '\'') || ((source.currentCharacter() == '"')))
        {
            m_workBuffer.clear();
            XChar quote = source.currentCharacter();
            source.moveToNextCharacter();
            while (source.charactersToParse() && (source.currentCharacter() != quote))
            {
                m_workBuffer += source.currentCharacter();
                source.moveToNextCharacter();
            }
            if (!source.charactersToParse())
            {
                throw XML::SyntaxError();
            }
            source.moveToNextCharacter();
            return (m_workBuffer);
        }
        throw XML::SyntaxError();
    }
    XString XML::extractAttributeName(ISource &source)
    {
        m_workBuffer.clear();
        while (source.charactersToParse() && validNameChar(source.currentCharacter()))
        {
            m_workBuffer += source.currentCharacter();
            source.moveToNextCharacter();
        }
        if (!source.charactersToParse() || !validateAttributeName(m_workBuffer))
        {
            throw XML::SyntaxError();
        }
        return (m_workBuffer);
    }
    void XML::parseComment(ISource &source)
    {
        while (source.charactersToParse() && !source.findString(U"-->"))
        {
            source.moveToNextCharacter();
        }
    }
    std::vector<XAttribute> XML::parseAttributes(ISource &source)
    {
        std::vector<XAttribute> attributes;
        while (source.currentCharacter() != '?' && source.currentCharacter() != '/' && source.currentCharacter() != '>')
        {
            XString name = extractAttributeName(source);
            source.ignoreWhiteSpace();
            if (source.currentCharacter() != '=')
            {
                throw XML::SyntaxError();
            }
            source.moveToNextCharacter();
            source.ignoreWhiteSpace();
            XString value = extractAttributeValue(source);
            source.ignoreWhiteSpace();
            if (!attributePresent(attributes, name))
            {
                attributes.emplace_back(m_toFromUTF8.to_bytes(name), m_toFromUTF8.to_bytes(value));
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
        if (source.currentCharacter() != '<')
        {
            throw XML::SyntaxError();
        }
        XNodeElement xNodeElement;
        source.moveToNextCharacter();
        source.ignoreWhiteSpace();
        xNodeElement.name = m_toFromUTF8.to_bytes(extractTagName(source));
        source.ignoreWhiteSpace();
        xNodeElement.attributes = parseAttributes(source);
        if (source.findString(U"/>") || source.findString(U">"))
        {
            while (source.charactersToParse() && !source.findString(U"</" + m_toFromUTF8.from_bytes(xNodeElement.name) + U">"))
            {
                if (source.currentCharacter() != '<')
                {
                    xNodeElement.contents += m_toFromUTF8.to_bytes(source.currentCharacter());
                    source.moveToNextCharacter();
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
        BufferSource xml(m_toFromUTF8.from_bytes(xmlToParse));
        return (parseXML(xml));
    }
} // namespace H4
