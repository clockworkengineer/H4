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
    void XML::xmlParseEntityMappingContents(XNodeElement *xNodeElement, XValue &entityReference)
    {
        XNodeEntityReference xNodeEntityReference(entityReference);
        if (entityReference.unparsed[1] != '#')
        {
            XNodeElement entityElement;
            BufferSource entitySource(entityReference.parsed);
            if (entityReference.parsed.find("<") == std::string::npos)
            {
                while (entitySource.more())
                {
                    xmlParseElementContents(entitySource, &entityElement);
                }
                xNodeEntityReference.children = std::move(entityElement.children);
                if (!xNodeElement->children.empty())
                {
                    if (xNodeElement->children.back()->getNodeType() == XNodeType::content)
                    {
                        XNodeRef<XNodeContent>(*xNodeElement->children.back()).isWhiteSpace = false;
                    }
                }
            }
            else
            {
                while (entitySource.more())
                {
                    xmlParseElementContents(entitySource, xNodeElement);
                }
                return;
            }
        }
        xNodeElement->children.emplace_back(std::make_unique<XNodeEntityReference>(std::move(xNodeEntityReference)));
    }
} // namespace H4
