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
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Parse XML read from source stream.
    /// </summary>
    void XML::parse()
    {
        parseWML();
    }
    /// <summary>
    ///
    /// </summary>
    /// <returns></returns>
    void XML::validate()
    {
        m_dtd.validate(m_prolog);
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
