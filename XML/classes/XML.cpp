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
        while (source.charactersToParse() &&
               source.currentCharacter() != '/' &&
               source.currentCharacter() != '>' &&
               !std::iswspace(source.currentCharacter()))
        {
            m_workBuffer += source.currentCharacter();
            source.moveToNextCharacter();
        }
        if (!validateTagName(m_workBuffer))
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
            while (source.charactersToParse() &&
                   source.currentCharacter() != quote)
            {
                XChar ch;
                if (source.currentCharacter() == '&')
                {
                    ch = parseCharacterEntities(source);
                }
                else
                {
                    ch = source.currentCharacter();
                    source.moveToNextCharacter();
                }
                m_workBuffer += ch;
            }
            source.moveToNextCharacter();
            return (m_workBuffer);
        }
        throw XML::SyntaxError();
    }
    XString XML::extractAttributeName(ISource &source)
    {
        m_workBuffer.clear();
        while (source.charactersToParse() &&
               source.currentCharacter() != '=' &&
               !std::iswspace(source.currentCharacter()))
        {
            m_workBuffer += source.currentCharacter();
            source.moveToNextCharacter();
        }
        if (!validateAttributeName(m_workBuffer))
        {
            throw XML::SyntaxError();
        }
        return (m_workBuffer);
    }
    XChar XML::parseCharacterEntities(ISource &source)
    {
        XString entityName;
        source.moveToNextCharacter();
        while (source.currentCharacter() && source.currentCharacter() != ';')
        {
            entityName += source.currentCharacter();
            source.moveToNextCharacter();
        }
        source.moveToNextCharacter();
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
    void XML::parseComment(ISource &source)
    {
        while (source.charactersToParse() && !source.foundString(U"--"))
        {
            source.moveToNextCharacter();
        }
        if (source.currentCharacter() != '>')
        {
            throw SyntaxError();
        }
        source.moveToNextCharacter();
    }
    std::vector<XAttribute> XML::parseAttributes(ISource &source)
    {
        std::vector<XAttribute> attributes;
        while (source.currentCharacter() != '?' &&
               source.currentCharacter() != '/' &&
               source.currentCharacter() != '>')
        {
            XString attributeName = extractAttributeName(source);
            source.ignoreWhiteSpace();
            if (source.currentCharacter() != '=')
            {
                throw XML::SyntaxError();
            }
            source.moveToNextCharacter();
            source.ignoreWhiteSpace();
            XString attributeValue = extractAttributeValue(source);
            source.ignoreWhiteSpace();
            if (!attributePresent(attributes, attributeName))
            {
                attributes.emplace_back(m_toFromUTF8.to_bytes(attributeName), m_toFromUTF8.to_bytes(attributeValue));
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
        if (source.foundString(U"<?xml"))
        {
            source.ignoreWhiteSpace();
            std::vector<XAttribute> attributes = validateDeclaration(parseAttributes(source));
            if (source.foundString(U"?>"))
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
        XNodeElement xNodeElement;
        source.moveToNextCharacter();
        source.ignoreWhiteSpace();
        xNodeElement.name = m_toFromUTF8.to_bytes(extractTagName(source));
        source.ignoreWhiteSpace();
        xNodeElement.attributes = parseAttributes(source);
        XString closingTag = U"</" + m_toFromUTF8.from_bytes(xNodeElement.name) + U">";
        if (source.foundString(U"/>") || source.foundString(U">"))
        {
            while (source.charactersToParse() && !source.foundString(closingTag))
            {
                if (source.currentCharacter() != '<')
                {
                    XChar ch;
                    if (source.currentCharacter() == '&')
                    {
                        ch = parseCharacterEntities(source);
                    }
                    else
                    {
                        ch = source.currentCharacter();
                        source.moveToNextCharacter();
                    }
                    xNodeElement.contents += m_toFromUTF8.to_bytes(ch);
                }
                else if (source.foundString(U"<!--"))
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
        if (source.foundString(U"<!--"))
        {
            parseComment(source);
            source.ignoreWhiteSpace();
        }
        if (source.currentCharacter() != '<')
        {
            throw XML::SyntaxError();
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
        BufferSource xml(m_toFromUTF8.from_bytes(convertCRLF(xmlToParse)));
        return (parseXML(xml));
    }
} // namespace H4
