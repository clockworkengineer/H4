#ifndef XML_CORE_HPP
#define XML_CORE_HPP
//
// C++ STL
//
#include <string>
//
// Source/Destination interfaces
//
// #include "ISource.hpp"
// // #include "IDestination.hpp"
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
    };
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

} // namespace H4
#endif /* XML_CORE_HPP */
