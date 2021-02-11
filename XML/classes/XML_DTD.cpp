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
    XString XML::parseDTDValue(ISource &source)
    {
        if ((source.current() == '\'') || ((source.current() == '"')))
        {
            XString attributeValue;
            XChar quote = source.current();
            source.next();
            while (source.more() && source.current() != quote)
            {
                attributeValue += parseEncodedCharacter(source);
            }
            source.next();
            source.ignoreWS();
            return (attributeValue);
        }
        throw XML::SyntaxError();
    }
    XString XML::parseDTDName(ISource &source)
    {
        XString dtdName = parseName(source);
        if (!validateName(dtdName))
        {
            throw XML::SyntaxError();
        }
        return (dtdName);
    }
    void XML::parseDTDAttributeList(ISource &source, XNodeDTD *xNodeDTD)
    {

        source.ignoreWS();
        std::string elementName = m_toFromUTF8.to_bytes(parseDTDName(source));
        while (source.current() != '>')
        {
            XDTDAttribute xDTDAttribute;
            xDTDAttribute.name = m_toFromUTF8.to_bytes(parseDTDName(source));
            if (source.match(U"CDATA"))
            {
                xDTDAttribute.type = "CDATA";
            }
            else if (source.match(U"IDREF"))
            {
                xDTDAttribute.type = "IDREF";
            }
            else if (source.match(U"IDREFS"))
            {
                xDTDAttribute.type = "IDREFS";
            }
            else if (source.match(U"NMTOKEN"))
            {
                xDTDAttribute.type = "NMTOKEN";
            }
            else if (source.match(U"NMTOKENS"))
            {
                xDTDAttribute.type = "NMTOKENS";
            }
            else if (source.match(U"ENTITY"))
            {
                xDTDAttribute.type = "ENTITY";
            }
            else if (source.match(U"ENTITIES"))
            {
                xDTDAttribute.type = "ENTITIES";
            }
            else if (source.match(U"NOTATION"))
            {
                xDTDAttribute.type = "NOTATION";
            }
            else if (source.match(U"ID"))
            {
                xDTDAttribute.type = "ID";
            }
            else if (source.current() == '(')
            {
                while (source.more() && source.current() != ')')
                {
                    xDTDAttribute.type += source.current();
                    source.next();
                }
                    xDTDAttribute.type += source.current();
                source.next();
            }
            else
            {
                throw XML::SyntaxError();
            }
            source.ignoreWS();
            if (source.match(U"#REQUIRED"))
            {
                xDTDAttribute.value = "#REQUIRED";
            }
            else if (source.match(U"#IMPLIED"))
            {
                xDTDAttribute.value = "#IMPLIED";
            }
            else if (source.match(U"#FIXED"))
            {
                xDTDAttribute.value = "#FIXED ";
                xDTDAttribute.value += m_toFromUTF8.to_bytes(parseDTDValue(source));
            }
            else
            {
                xDTDAttribute.value = m_toFromUTF8.to_bytes(parseDTDValue(source));
            }
            source.ignoreWS();
            xNodeDTD->elements[elementName].attributes.emplace_back(xDTDAttribute);
        }
        // if (source.current()!='>') {
        //     throw XML::SyntaxError();
        // }
        source.next();
        source.ignoreWS();
    }
    void XML::parseDTDEntity(ISource &source, XNodeDTD * /*xNodeDTD*/)
    {
        source.ignoreWS();
        XString entityName = parseDTDName(source);
        XString entityValue = parseDTDValue(source);
        if (source.current() == '>')
        {
            m_entityToCharacter[entityName] = entityValue;
            source.next();
            source.ignoreWS();
        }
        else
        {
            throw XML::SyntaxError();
        }
    }
    void XML::parseDTDElement(ISource &source, XNodeDTD *xNodeDTD)
    {
        source.ignoreWS();
        XString elementName = parseDTDName(source);
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
        if (source.current() != '>')
        {
            throw XML::SyntaxError();
        }
        source.next();
        source.ignoreWS();
        XDTDElement element;
        element.name = m_toFromUTF8.to_bytes(elementName);
        element.content = m_toFromUTF8.to_bytes(elementContent);
        xNodeDTD->elements[element.name].name = element.name;
        xNodeDTD->elements[element.name].content = element.content;
    }
    void XML::parseDTDExternal(ISource &source, XNodeDTD *xNodeDTD)
    {
        if (source.match(U"SYSTEM"))
        {
            source.ignoreWS();
            xNodeDTD->external.name = m_toFromUTF8.to_bytes(U"SYSTEM");
            xNodeDTD->external.value = m_toFromUTF8.to_bytes(parseDTDValue(source));
        }
        else if (source.match(U"PUBLIC"))
        {
            source.ignoreWS();
            xNodeDTD->external.name = m_toFromUTF8.to_bytes(U"PUBLIC");
            xNodeDTD->external.value = m_toFromUTF8.to_bytes(parseDTDValue(source)) + ", " + m_toFromUTF8.to_bytes(parseDTDValue(source));
        }
        else
        {
            throw XML::SyntaxError();
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
            else
            {
                throw XML::SyntaxError();
            }
            
        }
    }
    void XML::parseDTD(ISource &source, XNodeElement *xNodeElement)
    {
        XNodeDTD xNodeDTD;
        source.ignoreWS();
        xNodeDTD.name = m_toFromUTF8.to_bytes(parseDTDName(source));
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
