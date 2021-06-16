//
// Class: XML
//
// Description: Parse XML into an internal XML Object (generating an exception
// if it is found not to be syntactically correct) so that it can be manipulated,
// interrogated and saved back to a text form (stringify). For more
// information on the XML standard check out https://www.w3.org/TR/REC-xml/.
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
    // ==============
    // PUBLIC METHODS
    // ==============
    DTD::DTDElement &XML::getElement(const std::string &elementName)
    {
        return(m_dtd.m_elements[elementName]);
    }
    /// <summary>
    /// Fetch entity mapping structure.
    /// </summary>
    /// <param name="entityName">Entity name.</param>
    /// <returns>Entity structure.</returns>
    XMLEntityMapping &XML::getEntity(const std::string &entityName)
    {
        return (m_dtd.m_entityMapping[entityName]);
    }
    /// <summary>
    /// Parse XML read from source stream into internal object.
    /// </summary>
    void XML::parse()
    {
        parseXML();
    }
    /// <summary>
    /// Validate XML against any DTD provided to see whether it is well-formed. If an
    /// exception is thrown then there is a validation issue and the XML is not wellformed.
    /// </summary>
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
