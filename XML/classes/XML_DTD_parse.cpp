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
    XString XML::parseDTDAttributeType(ISource &source)
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
                throw SyntaxError();
            }
        }
        source.ignoreWS();
        return (type);
    }
    XString XML::parseDTDAttributeValue(ISource &source)
    {
        XString value;
        if (source.match(U"#REQUIRED"))
        {
            value = U"#REQUIRED";
        }
        else if (source.match(U"#IMPLIED"))
        {
            value = U"#IMPLIED";
        }
        else if (source.match(U"#FIXED"))
        {
            value = U"#FIXED ";
            value += parseValue(source);
        }
        else
        {
            value = parseValue(source);
        }
        return (value);
    }
    void XML::parseDTDAttributeList(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        std::string elementName = m_UTF8.to_bytes(parseName(source));
        while (source.more() && source.current() != '>')
        {
            XDTDAttribute xDTDAttribute;
            xDTDAttribute.name = m_UTF8.to_bytes(parseName(source));
            xDTDAttribute.type = m_UTF8.to_bytes(parseDTDAttributeType(source));
            xDTDAttribute.value = m_UTF8.to_bytes(parseDTDAttributeValue(source));
            xNodeDTD->elements[elementName].attributes.emplace_back(xDTDAttribute);
            source.ignoreWS();
        }
    }
    void XML::parseDTDNotation(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        XAttribute notation;
        std::string name = m_UTF8.to_bytes(parseName(source));
        notation.name = m_UTF8.to_bytes(parseName(source));
        while (source.more() && source.current() != '>')
        {
            notation.value += m_UTF8.to_bytes(source.current());
            source.next();
        }
        xNodeDTD->notations[name] = notation;
        source.ignoreWS();
    }
    void XML::parseDTDEntity(ISource &source, XNodeDTD *xNodeDTD)
    {
        std::string entity = "&";
        source.ignoreWS();
        if (source.current() == '%')
        {
            entity = "%";
            source.next();
            source.ignoreWS();
        }
        entity += m_UTF8.to_bytes(parseName(source)) + ";";
        XString entityValue = parseValue(source);
        m_entityMapping[m_UTF8.from_bytes(entity)] = entityValue;
        xNodeDTD->entityMapping[entity] = m_UTF8.to_bytes(entityValue);
    }
    void XML::parseDTDElement(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        XString elementName = parseName(source);
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
        XDTDElement element(m_UTF8.to_bytes(elementName), m_UTF8.to_bytes(elementContent));
        xNodeDTD->elements.emplace(std::pair(element.name, element));
        source.ignoreWS();
    }
    void XML::parseDTDExternal(ISource &source, XNodeDTD *xNodeDTD)
    {
        if (source.match(U"SYSTEM"))
        {
            source.ignoreWS();
            xNodeDTD->external.name = m_UTF8.to_bytes(U"SYSTEM");
            xNodeDTD->external.value = m_UTF8.to_bytes(parseValue(source));
        }
        else if (source.match(U"PUBLIC"))
        {
            source.ignoreWS();
            xNodeDTD->external.name = m_UTF8.to_bytes(U"PUBLIC");
            xNodeDTD->external.value = m_UTF8.to_bytes(parseValue(source)) + ", " + m_UTF8.to_bytes(parseValue(source));
        }
        else
        {
            throw SyntaxError();
        }
        if (source.current() != '>')
        {
            throw SyntaxError();
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
                throw SyntaxError();
            }
            if (source.current() != '>')
            {
                throw SyntaxError();
            }
            source.next();
            source.ignoreWS();
        }
    }
    void XML::parseDTD(ISource &source, XNodeElement *xNodeElement)
    {
        XNodeDTD xNodeDTD;
        source.ignoreWS();
        xNodeDTD.name = m_UTF8.to_bytes(parseName(source));
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
