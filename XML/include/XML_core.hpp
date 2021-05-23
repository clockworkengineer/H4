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
    using XString = std::u32string;
    using XChar = XString::value_type;
    //
    // XML value
    //
    struct XValue
    {
    public:
        std::string unparsed;
        std::string parsed;
        XValue operator+=(const XValue &rhs)
        {
            parsed += rhs.parsed;
            unparsed += rhs.unparsed;
            return (*this);
        }
    };
    //
    // XML Attribute
    //
    struct XAttribute
    {
    public:
        XAttribute() {}
        XAttribute(const std::string &name, const XValue &value) : name(name), value(value)
        {
        }
        std::string name;
        XValue value;
    };
    //
    // XML External reference
    //
    struct XExternalReference
    {
    public:
        std::string type;
        std::string systemID;
        std::string publicID;
    };
    //
    // XML Entity mapping
    //
    struct XEntityMapping
    {
    public:
        std::string internal;
        XExternalReference external;
        std::string notation;
    };
    using XEntityMappings = std::unordered_map<std::string, XEntityMapping>;
    //
    // XML validation
    //
    bool validChar(XChar ch);
    bool validNameStartChar(XChar ch);
    bool validNameChar(XChar ch);
    bool validReservedName(const XString &name);
    bool validName(XString name);
    //
    // XML parsing
    //
    class ISource;
    XValue parseEntityReference(ISource &xmlSource);
    std::string parseName(ISource &xmlSource);
    XValue parseCharacterReference(ISource &xmlSource);
    XValue parseCharacter(ISource &xmlSource, XEntityMappings &entityMapping, bool translateEntity = true);
    XValue parseValue(ISource &xmlSource, XEntityMappings &entityMapping, bool translateEntity = true);
    //
    // XML entity
    //
    void mapEntityReference(XValue &entityReference, XEntityMappings &entityMapping);
} // namespace H4
#endif /* XML_CORE_HPP */
