//
// Class: XML
//
// Description: XML Document Type Definition parsing.
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
    void parseDTDElementContentSpecification(XValue &contents)
    {
        for (size_t index = 0;;)
        {
            if (contents.unparsed[index] == ',' || std::iswspace(contents.unparsed[index]))
            {
                index++;
                if (index == contents.unparsed.size())
                {
                    break;
                }
            }
            else if (contents.unparsed[index] == '#')
            {
                index++;
                contents.parsed += "(<#";
                while (std::isalnum(contents.unparsed[index]))
                {
                    contents.parsed += contents.unparsed[index++];
                    if (index == contents.unparsed.size())
                    {
                        contents.parsed += ">)";
                        break;
                    }
                }
                contents.parsed += ">)";
            }
            else if (std::isalpha(contents.unparsed[index]))
            {
                contents.parsed += "(<";
                while (std::isalnum(contents.unparsed[index]))
                {
                    contents.parsed += contents.unparsed[index++];
                    if (index == contents.unparsed.size())
                    {
                        contents.parsed += ">)";
                        break;
                    }
                }
                contents.parsed += ">)";
            }
            else
            {
                contents.parsed += contents.unparsed[index++];
                if (index == contents.unparsed.size())
                {
                    break;
                }
            }
        }
        // if (contents.parsed != "((<#PCDATA>))")
        // {
        //     if (contents.parsed.find("(<#PCDATA>)") != std::string::npos)
        //     {
        //         if (!contents.parsed.starts_with("((<#PCDATA>)") ||
        //             (contents.unparsed.find(',') != std::string::npos) ||
        //             (contents.unparsed.back() != '*'))
        //         {
        //             throw XML::ValidationError("Invalid mixed content specification.");
        //         }
        //     }
        // }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDPostProcessing(XNodeDTD *xNodeDTD)
    {
        for (auto &element : xNodeDTD->elements)
        {
            if (element.second.content.parsed.empty())
            {
                parseDTDElementContentSpecification(element.second.content);
            }
        }
    }
    /// <summary>
    /// Parse externally defined DTD into DTD XNode.
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDExternalContents(XNodeDTD *xNodeDTD)
    {
        if (xNodeDTD->external.name == "SYSTEM")
        {
            FileSource dtdFile(xNodeDTD->external.value.parsed);
            parseDTDInternal(dtdFile, xNodeDTD);
        }
        else if (xNodeDTD->external.name == "PUBLIC")
        {
            // Public external DTD currently not supported
        }
    }
    XAttribute XML::parseDTDExternalReference(ISource &xmlSource)
    {
        XAttribute result;
        if (xmlSource.match(U"SYSTEM"))
        {
            xmlSource.ignoreWS();
            result.name = "SYSTEM";
            result.value = parseValue(xmlSource);
        }
        else if (xmlSource.match(U"PUBLIC"))
        {
            xmlSource.ignoreWS();
            XValue publicValue = parseValue(xmlSource);
            XValue systemValue = parseValue(xmlSource);
            result.name = "PUBLIC";
            result.value.parsed = publicValue.parsed + ", " + systemValue.parsed;
            result.value.unparsed = publicValue.unparsed + U" " + systemValue.unparsed;
        }
        else
        {
            throw SyntaxError(xmlSource, "Invalid external DTD specifier.");
        }
        return (result);
    }
    /// <summary>
    /// Parse XML DTD attribute type field.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Attribute type as string (UTF-8 encoded).</returns>
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
    /// Parse XML DTD attribute value.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns>Attribute value as string (UTF-8 encoded).</returns>
    /// <returns></returns>
    XValue XML::parseDTDAttributeValue(ISource &xmlSource)
    {
        XValue value;
        if (xmlSource.match(U"#REQUIRED"))
        {
            value.parsed = "#REQUIRED";
            value.unparsed = U"#REQUIRED";
        }
        else if (xmlSource.match(U"#IMPLIED"))
        {
            value.parsed = "#IMPLIED";
            value.unparsed = U"#IMPLIED";
        }
        else if (xmlSource.match(U"#FIXED"))
        {
            XValue fixedValue = parseValue(xmlSource);
            value.parsed = "#FIXED " + fixedValue.parsed;
            value.unparsed = U"#FIXED " + fixedValue.unparsed;
        }
        else
        {
            value = parseValue(xmlSource);
        }
        return (value);
    }
    /// <summary>
    /// Parse XML DTD element attribute list.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDTDAttributeList(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        xmlSource.ignoreWS();
        std::string elementName = parseName(xmlSource);
        while (xmlSource.more() && validNameStartChar(xmlSource.current()))
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
    /// Parse XML DTD notation.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDTDNotation(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        xmlSource.ignoreWS();
        XAttribute notation;
        std::string name = parseName(xmlSource);
        xNodeDTD->notations[name] = parseDTDExternalReference(xmlSource);
        xmlSource.ignoreWS();
    }
    /// <summary>
    /// Parse XML DTD entity.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
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
        XValue entityValue = parseValue(xmlSource);
        m_entityMapping[xmlSource.from_bytes(entityName)] = xmlSource.from_bytes(entityValue.parsed);
        xNodeDTD->entityMapping[entityName] = entityValue.parsed;
    }
    /// <summary>
    /// Parse an XML DTD element.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDTDElement(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        xmlSource.ignoreWS();
        std::string elementName = parseName(xmlSource);
        XValue contentSpecification;
        if (xmlSource.match(U"EMPTY"))
        {
            contentSpecification.unparsed = U"EMPTY";
            contentSpecification.parsed = "EMPTY";
        }
        else if (xmlSource.match(U"ANY"))
        {
            contentSpecification.unparsed = U"ANY";
            contentSpecification.parsed = "ANY";
        }
        else
        {
            while (xmlSource.more() &&
                   (xmlSource.current() != '<') &&
                   (xmlSource.current() != '>'))
            {
                contentSpecification.unparsed += xmlSource.current();
                xmlSource.next();
            }
        }
        XDTDElement element(elementName, contentSpecification);
        xNodeDTD->elements.emplace(std::pair(element.name, element));
        xmlSource.ignoreWS();
    }
    /// <summary>
    /// Parse externally defined XML DTD.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDTDExternal(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        xNodeDTD->external = parseDTDExternalReference(xmlSource);
        parseDTDExternalContents(xNodeDTD);
        if (xmlSource.current() != '>')
        {
            throw SyntaxError(xmlSource, "Missing '>' terminator.");
        }
        xmlSource.next();
        xmlSource.ignoreWS();
    }
    /// <summary>
    /// Parse XML comment in DTD.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDTDComment(ISource &xmlSource, XNodeDTD * /*xNodeDTD*/)
    {
        while (xmlSource.more() && !xmlSource.match(U"--"))
        {
            xmlSource.next();
        }
    }
    /// <summary>
    /// Parse internally defined XML DTD.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDTDInternal(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
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
    /// Parse XML DTD.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
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
            xmlSource.next();
            xmlSource.ignoreWS();
            parseDTDInternal(xmlSource, &xNodeDTD);
        }
        else
        {
            parseDTDExternal(xmlSource, &xNodeDTD);
        }
        // Save away unparsed form
        xNodeDTD.unparsed = "<!DOCTYPE" + xmlSource.getRange(start, xmlSource.position());
        for (auto ch : xNodeDTD.unparsed)
        {
            if (ch == kLineFeed)
            {
                xNodeDTD.lineNumber++;
            }
        }
        parseDTDPostProcessing(&xNodeDTD);
        xNodeElement->elements.emplace_back(std::make_unique<XNodeDTD>(xNodeDTD));
    }
} // namespace H4
