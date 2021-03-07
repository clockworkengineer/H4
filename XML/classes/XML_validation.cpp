//
// Class: XML
//
// Description:
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
    XAttribute XML::m_defaultAtributes[3] = {{"version", "1.0"}, {"encoding", "UTF-8"}, {"standalone", "no"}};
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    bool XML::validChar(XChar ch)
    {
        return ((ch == 0x09) ||
                (ch == kLineFeed) ||
                (ch == kCarriageReturn) ||
                (ch >= 0x20 && ch <= 0xD7FF) ||
                (ch >= 0xE000 && ch <= 0xFFFD) ||
                (ch >= 0x10000 && ch <= 0x10FFFF));
    }
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
    bool XML::validReservedName(const XString &name)
    {
        return (name.starts_with(U"xmlns") || name.starts_with(U"xml-stylesheet") || name == U"xml");
    }
    bool XML::validateName(XString name)
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
    bool XML::validateXMLDeclaration(XNodeElement *xNodeElement)
    {
        // Syntax error if no version present
        if (!isAttributePresent(xNodeElement->attributes, "version"))
        {
            return (false);
        }
        // Fill in gaps with default if missing attributes
        std::vector<XAttribute> validatedAttributes;
        long currentAttribute = 0;
        for (auto attrIndex = 0; attrIndex < 3; attrIndex++)
        {
            if ((currentAttribute < (int)xNodeElement->attributes.size()) &&
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
        if (currentAttribute != (long)xNodeElement->attributes.size())
        {
            return (false);
        }
        // Encoding all upper case
        std::transform(validatedAttributes[1].value.begin(), validatedAttributes[1].value.end(),
                       validatedAttributes[1].value.begin(), [](unsigned int c) { return std::toupper(c); });
        // Check valid declaration values
        if ((validatedAttributes[0].value != "1.0") ||
            ((validatedAttributes[1].value != "UTF-8") && (validatedAttributes[1].value != "UTF-16")) ||
            ((validatedAttributes[2].value) != "yes" && (validatedAttributes[2].value != "no")))
        {
            return (false);
        }
        xNodeElement->attributes = validatedAttributes;
        return (true);
    }
} // namespace H4
