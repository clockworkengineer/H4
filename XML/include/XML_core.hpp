#ifndef XML_CORE_HPP
#define XML_CORE_HPP
//
// C++ STL
//
#include <filesystem>
#include <algorithm>
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
    // XML syntax error.
    //
    struct XMLSyntaxError : public std::exception
    {
    public:
        XMLSyntaxError(const std::string &description = "")
        {
            errorMessage = "XML Syntax Error: " + description;
        }
        XMLSyntaxError(ISource &xmlSource, const std::string &description = "")
        {
            errorMessage = "XML Syntax Error [Line: " + std::to_string(xmlSource.getLineNo()) +
                           " Column: " + std::to_string(xmlSource.getColumnNo()) + "] " + description;
        }
        virtual const char *what() const throw()
        {
            return (errorMessage.c_str());
        }

    private:
        std::string errorMessage;
    };
    //
    // XML validation error
    //
    struct XMLValidationError : public std::exception
    {
    public:
        XMLValidationError(long m_lineNumber, const std::string &description = "")
        {
            errorMessage = "XML Validation Error [Line: " + std::to_string(m_lineNumber) + "] " + description;
        }
        virtual const char *what() const throw()
        {
            return (errorMessage.c_str());
        }

    private:
        std::string errorMessage;
    };
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
    bool validChar(ISource::Char ch);
    bool validNameStartChar(ISource::Char ch);
    bool validNameChar(ISource::Char ch);
    bool validReservedName(const ISource::String &name);
    bool validName(ISource::String name);
    bool validAttributeValue(XMLValue &value);
    //
    // XML parsing
    //
    // class ISource;
    XMLValue parseEntityReference(ISource &xmlSource);
    std::string parseName(ISource &xmlSource);
    XMLValue parseCharacterReference(ISource &xmlSource);
    XMLValue parseCharacter(ISource &xmlSource, XMLEntityMappings &entityMapping, bool translateEntity = true);
    XMLValue parseValue(ISource &xmlSource, XMLEntityMappings &entityMapping, bool translateEntity = true);
    std::string extractTagBody(ISource &xmlSource);
    std::string translateEntities(const std::string &toTranslate, const XMLEntityMappings &entityMapping, char type = '%');
    std::vector<std::string> splitString(std::string stringToSplit, char delimeter);
    void trimmString(std::string &stringToTrimm);
    //
    // XML entity
    //
    void mapEntityReference(XMLValue &entityReference, XMLEntityMappings &entityMapping);
} // namespace H4
#endif /* XML_CORE_HPP */
