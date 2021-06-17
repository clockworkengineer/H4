//
// Class: XML
//
// Description: XML Parser Core Functions.
//
// Dependencies:   C17++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XMLConfig.hpp"
#include "XML.hpp"
#include "XMLSources.hpp"
#include "XMLDestinations.hpp"
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
    /// <summary>
    /// Check whether a character is valid for XML.
    /// </summary>
    /// <param name="ch">Character to validate</param>
    /// <returns>true then valid/.</returns>
    bool validChar(ISource::Char ch)
    {
        return ((ch == 0x09) ||
                (ch == kLineFeed) ||
                (ch == kCarriageReturn) ||
                (ch >= 0x20 && ch <= 0xD7FF) ||
                (ch >= 0xE000 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0x10FFFF));
    }
    /// <summary>
    /// Check whether character is a valid to start an XML name with.
    /// </summary>
    /// <param name="ch">Character value to validate.</param>
    /// <returns>true then valid.</returns>
    bool validNameStartChar(ISource::Char ch)
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
    /// <summary>
    /// Check whether a character is valid for an XML name.
    /// </summary>
    /// <param name="ch">Name to validate.</param>
    /// <returns>true then valid.</returns>
    bool validNameChar(ISource::Char ch)
    {
        return (validNameStartChar(ch) ||
                (ch == '-') ||
                (ch == '.') ||
                (ch >= '0' && ch <= '9') ||
                (ch == 0xB7) ||
                (ch >= 0x0300 && ch <= 0x036F) ||
                (ch >= 0x203F && ch <= 0x2040));
    }
    /// <summary>
    /// Check name that starts with xml is a valid reserved name.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>true then valid.</returns>
    bool validReservedName(const ISource::String &name)
    {
        return ((name.find(U"xmlns") == 0) || (name.find(U"xml-stylesheet") == 0) || (name == U"xml"));
    }
    /// <summary>
    /// Validate XML tag/attribute names.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>true then valid.</returns>
    bool validName(ISource::String name)
    {
        if (name.empty())
        {
            return (false);
        }
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.find(U"xml") == 0 && !(validReservedName(name)))
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
    /// <summary>
    /// Parse  and return XML name.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>XML name.</returns>
    std::string parseName(ISource &xmlSource)
    {
        ISource::String name;
        while (xmlSource.more() && validNameChar(xmlSource.current()))
        {
            name += xmlSource.current();
            xmlSource.next();
        }
        xmlSource.ignoreWS();
        if (!validName(name))
        {
            throw XML::SyntaxError(xmlSource, "Invalid name '" + xmlSource.to_bytes(name) + "' encountered.");
        }
        return (xmlSource.to_bytes(name));
    }
    /// <summary>
    /// Parse and return XML entity reference.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Parsed entity reference value.</returns>
    XMLValue parseEntityReference(ISource &xmlSource)
    {
        XMLValue entityReference;
        entityReference.unparsed = xmlSource.current_to_bytes();
        xmlSource.next();
        entityReference.unparsed += parseName(xmlSource);
        if (xmlSource.current() != ';')
        {
            throw XML::SyntaxError(xmlSource, "Invalidly formed entity reference.");
        }
        entityReference.unparsed += ';';
        xmlSource.next();
        return (entityReference);
    }
    /// <summary>
    /// Parse a character reference value (hex/dec) returning its value.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Character reference value.</returns>
    XMLValue parseCharacterReference(ISource &xmlSource)
    {
        XMLValue characterRefence;
        characterRefence.unparsed = "&#";
        while (xmlSource.more() && xmlSource.current() != ';')
        {
            characterRefence.unparsed += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        if (xmlSource.current() != ';')
        {
            throw XML::SyntaxError(xmlSource, "Invalidly formed  character reference.");
        }
        xmlSource.next();
        characterRefence.unparsed += ';';
        std::string reference = characterRefence.unparsed;
        char *end;
        long result = 10;
        if (reference[2] == 'x')
        {
            reference = reference.substr(3, reference.size() - 4);
            result = 16;
        }
        else
        {
            reference = reference.substr(2, reference.size() - 3);
        }
        result = std::strtol(reference.c_str(), &end, result);
        if (*end == '\0')
        {
            if (!validChar(result))
            {
                throw XML::SyntaxError(xmlSource, "Character reference invalid character.");
            }
            characterRefence.parsed = xmlSource.to_bytes(result);
            return (characterRefence);
        }
        throw XML::SyntaxError(xmlSource, "Cannot convert character reference.");
    }
    /// <summary>
    /// Lookup an entity reference to get its parsed value.
    /// </summary>
    /// <param name="entityReference">Entity reference to lookup and return.</param>
    /// <param name="entityMapping">Entity mapping table.</param>
    void mapEntityReference(XMLValue &entityReference, XMLEntityMappings &entityMapping)
    {
        if (entityMapping.count(entityReference.unparsed) > 0)
        {
            if (!entityMapping[entityReference.unparsed].internal.empty())
            {
                entityReference.parsed = entityMapping[entityReference.unparsed].internal;
            }
            else
            {
                if (std::filesystem::exists(entityMapping[entityReference.unparsed].external.systemID))
                {
                    FileSource entitySource(entityMapping[entityReference.unparsed].external.systemID);
                    while (entitySource.more())
                    {
                        entityReference.parsed += entitySource.current_to_bytes();
                        entitySource.next();
                    }
                }
                else
                {
                    throw XML::SyntaxError("Entity '" + entityReference.unparsed + "' source file '" + entityMapping[entityReference.unparsed].external.systemID + "' does not exist.");
                }
            }
        }
    }
    /// <summary>
    /// Parse character value which can be either be a plain character,
    /// character reference or entity reference that maps to a string of
    /// characters.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <param name="entityMapping">Entity mapping table.</param>
    /// <param name="translateEntity">== true then translate entity reference</param>
    /// <returns>Character value.</returns>
    XMLValue parseCharacter(ISource &xmlSource, std::unordered_map<std::string, XMLEntityMapping> &entityMapping, bool translateEntity)
    {
        XMLValue character;
        if (xmlSource.match(U"&#"))
        {
            character = parseCharacterReference(xmlSource);
        }
        else if (xmlSource.current() == '&')
        {
            character = parseEntityReference(xmlSource);
            if (translateEntity)
            {
                mapEntityReference(character, entityMapping);
            }
            else
            {
                character.parsed = character.unparsed;
            }
        }
        else if (validChar(xmlSource.current()))
        {
            character.parsed = xmlSource.current_to_bytes();
            character.unparsed = character.parsed;
            xmlSource.next();
        }
        else
        {
            throw XML::SyntaxError(xmlSource, "Invalid character value encountered.");
        }
        return (character);
    }
    /// <summary>
    /// Parse literal string value and return it.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Literal string value.</returns>
    XMLValue parseValue(ISource &xmlSource, XMLEntityMappings &entityMapping, bool translateEntity)
    {
        if ((xmlSource.current() == '\'') || ((xmlSource.current() == '"')))
        {
            XMLValue value;
            ISource::Char quote = xmlSource.current();
            xmlSource.next();
            while (xmlSource.more() && xmlSource.current() != quote)
            {
                value += parseCharacter(xmlSource, entityMapping, translateEntity);
            }
            xmlSource.next();
            xmlSource.ignoreWS();
            return (value);
        }
        throw XML::SyntaxError(xmlSource, "Invalid attribute value.");
    }
    /// <summary>
    /// Extract body of tag up until '>'.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Body of tag.</returns>
    std::string extractTagBody(ISource &xmlSource)
    {
        xmlSource.ignoreWS();
        std::string body;
        while (xmlSource.more() && xmlSource.current() != '>')
        {
            body += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        return (body);
    }
    /// <summary>
    /// Translate any entities found in passed in string.
    /// </summary>
    /// <param name=""></param>
    /// <returns>Translated string.</returns>
    std::string translateEntities(const std::string &toTranslate, const XMLEntityMappings &entityMapping, char type)
    {
        std::string translated = toTranslate;
        bool matchFound;
        do
        {
            matchFound = false;
            for (auto entity : entityMapping)
            {
                if (entity.first[0] == type)
                {
                    size_t pos = translated.find(entity.first);
                    if (pos != std::string::npos)
                    {
                        translated.replace(pos, entity.first.length(), entity.second.internal);
                        matchFound = true;
                    }
                }
            }
        } while (matchFound);
        return (translated);
    }
    /// <summary>
    /// Split a string into a vector of strings using the passed in delimeter.
    /// </summary>
    /// <param name="stringToSplit">String to split up.</param>
    /// <param name="delimeter">Character delimeter to split on.</param>
    /// <returns>Vector of split strings.</returns>
    std::vector<std::string> splitString(std::string stringToSplit, char delimeter)
    {
        std::stringstream sourceStream(stringToSplit);
        std::string splitOffItem;
        std::vector<std::string> splitStrings;
        while (std::getline(sourceStream, splitOffItem, delimeter))
        {
            splitStrings.push_back(splitOffItem);
        }
        return splitStrings;
    }
    /// <summary>
    /// Trimm whitespace from beginning and end of a string.
    /// </summary>
    /// <param name="stringToTrimm">String to trimm.</param>
    void trimmString(std::string &stringToTrimm)
    {
        stringToTrimm.erase(stringToTrimm.begin(), std::find_if(stringToTrimm.begin(), stringToTrimm.end(), [](unsigned char ch)
                                                                { return !std::iswspace(ch); }));
        stringToTrimm.erase(std::find_if(stringToTrimm.rbegin(), stringToTrimm.rend(), [](unsigned char ch)
                                         { return !std::iswspace(ch); }).base(),
                            stringToTrimm.end());
    }
} // namespace H4
