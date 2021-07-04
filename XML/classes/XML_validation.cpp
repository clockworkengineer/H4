//
// Class: XML
//
// Description: XML declaration and attribute validation.
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
    /// Validate XML declaration. This includes that it has at least a version and that
    /// its atributes version, encoding and standalone occur in the correct order.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>true when declaration valid.</returns>
    void XML::validDeclaration(ISource &xmlSource, XMLNodeElement *xmlNodeProlog)
    {
        // Encoding all upper case
        toUpperString(xmlNodeProlog->getAttribute("encoding").value.parsed);
        // Check valid declaration values
        std::set<std::string> validVersions{"1.0", "1.1"};
        if (validVersions.find(xmlNodeProlog->getAttribute("version").value.parsed) == validVersions.end())
        {
            throw XMLSyntaxError(xmlSource, "Unsupported version " + xmlNodeProlog->getAttribute("version").value.parsed + ".");
        }
        std::set<std::string> validEncodings{"UTF-8", "UTF-16"};
        if (validEncodings.find(xmlNodeProlog->getAttribute("encoding").value.parsed) == validEncodings.end())
        {
            throw XMLSyntaxError(xmlSource, "Unsupported encoding " + xmlNodeProlog->getAttribute("encoding").value.parsed + " specified.");
        }
        std::set<std::string> validStandalone{"yes", "no"};
        if (validStandalone.find(xmlNodeProlog->getAttribute("standalone").value.parsed) == validStandalone.end())
        {
            throw XMLSyntaxError(xmlSource, "Invalid standalone value of '" + xmlNodeProlog->getAttribute("standalone").value.parsed + "'.");
        }
    }
} // namespace H4
