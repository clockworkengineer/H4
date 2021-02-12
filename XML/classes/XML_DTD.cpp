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
                throw XML::SyntaxError();
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
        while (source.current() != '>')
        {
            XDTDAttribute xDTDAttribute;
            xDTDAttribute.name = m_UTF8.to_bytes(parseName(source));
            xDTDAttribute.type = m_UTF8.to_bytes(parseDTDAttributeType(source));
            xDTDAttribute.value = m_UTF8.to_bytes(parseDTDAttributeValue(source));
            source.ignoreWS();
            xNodeDTD->elements[elementName].attributes.emplace_back(xDTDAttribute);
        }
        source.ignoreWS();
    }
    void XML::parseDTDEntity(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        XString entityName = parseName(source);
        XString entityValue = parseValue(source);
        m_entityMapping[entityName] = entityValue;
        xNodeDTD->entityMapping[entityName] = entityValue;
    }
    void XML::parseDTDElement(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        XString elementName = parseName(source);
        XString elementContent;
        if (source.current() == '(')
        {
            int bracketCount = 1;
            elementContent += source.current();
            source.next();
            while (source.more() && bracketCount != 0)
            {
                elementContent += source.current();
                source.next();
                if (source.current() == '(')
                {
                    bracketCount++;
                }
                else if (source.current() == ')')
                {
                    bracketCount--;
                }
            }
            elementContent += source.current();
            source.next();
        }
        else if (source.match(U"EMPTY"))
        {
            elementContent = U"EMPTY";
        }
        else if (source.match(U"ANY"))
        {
            elementContent = U"ANY";
        }
        else
        {
            throw XML::SyntaxError();
        }
        source.ignoreWS();
        XDTDElement element(m_UTF8.to_bytes(elementName), m_UTF8.to_bytes(elementContent));
        xNodeDTD->elements.emplace(std::pair(element.name, element));
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
        if (source.current() != '>')
        {
            throw XML::SyntaxError();
        }
        source.next();
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
            if (source.current() != '>')
            {
                throw XML::SyntaxError();
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
        source.ignoreWS();
        xNodeElement->elements.emplace_back(std::make_unique<XNodeDTD>(xNodeDTD));
    }
} // namespace H4
