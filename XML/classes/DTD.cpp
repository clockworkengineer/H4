//
// Class: DTD
//
// Description: Parse XML Data Type Declaration (DTD) portion of an XML file so
// that the XML maybe be validated against it and flagged as well-formed if it passes.
// For more information on the XML standard check out https://www.w3.org/TR/REC-xml/.
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
    /// <summary>
    /// Intialise internal tables used by  DTD parser.
    /// </summary>
    /// <param name=""></param>
    void DTD::initialiseTables()
    {
        // Default entity mappings
        m_entityMapping["&amp;"].internal = "&#x26;";
        m_entityMapping["&quot;"].internal = "&#x22;";
        m_entityMapping["&apos;"].internal = "&#x27;";
        m_entityMapping["&lt;"].internal = "&#x3C;";
        m_entityMapping["&gt;"].internal = "&#x3E;";
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    uint16_t DTD::getType()
    {
        return(m_type);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    std::string &DTD::getRootName()
    {
        return(m_name);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLExternalReference &DTD::getExternalReference()
    {
        return(m_external);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    bool DTD::isElementPresent(const std::string &elementName)
    {
        return (m_elements.find(elementName) != m_elements.end());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    DTDElement &DTD::getElement(const std::string &elementName)
    {
        return (m_elements[elementName]);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLEntityMapping &DTD::getEntity(const std::string &entityName)
    {
        return (m_entityMapping[entityName]);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    XMLExternalReference &DTD::getNotation(const std::string &notationName)
    {
        return (m_notations[notationName]);
    }
    /// <summary>
    /// Parse DTD read from source stream.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    void DTD::parse(ISource &dtdSource)
    {
        parseDTD(dtdSource);
    }
    /// <summary>
    /// Validate XML against its DTD. Throwing an exception if there is a
    /// issue and the XML is not well-formed.
    /// </summary>
    /// <param name="prolog">Prolog element containing root of XML to validate.</param>
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
