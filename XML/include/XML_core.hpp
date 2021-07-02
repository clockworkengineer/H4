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
    // XML syntax error
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
        bool isEntityReference()
        {
            return (unparsed[0] == '&' && unparsed[1] != '#' && unparsed.back() == ';');
        }
        bool isCharacterReference()
        {
            return (unparsed[0] == '&' && unparsed[1] == '#' && unparsed.back() == ';');
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
    //
    // XML Entity mapper
    //
    class XMLEntityMapper
    {
    public:
        XMLEntityMapper()
        {
            m_enityMappings["&amp;"].internal = "&#x26;";
            m_enityMappings["&quot;"].internal = "&#x22;";
            m_enityMappings["&apos;"].internal = "&#x27;";
            m_enityMappings["&lt;"].internal = "&#x3C;";
            m_enityMappings["&gt;"].internal = "&#x3E;";
        }
        void add(const std::string &entityName, XMLEntityMapping &entityMapping)
        {
            m_enityMappings[entityName] = entityMapping;
        }
        XMLEntityMapping &get(const std::string &entityName)
        {
            return (m_enityMappings[entityName]);
        }
        void remove(const std::string &entityName)
        {
            m_enityMappings.erase(entityName);
        }
        bool isPresent(const std::string &entityName)
        {
            return (m_enityMappings.count(entityName) != 0);
        }
        std::unordered_map<std::string, XMLEntityMapping> &getList()
        {
            return (m_enityMappings);
        }

    private:
        std::unordered_map<std::string, XMLEntityMapping> m_enityMappings;
    };
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
    XMLValue parseEntityReference(ISource &xmlSource);
    std::string parseName(ISource &xmlSource);
    XMLValue parseCharacterReference(ISource &xmlSource);
    XMLValue parseCharacter(ISource &xmlSource);
    XMLValue parseValue(ISource &xmlSource, XMLEntityMapper &entityMapper, bool translateEntity = true);
    std::string parseTagBody(ISource &xmlSource);
    //
    // XML entity
    //
    std::string translateEntities(const std::string &toTranslate, XMLEntityMapper &entityMapper, char type = '%');
    void mapEntityReference(XMLValue &entityReference, XMLEntityMapper &entityMapper);
    //
    // XML utility
    //
    std::vector<std::string> splitString(std::string stringToSplit, char delimeter);
    void trimmString(std::string &stringToTrimm);
} // namespace H4
#endif /* XML_CORE_HPP */
