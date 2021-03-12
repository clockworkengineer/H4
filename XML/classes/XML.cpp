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
        m_entityMapping[U"&amp;"] = U"&";
        m_entityMapping[U"&quot;"] = U"\"";
        m_entityMapping[U"&apos;"] = U"\'";
        m_entityMapping[U"&lt;"] = U"<";
        m_entityMapping[U"&gt;"] = U">";
        // DTD attribute list type tokens
        XML::m_dtdAttrListTypes.insert(
            {U"CDATA",
             U"IDREF",
             U"IDREFS",
             U"NMTOKEN",
             U"NMTOKENS",
             U"ENTITY",
             U"ENTITIES",
             U"NOTATION",
             U"ID"});
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Parse XML read from xmlSource.
    /// </summary>
    /// <param name="xmlSource">Source stream for XML chacaters.</param>
    /// <returns>XMLObject that represents the parsed XML.</returns>
    XMLObject XML::parse(ISource &xmlSource)
    {
        return (parseXML(xmlSource));
    }
    /// <summary>
    /// Create XML text for an XMLObject.
    /// </summary>
    /// <param name="xmlObject">XML object to create text for.</param>
    /// <returns>XML text for XMLObject.</returns>
    void XML::stringify(XMLObject &xmlObject, IDestination &xmlDestination)
    {
        stringifyXML(&xmlObject.prolog, xmlDestination);
    }
} // namespace H4
