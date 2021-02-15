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
    // Translation between UTF8/UTF16 and UTF32
    std::wstring_convert<std::codecvt_utf8_utf16<XString::value_type>, XString::value_type> XML::m_UTF8;
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> XML::m_UTF16;
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    void XML::initialiseTables()
    {
        // Default entity mappins
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
    bool XML::isAttributePresent(std::vector<XAttribute> attributes, const XString &name)
    {
        return (std::find_if(attributes.rbegin(), attributes.rend(),
                             [&name](const XAttribute &attr) { return (attr.name == XML::m_UTF8.to_bytes(name)); }) != attributes.rend());
    }

    std::string XML::convertCRLFToLF(const std::string &xmlString)
    {
        std::string converted = xmlString;
        size_t pos = converted.find("\x0D\x0A");
        while (pos != std::string::npos)
        {
            converted.replace(pos, 2, "\x0A");
            pos = converted.find("\x0D\x0A", pos + 1);
        }
        return (converted);
    }
    void XML::addNamespacesToList(XNodeElement *XNodeElement)
    {
        for (auto attribute : XNodeElement->attributes)
        {
            if (attribute.name.starts_with("xmlns"))
            {
                // Defining a namespace
                attribute.name = (attribute.name.size() > 5) ? attribute.name.substr(6) : ":";
                XNodeElement->namespaces.emplace_back(attribute);
            }
        }
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    std::unique_ptr<XNode> XML::parse(const std::string &xmlToParse)
    {
        BufferSource xml(XML::m_UTF8.from_bytes(convertCRLFToLF(xmlToParse)));
        return (parseXML(xml));
    }
    std::unique_ptr<XNode> XML::parse(const std::u16string &xmlToParse)
    {
        std::u16string utf16xml{xmlToParse};
        if (!xmlToParse.starts_with(u"<?xml"))
        {
            for (char16_t &ch : utf16xml)
            {
                ch = (static_cast<u_int16_t>(ch) >> 8) | (static_cast<u_int16_t>(ch) << 8);
            }
        }
        return (parse(XML::m_UTF16.to_bytes(utf16xml)));
    }
} // namespace H4
