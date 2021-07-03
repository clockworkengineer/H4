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
    // Defaults for XML declaration
    XMLAttribute XML::m_defaultAtributes[3] = {{"version", {"1.0", "1.0"}}, {"encoding", {"UTF-8", "UTF-8"}}, {"standalone", {"no", "no"}}};
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
        // Syntax error if no version present
        if (!xmlNodeProlog->isAttributePresent("version"))
        {
            throw XMLSyntaxError(xmlSource, "Version missing from declaration.");
        }
        // Save declaration attributes to be validated
        std::vector<XMLAttribute> prologAttributes{xmlNodeProlog->getAttributeList()};
        xmlNodeProlog->clearAttributes();
        long currentAttribute = 0;
        for (auto attrIndex = 0; attrIndex < 3; attrIndex++)
        {
            if ((currentAttribute < (int)prologAttributes.size()) &&
                (prologAttributes[currentAttribute].name == XML::m_defaultAtributes[attrIndex].name))
            {
                xmlNodeProlog->addAttribute(prologAttributes[currentAttribute].name, prologAttributes[currentAttribute].value);
                currentAttribute++;
            }
            else
            {
                xmlNodeProlog->addAttribute(XML::m_defaultAtributes[attrIndex].name, XML::m_defaultAtributes[attrIndex].value);
            }
        }
        // Order not version, encoding, standalone == syntax error
        if (currentAttribute != (long)prologAttributes.size())
        {
            throw XMLSyntaxError(xmlSource, "Incorrect order for version, encoding and standalone attributes.");
        }
        // Encoding all upper case
        std::transform(xmlNodeProlog->getAttribute("encoding").value.parsed.begin(), xmlNodeProlog->getAttribute("encoding").value.parsed.end(),
                       xmlNodeProlog->getAttribute("encoding").value.parsed.begin(), [](unsigned int c)
                       { return std::toupper(c); });
        // Check valid declaration values
        std::set<std::string> versions{"1.0", "1.1"};
        if (versions.find(xmlNodeProlog->getAttribute("version").value.parsed) == versions.end())
        {
            throw XMLSyntaxError(xmlSource, "Unsupported version number " + xmlNodeProlog->getAttribute("version").value.parsed + ".");
        }
        std::set<std::string> encoding{"UTF-8", "UTF-16"};
        if (encoding.find(xmlNodeProlog->getAttribute("encoding").value.parsed) == encoding.end())
        {
            throw XMLSyntaxError(xmlSource, "Unsupported encoding " + xmlNodeProlog->getAttribute("encoding").value.parsed + " specified.");
        }
        std::set<std::string> standalone{"yes", "no"};
        if (standalone.find(xmlNodeProlog->getAttribute("standalone").value.parsed) == standalone.end())
        {
            throw XMLSyntaxError(xmlSource, "Invalid standalone value of '" + xmlNodeProlog->getAttribute("standalone").value.parsed + "'.");
        }
    }
} // namespace H4
