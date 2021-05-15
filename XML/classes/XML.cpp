//
// Class: XML
//
// Description: Parse XML into an XMLObject (generating an exception
// if it is found not to be wellformed XML) so that it can be manipulated,
// interrogated and saved back to a text form. For more information on the
// XML standard check out https://www.w3.org/TR/REC-xml/.
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
    /// Intialise internal tables used by  XML parser.
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::initialiseTables()
    {
        // Default entity mappings
        m_entityMapping["&amp;"].internal = "&#x26;";
        m_entityMapping["&quot;"].internal = "&#x22;";
        m_entityMapping["&apos;"].internal = "&#x27;";
        m_entityMapping["&lt;"].internal = "&#x3C;";
        m_entityMapping["&gt;"].internal = "&#x3E;";
        // DTD attribute list type tokens
        // Make sure longer possible matches are first
        XML::m_dtdAttrListTypes.push_back(U"CDATA");
        XML::m_dtdAttrListTypes.push_back(U"IDREFS");
        XML::m_dtdAttrListTypes.push_back(U"IDREF");
        XML::m_dtdAttrListTypes.push_back(U"ID");
        XML::m_dtdAttrListTypes.push_back(U"NMTOKENS");
        XML::m_dtdAttrListTypes.push_back(U"NMTOKEN");
        XML::m_dtdAttrListTypes.push_back(U"ENTITIES");
        XML::m_dtdAttrListTypes.push_back(U"ENTITY");
        XML::m_dtdAttrListTypes.push_back(U"NOTATION");
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Parse XML read from source stream.
    /// </summary>
    void XML::parse()
    {
        xmlParse();
    }
    /// <summary>
    ///
    /// </summary>
    /// <returns></returns>
    void XML::validate()
    {
        dtdValidateXML();
    }
    /// <summary>
    /// Create XML text from an XML object.
    /// </summary>
    /// <param name="xmlDestination">XML destination stream.</param>
    void XML::stringify(IDestination &xmlDestination)
    {
        stringifyXML(xmlDestination);
    }
} // namespace H4
