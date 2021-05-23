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
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseEntityMappingContents(XMLNodeElement *xNodeElement, XMLValue &entityReference)
    {
        XMLNodeEntityReference xNodeEntityReference(entityReference);
        if (entityReference.unparsed[1] != '#')
        {
            XMLNodeElement entityElement;
            BufferSource entitySource(entityReference.parsed);
            if (entityReference.parsed.find("<") == std::string::npos)
            {
                while (entitySource.more())
                {
                    parseElementContents(entitySource, &entityElement);
                }
                xNodeEntityReference.children = std::move(entityElement.children);
                if (!xNodeElement->children.empty())
                {
                    if (xNodeElement->children.back()->getNodeType() == XMLNodeType::content)
                    {
                        XMLNodeRef<XMLNodeContent>(*xNodeElement->children.back()).isWhiteSpace = false;
                    }
                }
            }
            else
            {
                while (entitySource.more())
                {
                    parseElementContents(entitySource, xNodeElement);
                }
                return;
            }
        }
        xNodeElement->children.emplace_back(std::make_unique<XMLNodeEntityReference>(std::move(xNodeEntityReference)));
    }
} // namespace H4
