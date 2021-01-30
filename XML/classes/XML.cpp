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
    static const char *validTagCharacters{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_."};
    static const char *validAttributeCharacters{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"};
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    inline bool attributePresent(std::vector<XAttribute> attributes, const std::string &name)
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
        if (currentAttribute != (long)attributes.size())
        {
            throw XML::SyntaxError();
        }
        return (validatedAttributes);
    }
    std::string XML::extractTagName(ISource &source)
    {
        std::string tagName;
        while (source.bytesToParse() && (std::strchr(validTagCharacters, source.currentByte()) != nullptr))
        {
            tagName += source.currentByte();
            source.moveToNextByte();
        }
        if (!source.bytesToParse() || !validTagName(tagName))
        {
            throw XML::SyntaxError();
        }
        return (tagName);
    }
    std::string XML::extractAttributeValue(ISource &source)
    {
        if ((source.currentByte() == '\'') || ((source.currentByte() == '"')))
        {
            std::string attributeValue;
            char quote = source.currentByte();
            source.moveToNextByte();
            while (source.bytesToParse() && (source.currentByte() != quote))
            {
                attributeValue += source.currentByte();
                source.moveToNextByte();
            }
            if (!source.bytesToParse())
            {
                throw XML::SyntaxError();
            }
            source.moveToNextByte();
            return (attributeValue);
        }
        throw XML::SyntaxError();
    }
    std::string XML::extractAttributeName(ISource &source)
    {
        std::string attributeName;
        while (source.bytesToParse() && std::strchr(validAttributeCharacters, source.currentByte()) != nullptr)
        {
            attributeName += source.currentByte();
            source.moveToNextByte();
        }
        if (!source.bytesToParse() || !validAtttributeName(attributeName))
        {
            throw XML::SyntaxError();
        }
        return (attributeName);
    }
    bool XML::findString(ISource &source, const std::string &targetString)
    {
        long index = 0;
        while (source.bytesToParse() && source.currentByte() == targetString[index])
        {
            source.moveToNextByte();
            if (++index == (long)targetString.length())
            {
                return (true);
            }
        }
        source.backupBytes(index);
        return (false);
    }
    void XML::ignoreWhiteSpace(ISource &source)
    {
        while (source.bytesToParse() && std::iswspace(source.currentByte()))
        {
            source.moveToNextByte();
        }
        if (!source.bytesToParse())
        {
            throw XML::SyntaxError();
        }
    }
    void XML::parseComment(ISource &source)
    {
        while (source.bytesToParse() && !findString(source, "-->"))
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
            ignoreWhiteSpace(source);
            if (source.currentByte() != '=')
            {
                throw XML::SyntaxError();
            }
            source.moveToNextByte();
            ignoreWhiteSpace(source);
            std::string value = extractAttributeValue(source);
            ignoreWhiteSpace(source);
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
        ignoreWhiteSpace(source);
        if (findString(source, "<?xml"))
        {
            ignoreWhiteSpace(source);
            std::vector<XAttribute> attributes = validateDeclaration(parseAttributes(source));
            if (findString(source, "?>"))
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
        if (source.currentByte() != '<')
        {
            throw XML::SyntaxError();
        }
        source.moveToNextByte();
        ignoreWhiteSpace(source);
        xNodeElement.name = extractTagName(source);
        ignoreWhiteSpace(source);
        xNodeElement.attributes = parseAttributes(source);
        if (findString(source, "/>") || findString(source, ">"))
        {
            while (source.bytesToParse() && !findString(source, "</" + xNodeElement.name + ">"))
            {
                if (source.currentByte() != '<')
                {
                    xNodeElement.contents += source.currentByte();
                    source.moveToNextByte();
                }
                else if (findString(source, "<!--"))
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
        ignoreWhiteSpace(source);
        if (findString(source, "<!--"))
        {
            parseComment(source);
            ignoreWhiteSpace(source);
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
