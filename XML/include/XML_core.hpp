#ifndef XML_CORE_HPP
#define XML_CORE_HPP
//
// C++ STL
//
#include <string>
//
// XML character constants
//
#define kCarriageReturn 0x0D
#define kLineFeed 0x0A
// =========
// NAMESPACE
// =========
namespace H4
{
    //
    // 32 bit characters internally
    //
    using XMLString = std::u32string;
    using XMLChar = XMLString::value_type;
    //
    // XML value
    //
    struct XMLValue
    {
    public:
        std::string unparsed;
        std::string parsed;
        XMLValue operator+=(const XMLValue &rhs)
        {
            parsed += rhs.parsed;
            unparsed += rhs.unparsed;
            return (*this);
        }
    };
    //
    // XML Attribute
    //
    struct XMLAttribute
    {
    public:
        XMLAttribute() {}
        XMLAttribute(const std::string &name, const XMLValue &value) : name(name), value(value)
        {
        }
        std::string name;
        XMLValue value;
    };
    //
    // XML External reference
    //
    struct XMLExternalReference
    {
    public:
        std::string type;
        std::string systemID;
        std::string publicID;
    };
    //
    // XML Entity mapping
    //
    struct XMLEntityMapping
    {
    public:
        std::string internal;
        XMLExternalReference external;
        std::string notation;
    };
    using XMLEntityMappings = std::unordered_map<std::string, XMLEntityMapping>;
    //
    // XML validation
    //
    bool validChar(XMLChar ch);
    bool validNameStartChar(XMLChar ch);
    bool validNameChar(XMLChar ch);
    bool validReservedName(const XMLString &name);
    bool validName(XMLString name);
    //
    // XML parsing
    //
    class ISource;
    XMLValue parseEntityReference(ISource &xmlSource);
    std::string parseName(ISource &xmlSource);
    XMLValue parseCharacterReference(ISource &xmlSource);
    XMLValue parseCharacter(ISource &xmlSource, XMLEntityMappings &entityMapping, bool translateEntity = true);
    XMLValue parseValue(ISource &xmlSource, XMLEntityMappings &entityMapping, bool translateEntity = true);
    //
    // XML entity
    //
    void mapEntityReference(XMLValue &entityReference, XMLEntityMappings &entityMapping);
} // namespace H4
#endif /* XML_CORE_HPP */
