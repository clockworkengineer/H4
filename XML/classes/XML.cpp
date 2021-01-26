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
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    inline std::string extractAttributeValue(XML::ISource &source)
    {
        std::string value;
        while (source.bytesToParse() && std::isalpha(source.currentByte()))
        {
            value += source.currentByte();
        }
        if (!source.bytesToParse())
        {
            throw XML::SyntaxError();
        }
        return (value);
    }
    inline std::string extractAttributeName(XML::ISource &source)
    {
        std::string name;
        while (source.bytesToParse() && std::isalpha(source.currentByte()))
        {
            name += source.currentByte();
        }
        if (!source.bytesToParse())
        {
            throw XML::SyntaxError();
        }
        return (name);
    }
    inline bool startsWith(XML::ISource &source, const std::string &targetString)
    {
        long index = 0;
        while (source.bytesToParse() && source.currentByte() == targetString[index])
        {
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
    std::list<XML::XAttribute> parseAttributes(XML::ISource &source, const std::string &endTag)
    {
        std::list<XML::XAttribute> attributes;
        while (!startsWith(source, endTag))
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
        }
        return (attributes);
    }
    XNodeRoot XML::parseDelaration(ISource &source)
    {
        XNodeRoot xNodeRoot;
        ignoreWhiteSpace(source);
        if (startsWith(source, "<?xml"))
        {
            ignoreWhiteSpace(source);
            parseAttributes(source, "?>");
        }
        return (xNodeRoot);
    }
    XNodeRoot XML::parseXML(ISource &source)
    {
        return (parseDelaration(source));
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
