//
// Class: DTD
//
// Description: Parse XML Data Type Definition (DTD) portion of an XML file so
// that the XML maybe be validated against it and flagged as well-formed if it passes. 
// For more information on the XML standard check out https://www.w3.org/TR/REC-xml/.
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
    /// Intialise internal tables used by  DTD parser.
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::initialiseTables()
    {
        // Default entity mappings
        m_entityMapping["&amp;"].internal = "&#x26;";
        m_entityMapping["&quot;"].internal = "&#x22;";
        m_entityMapping["&apos;"].internal = "&#x27;";
        m_entityMapping["&lt;"].internal = "&#x3C;";
        m_entityMapping["&gt;"].internal = "&#x3E;";
        // DTD attribute list type tokens
        // Make sure longer possible matches are first
        DTD::m_dtdAttrListTypes.push_back(U"CDATA");
        DTD::m_dtdAttrListTypes.push_back(U"IDREFS");
        DTD::m_dtdAttrListTypes.push_back(U"IDREF");
        DTD::m_dtdAttrListTypes.push_back(U"ID");
        DTD::m_dtdAttrListTypes.push_back(U"NMTOKENS");
        DTD::m_dtdAttrListTypes.push_back(U"NMTOKEN");
        DTD::m_dtdAttrListTypes.push_back(U"ENTITIES");
        DTD::m_dtdAttrListTypes.push_back(U"ENTITY");
        DTD::m_dtdAttrListTypes.push_back(U"NOTATION");
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Parse DTD read from source stream.
    /// </summary>
    void DTD::parse(ISource &dtdSource)
    {
        parseDTD(dtdSource);
    }
    /// <summary>
    /// Validate XML against its DTD. Throwing an exception if there is a
    /// issue and the XML is not well-formed.
    /// </summary>
    /// <returns></returns>
    void DTD::validate(XMLNodeElement &prolog)
    {
        validateDTD(prolog);
    }
    /// <summary>
    /// Create DTD text from an DTD object.
    /// </summary>
    /// <param name="xmlDestination">DTD destination stream.</param>
    void DTD::stringify(IDestination &xmlDestination)
    {
        xmlDestination.add(m_unparsed);
    }
} // namespace H4
