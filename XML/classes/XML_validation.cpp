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
        XMLAttribute defaultAttributes[] {{"version", {"1.0", "1.0"}}, {"encoding", {"UTF-8", "UTF-8"}}, {"standalone", {"no", "no"}}};
        // Syntax error if no version present
        if (!xmlNodeProlog->isAttributePresent("version"))
        {
            throw XMLSyntaxError(xmlSource, "Version missing from declaration.");
        }
        // Save declaration attributes to be validated
        std::vector<XMLAttribute> declarationAttributes{xmlNodeProlog->getAttributeList()};
        xmlNodeProlog->clearAttributes();
        // Check attribute order
        long currentAttribute = 0;
        for (auto attrIndex = 0; attrIndex < 3; attrIndex++)
        {
            if ((currentAttribute < (int)declarationAttributes.size()) &&
                (declarationAttributes[currentAttribute].name == defaultAttributes[attrIndex].name))
            {
                xmlNodeProlog->addAttribute(declarationAttributes[currentAttribute].name, declarationAttributes[currentAttribute].value);
                currentAttribute++;
            }
            else
            {
                xmlNodeProlog->addAttribute(defaultAttributes[attrIndex].name, defaultAttributes[attrIndex].value);
            }
        }
        // Order not version, encoding, standalone == syntax error
        if (currentAttribute != (long)declarationAttributes.size())
        {
            throw XMLSyntaxError(xmlSource, "Incorrect order for version, encoding and standalone attributes.");
        }
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
