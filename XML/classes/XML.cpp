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
    inline bool validTagName(std::string tagName)
    {
        std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
        return (!(tagName[0] == '-' || tagName[0] == '.' || std::strncmp(tagName.c_str(), "xml", 3) == 0));
    }
    inline bool validAtttributeName(std::string attributeName)
    {
        return (!std::isdigit(attributeName[0]));
    }
    inline std::string extractTagName(XML::ISource &source)
    {
        std::string name;
        while (source.bytesToParse() && (std::strchr(validTagCharacters, source.currentByte()) != nullptr))
        {
            name += source.currentByte();
            source.moveToNextByte();
        }
        if (!source.bytesToParse() || !validTagName(name))
        {
            throw XML::SyntaxError();
        }
        return (name);
    }
    inline std::string extractAttributeValue(XML::ISource &source)
    {
        if ((source.currentByte() == '\'') || ((source.currentByte() == '"')))
        {
            std::string value;
            char quote = source.currentByte();
            source.moveToNextByte();
            while (source.bytesToParse() && (source.currentByte() != quote))
            {
                value += source.currentByte();
                source.moveToNextByte();
            }
            if (!source.bytesToParse())
            {
                throw XML::SyntaxError();
            }
            source.moveToNextByte();
            return (value);
        }
        throw XML::SyntaxError();
    }
    inline std::string extractAttributeName(XML::ISource &source)
    {
        std::string name;
        while (source.bytesToParse() && std::strchr(validAttributeCharacters, source.currentByte()) != nullptr)
        {
            name += source.currentByte();
            source.moveToNextByte();
        }
        if (!source.bytesToParse() || !validAtttributeName(name))
        {
            throw XML::SyntaxError();
        }
        return (name);
    }
    inline bool findString(XML::ISource &source, const std::string &targetString)
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
    inline void ignoreWhiteSpace(XML::ISource &source)
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
    std::vector<XAttribute> validateDeclaration(const std::vector<XAttribute> &attributes)
    {
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
    std::vector<XAttribute> parseAttributes(XML::ISource &source, const std::string &endTag)
    {
        std::vector<XAttribute> attributes;
        while (!findString(source, endTag))
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
            attributes.emplace_back(name, value);
        }
        return (attributes);
    }
    XNodeRoot XML::parseDelaration(ISource &source)
    {
        XNodeRoot xNodeRoot;
        ignoreWhiteSpace(source);
        if (findString(source, "<?xml"))
        {
            ignoreWhiteSpace(source);
            std::vector<XAttribute> attributes = validateDeclaration(parseAttributes(source, "?>"));
            xNodeRoot.version = attributes[0].value;
            xNodeRoot.encoding = attributes[1].value;
            xNodeRoot.standalone = attributes[2].value;
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
        if (!findString(source, "/>"))
        {
            xNodeElement.attributes = parseAttributes(source, ">");
            while (source.bytesToParse() && !findString(source, "</" + xNodeElement.name + ">"))
            {
                if (source.currentByte() != '<')
                {
                    xNodeElement.contents += source.currentByte();
                    source.moveToNextByte();
                }
                else
                {
                    xNodeElement.elements.emplace_back(parseElement(source));
                }
            }
        }
        return (xNodeElement);
    }
    void XML::parseRootElement(ISource &source, XNodeRoot &xNodeRoot)
    {
        ignoreWhiteSpace(source);
        XNodeElement xNodeElement = parseElement(source);
        xNodeRoot.name = xNodeElement.name;
        xNodeRoot.attributes = xNodeElement.attributes;
        xNodeRoot.contents = xNodeElement.contents;
        xNodeRoot.elements = std::move(xNodeElement.elements);
    }
    XNodeRoot XML::parseXML(ISource &source)
    {
        XNodeRoot xNodeRoot = parseDelaration(source);
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
