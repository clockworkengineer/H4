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
        XString elementValue;
        if (source.current() == '(')
        {
            source.next();
            while (source.more() && source.current() != ')')
            {
                elementValue += source.current();
                source.next();
            }
            source.next();
        }
        else if (source.match(U"EMPTY"))
        {
            elementValue = U"EMPTY";
        }
        else if (source.match(U"ANY"))
        {
            elementValue = U"ANY";
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
        xNodeDTD->elements.emplace_back(m_toFromUTF8.to_bytes(elementName), m_toFromUTF8.to_bytes(elementValue));
    }
    void XML::parseDTDExternal(ISource &source, XNodeDTD *xNodeDTD)
    {
        if (source.match(U"SYSTEM"))
        {
            source.ignoreWS();
            xNodeDTD->elements.emplace_back(m_toFromUTF8.to_bytes(U"SYSTEM_FILE"), m_toFromUTF8.to_bytes(parseDTDValue(source)));
        }
        else if (source.match(U"PUBLIC"))
        {
            source.ignoreWS();
            xNodeDTD->elements.emplace_back(m_toFromUTF8.to_bytes(U"PUBLIC_PFI"), m_toFromUTF8.to_bytes(parseDTDValue(source)));
            xNodeDTD->elements.emplace_back(m_toFromUTF8.to_bytes(U"PUBLIC_URL"), m_toFromUTF8.to_bytes(parseDTDValue(source)));
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
        while (source.more() && !std::iswspace(source.current()))
        {
            xNodeDTD.name += source.current();
            source.next();
        }
        source.ignoreWS();
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
