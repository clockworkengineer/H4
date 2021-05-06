//
// Class: XML
//
// Description: XML parser entity/character reference related functionality.
//
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
#include <filesystem>
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
    /// Parse a character reference returning its value.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Character reference value.</returns>
    std::string XML::xmlParseCharacterReference(ISource &xmlSource, std::string reference)
    {
        char *end;
        long result = 10;
        if (reference[0] == 'x')
        {
            reference = reference.substr(1);
            result = 16;
        }
        result = std::strtol(reference.c_str(), &end, result);
        if (*end == '\0')
        {
            if (!validChar(result))
            {
                throw SyntaxError(xmlSource, "Character reference invalid character.");
            }
            return (xmlSource.to_bytes(result));
        }
        throw SyntaxError(xmlSource, "Cannot convert character reference.");
    }
    /// <summary>
    /// Parse XML string value representing a character reference or entity.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Value for reference or enity.</returns>
    XValue XML::xmlParseReferenceOrEntity(ISource &xmlSource)
    {
        XValue entityReference;
        while (xmlSource.more() && xmlSource.current() != ';')
        {
            entityReference.unparsed += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        if (xmlSource.current() != ';')
        {
            throw SyntaxError(xmlSource, "Invalidly formed  character reference or entity.");
        }
        entityReference.unparsed += ';';
        xmlSource.next();
        if (entityReference.unparsed[1] == '#')
        {
            entityReference.parsed = xmlParseCharacterReference(xmlSource, entityReference.unparsed.substr(2, entityReference.unparsed.size() - 3));
        }
        else if (m_entityMapping.count(entityReference.unparsed) > 0)
        {
            if (!m_entityMapping[entityReference.unparsed].internal.empty())
            {
                entityReference.parsed = m_entityMapping[entityReference.unparsed].internal;
            }
            else
            {
                if (std::filesystem::exists(m_entityMapping[entityReference.unparsed].external.systemID))
                {
                    FileSource entitySource(m_entityMapping[entityReference.unparsed].external.systemID);
                    while (entitySource.more())
                    {
                        entityReference.parsed += entitySource.current_to_bytes();
                        entitySource.next();
                    }
                }
                else
                {
                    throw SyntaxError("Entity '" + entityReference.unparsed + "' source file '" + m_entityMapping[entityReference.unparsed].external.systemID + "' does not exist.");
                }
            }
        }
        return (entityReference);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    bool XML::xmlEntityMappingParse(XNodeElement *xNodeElement, XValue &entityReference)
    {
        if (entityReference.unparsed.starts_with("&") && entityReference.unparsed.ends_with(";"))
        {
            XNodeEntityReference xNodeEntityReference(entityReference);
            if ((entityReference.parsed != "&") &&
                (entityReference.parsed != "\'") &&
                (entityReference.parsed != "\"") &&
                (entityReference.parsed != ">") &&
                (entityReference.parsed != "<"))
            {
                XNodeElement entityElement;
                BufferSource entitySource(entityReference.parsed);
                while (entitySource.more())
                {
                    xmlParseElementContents(entitySource, &entityElement);
                }
                xNodeEntityReference.elements = std::move(entityElement.elements);
            }
            if (!xNodeElement->elements.empty())
            {
                if (xNodeElement->elements.back()->getNodeType() == XNodeType::content)
                {
                    XNodeRef<XNodeContent>(*xNodeElement->elements.back()).isWhiteSpace = false;
                }
            }
            xNodeElement->elements.emplace_back(std::make_unique<XNodeEntityReference>(std::move(xNodeEntityReference)));
            return (true);
        }
        return (false);
    }
} // namespace H4
