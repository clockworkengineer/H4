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
#include <sstream>
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
    std::vector<XString> XML::m_dtdAttrListTypes;
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    std::vector<std::string> XML::split(std::string strToSplit, char delimeter)
    {
        std::stringstream ss(strToSplit);
        std::string item;
        std::vector<std::string> splittedStrings;
        while (std::getline(ss, item, delimeter))
        {
            splittedStrings.push_back(item);
        }
        return splittedStrings;
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    std::string XML::parseDTDAttributeEnumerationType(ISource &xmlSource)
    {
        std::string attributeType(1, xmlSource.current());
        xmlSource.next();
        xmlSource.ignoreWS();
        attributeType += parseName(xmlSource);
        while (xmlSource.more() && xmlSource.current() == '|')
        {
            attributeType += xmlSource.current();
            xmlSource.next();
            xmlSource.ignoreWS();
            attributeType += parseName(xmlSource);
        }
        if (xmlSource.current() != ')')
        {
            throw SyntaxError(xmlSource, "Invalid attribute type specified.");
        }
        attributeType += xmlSource.current();
        xmlSource.next();
        xmlSource.ignoreWS();
        return (attributeType);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    std::string XML::parseDTDTranslateParameterEntities(XNodeDTD *xNodeDTD, const std::string &parameterEntities)
    {
        std::string result = parameterEntities;
        while (result.find('%') != std::string::npos)
        {
            bool noMatch = true;
            for (auto entity : xNodeDTD->entityMapping)
            {
                size_t pos = result.find(entity.first);
                if (pos != std::string::npos)
                {
                    result.replace(pos, entity.first.length(), entity.second.internal);
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
    bool XML::parseDTDIsChoiceOrSequence(ISource &contentSpecSource)
    {
        bool choice = false;
        long start = contentSpecSource.position();
        while (contentSpecSource.more() &&
               contentSpecSource.current() != '|' &&
               contentSpecSource.current() != ',')
        {
            contentSpecSource.next();
        }
        if (contentSpecSource.more() && contentSpecSource.current() == '|')
        {
            choice = true;
        }
        contentSpecSource.backup(contentSpecSource.position() - start);
        return (choice);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementCP(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        contentSpecSource.next();
        contentSpecSource.ignoreWS();
        if (validNameStartChar(contentSpecSource.current()))
        {
            parseDTDElementName(contentSpecSource, contentSpecDestination);
        }
        else if (parseDTDIsChoiceOrSequence(contentSpecSource))
        {
            parseDTDElementChoice(contentSpecSource, contentSpecDestination);
        }
        else
        {
            parseDTDElementSequence(contentSpecSource, contentSpecDestination);
        }
        if (contentSpecSource.current() == '*' ||
            contentSpecSource.current() == '+' ||
            contentSpecSource.current() == '?')
        {
            contentSpecDestination.add(contentSpecSource.current());
            contentSpecSource.next();
            contentSpecSource.ignoreWS();
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementChoice(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        if (contentSpecSource.current() != '(')
        {
            throw SyntaxError("Invalid content region specification.");
        }
        contentSpecDestination.add("(");
        parseDTDElementCP(contentSpecSource, contentSpecDestination);
        while (contentSpecSource.more() && contentSpecSource.current() == '|')
        {
            contentSpecDestination.add("|");
            parseDTDElementCP(contentSpecSource, contentSpecDestination);
        }
        if (contentSpecSource.current() != ')')
        {
            throw SyntaxError("Invalid content region specification.");
        }
        contentSpecDestination.add(")");
        contentSpecSource.next();
        contentSpecSource.ignoreWS();
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseDTDElementSequence(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        if (contentSpecSource.current() != '(')
        {
            throw SyntaxError("Invalid content region specification.");
        }
        contentSpecDestination.add("(");
        parseDTDElementCP(contentSpecSource, contentSpecDestination);
        while (contentSpecSource.more() && contentSpecSource.current() == ',')
        {
            parseDTDElementCP(contentSpecSource, contentSpecDestination);
        }
        if (contentSpecSource.current() != ')')
        {
            throw SyntaxError("Invalid content region specification.");
        }
        contentSpecDestination.add(")");
        contentSpecSource.next();
        contentSpecSource.ignoreWS();
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
    void XML::parseDTDElementChildren(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        if (contentSpecSource.current() == '(')
        {
            if (parseDTDIsChoiceOrSequence(contentSpecSource))
            {
                parseDTDElementChoice(contentSpecSource, contentSpecDestination);
            }
            else
            {
                parseDTDElementSequence(contentSpecSource, contentSpecDestination);
            }
            if (contentSpecSource.current() == '*' ||
                contentSpecSource.current() == '+' ||
                contentSpecSource.current() == '?')
            {
                contentSpecDestination.add(contentSpecSource.current());
                contentSpecSource.next();
                contentSpecSource.ignoreWS();
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
    void XML::parseDTDElementMixedContent(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        contentSpecSource.ignoreWS();
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
        else if (contentSpecSource.current() == ')')
        {
            contentSpecDestination.add(")");
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
    void XML::parseDTDElementContentSpecification(XNodeDTD * /*xNodeDTD*/, XValue &contentSpec)
    {
        //        BufferSource contentSpecSource(parseDTDTranslateContentSpecEntities(xNodeDTD, contentSpec));
        BufferSource contentSpecSource(contentSpec.unparsed);
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
                parseDTDElementChildren(contentSpecSource, contentSpecDestination);
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
            if (!element.second.attributes.empty())
            {
                bool idAttributePresent = false;
                for (auto &attribute : element.second.attributes)
                {
                    if (attribute.type[0] == '(')
                    {
                        std::set<std::string> options;
                        for (auto &option : split(attribute.type.substr(1, attribute.type.size() - 2), '|'))
                        {
                            if (!options.contains(option))
                            {
                                options.insert(option);
                            }
                            else
                            {
                                throw SyntaxError("Enumerator value '" + option + "' for attribute '" + attribute.name + "' occurs more than once in its definition.");
                            }
                        }
                        if (!options.contains(attribute.value.parsed))
                        {
                            throw SyntaxError("Default value '" + attribute.value.parsed + "' for enumeration attribute '" + attribute.name + "' is invalid.");
                        }
                    }
                    else if (attribute.type == "ID")
                    {
                        if (idAttributePresent)
                        {
                            throw SyntaxError("Element <" + element.second.name + "> has more than one ID attribute.");
                        }
                        idAttributePresent = true;
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
        if (xNodeDTD->external.type == "SYSTEM")
        {
            FileSource dtdFile(xNodeDTD->external.systemID);
            parseDTDInternal(dtdFile, xNodeDTD);
        }
        else if (xNodeDTD->external.type == "PUBLIC")
        {
            // Public external DTD currently not supported
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    XExternalReference XML::parseDTDExternalReference(ISource &xmlSource)
    {
        XExternalReference result;
        if (xmlSource.match(U"SYSTEM"))
        {
            xmlSource.ignoreWS();
            result.type = "SYSTEM";
            result.systemID = parseValue(xmlSource).parsed;
        }
        else if (xmlSource.match(U"PUBLIC"))
        {
            xmlSource.ignoreWS();
            result.type = "PUBLIC";
            result.publicID = parseValue(xmlSource).parsed;
            result.systemID = parseValue(xmlSource).parsed;
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
        std::string attributeType;
        for (auto attrType : XML::m_dtdAttrListTypes)
        {
            if (xmlSource.match(attrType))
            {
                xmlSource.ignoreWS();
                return (xmlSource.to_bytes(attrType));
            }
        }
        if (xmlSource.current() == '(')
        {
            return (parseDTDAttributeEnumerationType(xmlSource));
        }
        throw SyntaxError(xmlSource, "Invalid attribute type specified.");
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
            xmlSource.ignoreWS();
            XValue fixedValue = parseValue(xmlSource);
            value.parsed = "#FIXED " + fixedValue.parsed;
            value.unparsed = "#FIXED " + fixedValue.unparsed;
        }
        else
        {
            xmlSource.ignoreWS();
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
            if (xDTDAttribute.type == "ID" && xDTDAttribute.value.parsed.starts_with("#FIXED "))
            {
                throw SyntaxError(xmlSource, "Attribute '" + xDTDAttribute.name + "' may not be of type ID and FIXED.");
            }
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
        if (xmlSource.current() == '\'' || xmlSource.current() == '"')
        {
            XValue entityValue = parseValue(xmlSource);
            m_entityMapping[entityName].internal = entityValue.parsed;
            xNodeDTD->entityMapping[entityName].internal = entityValue.parsed;
        }
        else
        {
            m_entityMapping[entityName].external = parseDTDExternalReference(xmlSource);
            if (xmlSource.match(U"NDATA"))
            {
                xmlSource.ignoreWS();
                m_entityMapping[entityName].notation = parseName(xmlSource);
            }
        }
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
    /// Parse XML parameter in DTD.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDTDParameterEntity(ISource &xmlSource, XNodeDTD *xNodeDTD)
    {
        std::string parameterEntity = "%";
        while (xmlSource.more() && xmlSource.current() != ';')
        {
            parameterEntity += xmlSource.to_bytes(xmlSource.current());
            xmlSource.next();
        }
        parameterEntity += xmlSource.to_bytes(xmlSource.current());
        BufferSource entitySource(parseDTDTranslateParameterEntities(xNodeDTD, parameterEntity));
        parseDTDInternal(entitySource, xNodeDTD);
        xmlSource.next();
        xmlSource.ignoreWS();
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
            else if (xmlSource.match(U"%"))
            {
                parseDTDParameterEntity(xmlSource, xNodeDTD);
                continue;
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
