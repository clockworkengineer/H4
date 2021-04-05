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
    std::string XML::translateDTDContentSpecEntities(XNodeDTD *xNodeDTD, const XValue &contentSpec)
    {
        std::string result = contentSpec.unparsed;
        while (result.find('%') != std::string::npos)
        {
            bool noMatch = true;
            for (auto entity : xNodeDTD->entityMapping)
            {
                size_t pos = result.find(entity.first);
                if (pos != std::string::npos)
                {
                    result.replace(pos, entity.first.length(), entity.second);
                    noMatch = false;
                }
            }
            if (noMatch)
            {
                throw SyntaxError("No match found for entity string '" + result + "'.");
            }
        }
        return (result);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementChoice(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        while (contentSpecSource.more() && contentSpecSource.current() == '|')
        {
            contentSpecDestination.add("|");
            contentSpecSource.next();
            parseDTDElementChild(contentSpecSource, contentSpecDestination);
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementSequence(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        while (contentSpecSource.more() && contentSpecSource.current() == ',')
        {
            contentSpecSource.next();
            parseDTDElementChild(contentSpecSource, contentSpecDestination);
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementBracket(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        do
        {
            parseDTDElementChild(contentSpecSource, contentSpecDestination);
        } while (contentSpecSource.more() && contentSpecSource.current() != ')');
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementName(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        contentSpecDestination.add("(<");
        contentSpecDestination.add(contentSpecSource.current());
        contentSpecSource.next();
        while (contentSpecSource.more() && validNameChar(contentSpecSource.current()))
        {
            contentSpecDestination.add(contentSpecSource.current());
            contentSpecSource.next();
        }
        contentSpecDestination.add(">)");
        contentSpecSource.ignoreWS();
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementChild(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        contentSpecSource.ignoreWS();
        if (contentSpecSource.match(U"("))
        {
            contentSpecDestination.add("(");
            parseDTDElementBracket(contentSpecSource, contentSpecDestination);
            if (contentSpecSource.current() != ')')
            {
                throw SyntaxError("Invalid content region specification.");
            }
            contentSpecDestination.add(")");
            contentSpecSource.next();
            if (contentSpecSource.current() == '*' ||
                contentSpecSource.current() == '+' ||
                contentSpecSource.current() == '?')
            {
                contentSpecDestination.add(contentSpecSource.current());
                contentSpecSource.next();
            }
        }
        else if (contentSpecSource.current() == '|')
        {
            parseDTDElementChoice(contentSpecSource, contentSpecDestination);
        }
        else if (contentSpecSource.current() == ',')
        {
            parseDTDElementSequence(contentSpecSource, contentSpecDestination);
        }
        else if (validNameStartChar(contentSpecSource.current()))
        {
            parseDTDElementName(contentSpecSource, contentSpecDestination);
            if (contentSpecSource.current() == '*' ||
                contentSpecSource.current() == '+' ||
                contentSpecSource.current() == '?')
            {
                contentSpecDestination.add(contentSpecSource.current());
                contentSpecSource.next();
            }
        }
        else
        {
            throw SyntaxError("Invalid content region specification.");
        }
        contentSpecSource.ignoreWS();
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementMixedContent(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        contentSpecSource.ignoreWS();
        if (contentSpecSource.current() == ')')
        {
            contentSpecDestination.add("((<#PCDATA>))");
            return;
        }
        contentSpecDestination.add("((<#PCDATA>)");
        if (contentSpecSource.current() == '|')
        {
            while (contentSpecSource.more() && contentSpecSource.current() == '|')
            {
                contentSpecDestination.add("|");
                contentSpecSource.next();
                contentSpecSource.ignoreWS();
                if (validNameStartChar(contentSpecSource.current()))
                {
                    parseDTDElementName(contentSpecSource, contentSpecDestination);
                }
                else
                {
                    throw SyntaxError("Invalid content region specification.");
                }
            }
            if (contentSpecSource.current() != ')')
            {
                throw SyntaxError("Invalid content region specification.");
            }
            contentSpecDestination.add(")");
            contentSpecSource.next();
            contentSpecSource.ignoreWS();
            if (contentSpecSource.current() == '*')
            {
                contentSpecDestination.add("*");
                contentSpecDestination.add(contentSpecSource.current());
                contentSpecSource.next();
            }
            if (contentSpecSource.more() && !std::iswspace(contentSpecSource.current()))
            {
                throw SyntaxError("Invalid content region specification.");
            }
        }
        else
        {
            throw SyntaxError("Invalid content region specification.");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementContentSpecification(XNodeDTD *xNodeDTD, XValue &contentSpec)
    {
        BufferSource contentSpecSource(translateDTDContentSpecEntities(xNodeDTD, contentSpec));
        BufferDestination contentSpecDestination;
        contentSpecSource.ignoreWS();
        if (contentSpecSource.current() == '(')
        {
            contentSpecSource.next();
            contentSpecSource.ignoreWS();
            if (contentSpecSource.match(U"#PCDATA"))
            {
                parseDTDElementMixedContent(contentSpecSource, contentSpecDestination);
            }
            else
            {
                contentSpecSource.backup(contentSpecSource.position());
                parseDTDElementChild(contentSpecSource, contentSpecDestination);
            }
        }
        else
        {
            throw SyntaxError("Invalid content region specification.");
        }
        contentSpec.parsed = contentSpecDestination.getBuffer();
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
                try
                {
                    parseDTDElementContentSpecification(xNodeDTD, element.second.content);
                }
                catch (SyntaxError &e)
                {
                    if (e.what() == std::string("XML Syntax Error: Invalid content region specification."))
                    {
                        throw SyntaxError("Invalid content region specification for element <" + element.second.name + ">.");
                    }
                    else
                    {
                        throw;
                    }
                }
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
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
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
            result.value.unparsed = publicValue.unparsed + " " + systemValue.unparsed;
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
            value.unparsed = "#REQUIRED";
        }
        else if (xmlSource.match(U"#IMPLIED"))
        {
            value.parsed = "#IMPLIED";
            value.unparsed = "#IMPLIED";
        }
        else if (xmlSource.match(U"#FIXED"))
        {
            XValue fixedValue = parseValue(xmlSource);
            value.parsed = "#FIXED " + fixedValue.parsed;
            value.unparsed = "#FIXED " + fixedValue.unparsed;
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
            contentSpecification.unparsed = "EMPTY";
            contentSpecification.parsed = "EMPTY";
        }
        else if (xmlSource.match(U"ANY"))
        {
            contentSpecification.unparsed = "ANY";
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
