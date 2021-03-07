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
    // DTD attribute list type tokens
    std::set<XString> XML::m_dtdAttrListTypes;
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    std::string XML::parseDTDValue(ISource &source)
    {
        auto value = parseValue(source);
        return(value.parsed);
    }
    std::string XML::parseDTDAttributeType(ISource &source)
    {
        XString type;
        for (auto attrType : XML::m_dtdAttrListTypes)
        {
            if (source.match(attrType))
            {
                type = attrType;
            }
        }
        if (type.empty())
        {
            if (source.current() == '(')
            {
                while (source.more() && source.current() != ')')
                {
                    type += source.current();
                    source.next();
                }
                type += source.current();
                source.next();
            }
            else
            {
                throw SyntaxError(source);
            }
        }
        source.ignoreWS();
        return (source.to_bytes(type));
    }
    std::string XML::parseDTDAttributeValue(ISource &source)
    {
        std::string value;
        if (source.match(U"#REQUIRED"))
        {
            value = "#REQUIRED";
        }
        else if (source.match(U"#IMPLIED"))
        {
            value = "#IMPLIED";
        }
        else if (source.match(U"#FIXED"))
        {
            value = "#FIXED ";
            value += parseDTDValue(source);
        }
        else
        {
            value = parseDTDValue(source);
        }
        return (value);
    }
    void XML::parseDTDAttributeList(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        std::string elementName = parseName(source);
        while (source.more() && source.current() != '>')
        {
            XDTDAttribute xDTDAttribute;
            xDTDAttribute.name = parseName(source);
            xDTDAttribute.type = parseDTDAttributeType(source);
            xDTDAttribute.value = parseDTDAttributeValue(source);
            xNodeDTD->elements[elementName].attributes.emplace_back(xDTDAttribute);
            source.ignoreWS();
        }
    }
    void XML::parseDTDNotation(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        XAttribute notation;
        std::string name = parseName(source);
        notation.name = parseName(source);
        while (source.more() && source.current() != '>')
        {
            notation.value.parsed += source.to_bytes(source.current());
            source.next();
        }
        xNodeDTD->notations[name] = notation;
        source.ignoreWS();
    }
    void XML::parseDTDEntity(ISource &source, XNodeDTD *xNodeDTD)
    {
        std::string entityName = "&";
        source.ignoreWS();
        if (source.current() == '%')
        {
            entityName = "%";
            source.next();
            source.ignoreWS();
        }
        entityName += parseName(source) + ";";
        std::string entityValue = parseDTDValue(source);
        m_entityMapping[source.from_bytes(entityName)] = source.from_bytes(entityValue);
        xNodeDTD->entityMapping[entityName] = entityValue;
    }
    void XML::parseDTDElement(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        std::string elementName = parseName(source);
        XString elementContent;
        if (source.match(U"EMPTY"))
        {
            elementContent = U"EMPTY";
        }
        else if (source.match(U"ANY"))
        {
            elementContent = U"ANY";
        }
        else
        {
            while (source.more() && source.current() != '>')
            {
                elementContent += source.current();
                source.next();
            }
        }
        XDTDElement element(elementName, source.to_bytes(elementContent));
        xNodeDTD->elements.emplace(std::pair(element.name, element));
        source.ignoreWS();
    }
    void XML::parseDTDExternal(ISource &source, XNodeDTD *xNodeDTD)
    {
        if (source.match(U"SYSTEM"))
        {
            source.ignoreWS();
            xNodeDTD->external.name = "SYSTEM";
            xNodeDTD->external.value.parsed = parseDTDValue(source);
        }
        else if (source.match(U"PUBLIC"))
        {
            source.ignoreWS();
            xNodeDTD->external.name = "PUBLIC";
            xNodeDTD->external.value.parsed = parseDTDValue(source) + ", " + parseDTDValue(source);
        }
        else
        {
            throw SyntaxError(source);
        }
        if (source.current() != '>')
        {
            throw SyntaxError(source);
        }
        source.next();
        source.ignoreWS();
    }
    void XML::parseDTDInternal(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.next();
        source.ignoreWS();
        while (source.more() && !source.match(U"]>"))
        {
            if (source.match(U"<!ENTITY"))
            {
                parseDTDEntity(source, xNodeDTD);
            }
            else if (source.match(U"<!ELEMENT"))
            {
                parseDTDElement(source, xNodeDTD);
            }
            else if (source.match(U"<!ATTLIST"))
            {
                parseDTDAttributeList(source, xNodeDTD);
            }
            else if (source.match(U"<!NOTATION"))
            {
                parseDTDNotation(source, xNodeDTD);
            }
            else
            {
                throw SyntaxError(source);
            }
            if (source.current() != '>')
            {
                throw SyntaxError(source);
            }
            source.next();
            source.ignoreWS();
        }
    }
    void XML::parseDTD(ISource &source, XNodeElement *xNodeElement)
    {
        XNodeDTD xNodeDTD;
        source.ignoreWS();
        xNodeDTD.name = parseName(source);
        if (source.current() == '[')
        {
            parseDTDInternal(source, &xNodeDTD);
        }
        else
        {
            parseDTDExternal(source, &xNodeDTD);
        }
        xNodeElement->elements.emplace_back(std::make_unique<XNodeDTD>(xNodeDTD));
    }
} // namespace H4
