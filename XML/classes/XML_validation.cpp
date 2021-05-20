//
// Class: XML
//
// Description: XML chracter and field value validation.
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
    // Defaults for XML declaration
    XAttribute XML::m_defaultAtributes[3] = {{"version", {"1.0", "1.0"}}, {"encoding", {"UTF-8", "UTF-8"}}, {"standalone", {"no", "no"}}};
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    /// <summary>
    /// Validate XMl declaration. This includes that it has at least a version and that
    /// its atributes version, encoding and standalone occur in the correct order.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>true when declaration valid.</returns>
    void XML::validXMLDeclaration(ISource &xmlSource, XNodeElement *xNodeProlog)
    {
        // Syntax error if no version present
        if (!xNodeProlog->isAttributePresent("version"))
        {
            throw SyntaxError(xmlSource, "Version missing from declaration.");
        }
        // Save declaration attributes to be validated and clear tem from prolog element
        std::vector<XAttribute> prologAttributes;
        for (auto attribute : xNodeProlog->getAttributeList())
        {
            prologAttributes.push_back(attribute);
        }
        xNodeProlog->clearAttributes();
        // Fill in gaps with default if missing attributes
        std::vector<XAttribute> validatedAttributes;
        long currentAttribute = 0;
        for (auto attrIndex = 0; attrIndex < 3; attrIndex++)
        {
            if ((currentAttribute < (int)prologAttributes.size()) &&
                (prologAttributes[currentAttribute].name == XML::m_defaultAtributes[attrIndex].name))
            {
                validatedAttributes.push_back(prologAttributes[currentAttribute]);
                currentAttribute++;
            }
            else
            {
                validatedAttributes.push_back(XML::m_defaultAtributes[attrIndex]);
            }
        }
        // Order not version, encoding, standalone == syntax error
        if (currentAttribute != (long)prologAttributes.size())
        {
            throw SyntaxError(xmlSource, "Incorrect order for version, encoding and standalone attributes.");
        }
        // Encoding all upper case
        std::transform(validatedAttributes[1].value.parsed.begin(), validatedAttributes[1].value.parsed.end(),
                       validatedAttributes[1].value.parsed.begin(), [](unsigned int c)
                       { return std::toupper(c); });
        // Check valid declaration values
        std::set<std::string> versions{"1.0", "1.1"};
        if (!versions.contains(validatedAttributes[0].value.parsed))
        {
            throw SyntaxError(xmlSource, "Unsupported version number " + validatedAttributes[0].value.parsed + ".");
        }
        std::set<std::string> encoding{"UTF-8", "UTF-16"};
        if (!encoding.contains(validatedAttributes[1].value.parsed))
        {
            throw SyntaxError(xmlSource, "Unsupported encoding " + validatedAttributes[1].value.parsed + " specified.");
        }
        std::set<std::string> standalone{"yes", "no"};
        if (!standalone.contains(validatedAttributes[2].value.parsed))
        {
            throw SyntaxError(xmlSource, "Invalid standalone value of '" + validatedAttributes[2].value.parsed + "'.");
        }
        // Set validated prolog attributes
        for (auto &attribute : validatedAttributes)
        {
            xNodeProlog->addAttribute(attribute.name, attribute.value);
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns>true then valid.</returns>
    bool XML::validAttributeValue(XValue &value)
    {
        BufferSource valueSoure(value.parsed);
        while (valueSoure.more())
        {
            if (valueSoure.current() == '&')
            {
                parseEntityReference(valueSoure);
            }
            else if ((valueSoure.current() == '<') ||
                     (valueSoure.current() == '"') ||
                     (valueSoure.current() == '\''))
            {
                return (false);
            }
            else
            {
                valueSoure.next();
            }
        }
        return (true);
    }
} // namespace H4
