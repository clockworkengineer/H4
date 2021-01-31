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
    inline bool validTagNameChar(char c)
    {
        static const char *validTag{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_."};
        return (std::strchr(validTag, c) != nullptr);
    }
    inline bool validAttributeNameChar(char c)
    {
        static const char *validAttribute{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"};
        return (std::strchr(validAttribute, c) != nullptr);
    }
    inline std::string XML::toUpper(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return str;
    }
    inline bool XML::checkValid(const char *valid, const char c)
    {
        return (std::strchr(valid, c) != nullptr);
    }
    inline bool XML::attributePresent(std::vector<XAttribute> attributes, const std::string &name)
    {
        return (std::find_if(attributes.begin(), attributes.end(),
                             [&name](const XAttribute &attr) { return (attr.name == name); }) != attributes.end());
    }
    inline bool XML::validTagName(std::string tagName)
    {
        std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
        return (!(std::isdigit(tagName[0]) || tagName[0] == '-' || tagName[0] == '.' || std::strncmp(tagName.c_str(), "xml", 3) == 0));
    }
    inline bool XML::validAtttributeName(std::string attributeName)
    {
        return (!std::isdigit(attributeName[0]));
    }
    std::vector<XAttribute> XML::validateDeclaration(const std::vector<XAttribute> &attributes)
    {
        // Syntax error if no version present
        if (!attributePresent(attributes, "version"))
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
        if ((validatedAttributes[0].value != "1.0") ||
            ((validatedAttributes[1].value != "UTF-8") && (validatedAttributes[1].value != "UTF-16")) ||
            ((validatedAttributes[2].value) != "yes" && (validatedAttributes[2].value != "no")))
        {
            throw XML::SyntaxError();
        }
        return (validatedAttributes);
    }
    std::string XML::extractTagName(ISource &source)
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
    std::string XML::extractAttributeValue(ISource &source)
    {
        if ((source.currentByte() == '\'') || ((source.currentByte() == '"')))
        {
            m_workBuffer.clear();
            char quote = source.currentByte();
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
    std::string XML::extractAttributeName(ISource &source)
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
        while (source.bytesToParse() && !source.findString("-->"))
        {
            source.moveToNextByte();
        }
    }
    std::vector<XAttribute> XML::parseAttributes(ISource &source)
    {
        std::vector<XAttribute> attributes;
        while (source.currentByte() != '?' && source.currentByte() != '/' && source.currentByte() != '>')
        {
            std::string name = extractAttributeName(source);
            source.ignoreWhiteSpace();
            if (source.currentByte() != '=')
            {
                throw XML::SyntaxError();
            }
            source.moveToNextByte();
            source.ignoreWhiteSpace();
            std::string value = extractAttributeValue(source);
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
        if (source.findString("<?xml"))
        {
            source.ignoreWhiteSpace();
            std::vector<XAttribute> attributes = validateDeclaration(parseAttributes(source));
            if (source.findString("?>"))
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
        if (source.findString("/>") || source.findString(">"))
        {
            while (source.bytesToParse() && !source.findString("</" + xNodeElement.name + ">"))
            {
                if (source.currentByte() != '<')
                {
                    xNodeElement.contents += source.currentByte();
                    source.moveToNextByte();
                }
                else if (source.findString("<!--"))
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
        if (source.findString("<!--"))
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
        BufferSource xml(xmlToParse);
        return (parseXML(xml));
    }
} // namespace H4
