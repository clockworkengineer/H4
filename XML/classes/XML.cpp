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
            source.moveToNextByte();
            return (value);
        }
        throw XML::SyntaxError();
    }
    inline std::string extractAttributeName(XML::ISource &source)
    {
        std::string name;
        while (source.bytesToParse() && std::isalpha(source.currentByte()))
        {
            name += source.currentByte();
            source.moveToNextByte();
        }
        source.moveToNextByte();
        return (name);
    }
    inline bool startsWith(XML::ISource &source, const std::string &targetString)
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
    std::vector<XML::XAttribute> validateDeclaration(const std::vector<XML::XAttribute> &attributes)
    {
        std::vector<XML::XAttribute> returned;
        static XML::XAttribute defaultAtributes[3] = {{"version", "1.0"}, {"encoding", "UTF-8"}, {"standalone", "no"}};
        if (attributes.size() <= 3)
        {
            long current = 0;
            for (auto attrIndex = 0; attrIndex < 3; attrIndex++)
            {
                if ((current<(int)attributes.size()) && 
                    (attributes[current].name == defaultAtributes[attrIndex].name))
                {
                    returned.push_back(attributes[current]);
                    current++;
                }
                else
                {
                    returned.push_back(defaultAtributes[attrIndex]);
                }
            }
            if (current != (long) attributes.size())
            {
                throw XML::SyntaxError();
            }
        }

        return (returned);
    }
    std::vector<XML::XAttribute> parseAttributes(XML::ISource &source, const std::string &endTag)
    {
        std::vector<XML::XAttribute> attributes;
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
            ignoreWhiteSpace(source);
            attributes.emplace_back(name, value);
        }
        return (attributes);
    }

    XNodeRoot XML::parseDelaration(ISource &source)
    {
        XNodeRoot xNodeRoot;
        std::vector<XAttribute> attributes;
        ignoreWhiteSpace(source);
        if (startsWith(source, "<?xml"))
        {
            ignoreWhiteSpace(source);
            attributes = parseAttributes(source, "?>");
            attributes = validateDeclaration(attributes);
            xNodeRoot.version = attributes[0].value;
            xNodeRoot.encoding = attributes[1].value;
            xNodeRoot.standalone = attributes[2].value;
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
