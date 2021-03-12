//
// Class: XML
//
// Description: XML Document Type Definition parser.
//
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
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    std::string XML::parseDTDValue(ISource &xmlSource)
    {
        auto value = parseValue(xmlSource);
        return (value.parsed);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    std::string XML::parseDTDAttributeType(ISource &xmlSource)
    {
        XString type;
        for (auto attrType : XML::m_dtdAttrListTypes)
        {
            if (xmlSource.match(attrType))
            {
                type = attrType;
            }
        }
        if (type.empty())
        {
            if (xmlSource.current() == '(')
            {
                while (xmlSource.more() && xmlSource.current() != ')')
                {
                    type += xmlSource.current();
                    xmlSource.next();
                }
                type += xmlSource.current();
                xmlSource.next();
            }
            else
            {
                throw SyntaxError(xmlSource, "Invalid attribute type specified.");
            }
        }
        xmlSource.ignoreWS();
        return (xmlSource.to_bytes(type));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    std::string XML::parseDTDAttributeValue(ISource &xmlSource)
    {
        std::string value;
        if (xmlSource.match(U"#REQUIRED"))
        {
            value = "#REQUIRED";
        }
        else if (xmlSource.match(U"#IMPLIED"))
        {
            value = "#IMPLIED";
        }
        else if (xmlSource.match(U"#FIXED"))
        {
            value = "#FIXED ";
            value += parseDTDValue(xmlSource);
        }
        else
        {
            value = parseDTDValue(xmlSource);
        }
        return (value);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDAttributeList(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        xmlSource.ignoreWS();
        std::string elementName = parseName(xmlSource);
        while (xmlSource.more() && xmlSource.current() != '>')
        {
            XDTDAttribute xDTDAttribute;
            xDTDAttribute.name = parseName(xmlSource);
            xDTDAttribute.type = parseDTDAttributeType(xmlSource);
            xDTDAttribute.value = parseDTDAttributeValue(xmlSource);
            xNodeDTD->elements[elementName].attributes.emplace_back(xDTDAttribute);
            xmlSource.ignoreWS();
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDNotation(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        xmlSource.ignoreWS();
        XAttribute notation;
        std::string name = parseName(xmlSource);
        notation.name = parseName(xmlSource);
        while (xmlSource.more() && xmlSource.current() != '>')
        {
            notation.value.parsed += xmlSource.to_bytes(xmlSource.current());
            xmlSource.next();
        }
        xNodeDTD->notations[name] = notation;
        xmlSource.ignoreWS();
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDEntity(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        std::string entityName = "&";
        xmlSource.ignoreWS();
        if (xmlSource.current() == '%')
        {
            entityName = "%";
            xmlSource.next();
            xmlSource.ignoreWS();
        }
        entityName += parseName(xmlSource) + ";";
        std::string entityValue = parseDTDValue(xmlSource);
        m_entityMapping[xmlSource.from_bytes(entityName)] = xmlSource.from_bytes(entityValue);
        xNodeDTD->entityMapping[entityName] = entityValue;
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElement(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        xmlSource.ignoreWS();
        std::string elementName = parseName(xmlSource);
        XString elementContent;
        if (xmlSource.match(U"EMPTY"))
        {
            elementContent = U"EMPTY";
        }
        else if (xmlSource.match(U"ANY"))
        {
            elementContent = U"ANY";
        }
        else
        {
            while (xmlSource.more() && xmlSource.current() != '>')
            {
                elementContent += xmlSource.current();
                xmlSource.next();
            }
        }
        XDTDElement element(elementName, xmlSource.to_bytes(elementContent));
        xNodeDTD->elements.emplace(std::pair(element.name, element));
        xmlSource.ignoreWS();
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDExternal(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        if (xmlSource.match(U"SYSTEM"))
        {
            xmlSource.ignoreWS();
            xNodeDTD->external.name = "SYSTEM";
            xNodeDTD->external.value.parsed = parseDTDValue(xmlSource);
        }
        else if (xmlSource.match(U"PUBLIC"))
        {
            xmlSource.ignoreWS();
            xNodeDTD->external.name = "PUBLIC";
            xNodeDTD->external.value.parsed = parseDTDValue(xmlSource) + ", " + parseDTDValue(xmlSource);
        }
        else
        {
            throw SyntaxError(xmlSource, "Invalid external DTD specifier.");
        }
        if (xmlSource.current() != '>')
        {
            throw SyntaxError(xmlSource, "Missing terminator '>'.");
        }
        xmlSource.next();
        xmlSource.ignoreWS();
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDComment(ISource &xmlSource, XNodeDTD * /*xNodeDTD*/)
    {
        while (xmlSource.more() && !xmlSource.match(U"--"))
        {
            xmlSource.next();
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDInternal(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        xmlSource.next();
        xmlSource.ignoreWS();
        while (xmlSource.more() && !xmlSource.match(U"]>"))
        {
            if (xmlSource.match(U"<!ENTITY"))
            {
                parseDTDEntity(xmlSource, xNodeDTD);
            }
            else if (xmlSource.match(U"<!ELEMENT"))
            {
                parseDTDElement(xmlSource, xNodeDTD);
            }
            else if (xmlSource.match(U"<!ATTLIST"))
            {
                parseDTDAttributeList(xmlSource, xNodeDTD);
            }
            else if (xmlSource.match(U"<!NOTATION"))
            {
                parseDTDNotation(xmlSource, xNodeDTD);
            }
            else if (xmlSource.match(U"<--"))
            {
                parseDTDComment(xmlSource, xNodeDTD);
            }
            else
            {
                throw SyntaxError(xmlSource, "Invalid DTD tag.");
            }
            if (xmlSource.current() != '>')
            {
                throw SyntaxError(xmlSource, "Missing '>' terminator.");
            }
            xmlSource.next();
            xmlSource.ignoreWS();
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTD(ISource &xmlSource, XNodeElement *xNodeElement)
    {
        // We take the easy option for allowing a DTD to be stringifyed
        // and keeping the correct order for its components by storing it
        // in its raw unparsed form.
        XNodeDTD xNodeDTD;
        long start = xmlSource.position();
        xmlSource.ignoreWS();
        xNodeDTD.name = parseName(xmlSource);
        if (xmlSource.current() == '[')
        {
            parseDTDInternal(xmlSource, &xNodeDTD);
        }
        else
        {
            parseDTDExternal(xmlSource, &xNodeDTD);
        }
        // Save away unparsed form
        xNodeDTD.unparsed = "<!DOCTYPE" + xmlSource.getRange(start, xmlSource.position());
        xNodeElement->elements.emplace_back(std::make_unique<XNodeDTD>(xNodeDTD));
    }
} // namespace H4
