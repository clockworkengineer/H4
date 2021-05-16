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
    /// Check whether a character is valid.
    /// </summary>
    /// <param name="ch">Character to validate</param>
    /// <returns>true then valid/.</returns>
    bool XML::validChar(XChar ch)
    {
        return ((ch == 0x09) ||
                (ch == kLineFeed) ||
                (ch == kCarriageReturn) ||
                (ch >= 0x20 && ch <= 0xD7FF) ||
                (ch >= 0xE000 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0x10FFFF));
    }
    /// <summary>
    /// Check whether character is a valid to start a name with.
    /// </summary>
    /// <param name="ch">Character value to validate.</param>
    /// <returns>true then valid.</returns>
    bool XML::validNameStartChar(XChar ch)
    {
        return ((ch == ':') ||
                (ch == '_') ||
                (ch >= 'A' && ch <= 'Z') ||
                (ch >= 'a' && ch <= 'z') ||
                (ch >= 0xC0 && ch <= 0xD6) ||
                (ch >= 0xD8 && ch <= 0xF6) ||
                (ch >= 0xF8 && ch <= 0x2FF) ||
                (ch >= 0x370 && ch <= 0x37D) ||
                (ch >= 0x37F && ch <= 0x1FFF) ||
                (ch >= 0x200C && ch <= 0x200D) ||
                (ch >= 0x2070 && ch <= 0x218F) ||
                (ch >= 0x2C00 && ch <= 0x2FEF) ||
                (ch >= 0x3001 && ch <= 0xD7FF) ||
                (ch >= 0xF900 && ch <= 0xFDCF) ||
                (ch >= 0xFDF0 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0xEFFFF));
    }
    /// <summary>
    /// Check whether a name has all valid characters.
    /// </summary>
    /// <param name="ch">Name to validate.</param>
    /// <returns>true then valid.</returns>
    bool XML::validNameChar(XChar ch)
    {
        return (validNameStartChar(ch) ||
                (ch == '-') ||
                (ch == '.') ||
                (ch >= '0' && ch <= '9') ||
                (ch == 0xB7) ||
                (ch >= 0x0300 && ch <= 0x036F) ||
                (ch >= 0x203F && ch <= 0x2040));
    }
    /// <summary>
    /// Check name that starts with xml is a valid reserved name.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    bool XML::validReservedName(const XString &name)
    {
        return (name.starts_with(U"xmlns") || name.starts_with(U"xml-stylesheet") || name == U"xml");
    }
    /// <summary>
    /// Validate XMl tag/ attribute names.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    bool XML::validName(XString name)
    {
        if (name.empty())
        {
            return (false);
        }
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if (name.starts_with(U"xml") && !(validReservedName(name)))
        {
            return (false);
        }
        if (!validNameStartChar(name[0]))
        {
            return (false);
        }
        for (auto it = name.begin() + 1; it != name.end(); it++)
        {
            if (!validNameChar(*it))
            {
                return (false);
            }
        }
        return (true);
    }
    /// <summary>
    /// Validate XMl declaration. This includes that it has at least a version and that
    /// its atributes version, encoding and standalone occur in the correct order.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>true when declaration valid.</returns>
    void XML::validXMLDeclaration(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        // Syntax error if no version present
        if (!xNodeElement->isAttributePresent("version"))
        {
            throw SyntaxError(xmlSource, "Version missing from declaration.");
        }
        // Fill in gaps with default if missing attributes
        std::vector<XAttribute> validatedAttributes;
        long currentAttribute = 0;
        for (auto attrIndex = 0; attrIndex < 3; attrIndex++)
        {
            if ((currentAttribute < (int)xNodeElement->getAttributeList().size()) &&
                (xNodeElement->attributes[currentAttribute].name == XML::m_defaultAtributes[attrIndex].name))
            {
                validatedAttributes.push_back(xNodeElement->attributes[currentAttribute]);
                currentAttribute++;
            }
            else
            {
                validatedAttributes.push_back(XML::m_defaultAtributes[attrIndex]);
            }
        }
        // Order not version, encoding, standalone == syntax error
        if (currentAttribute != (long)xNodeElement->getAttributeList().size())
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
        xNodeElement->attributes = validatedAttributes;
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
                xmlParseReferenceOrEntity(valueSoure);
            }
            else if ((valueSoure.current() == '<')||
                     (valueSoure.current() == '"')||
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
