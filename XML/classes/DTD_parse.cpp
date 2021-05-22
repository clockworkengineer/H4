//
// Class: DTD
//
// Description: DTD Document Type Definition parsing.
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
#include "DTD.hpp"
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
    std::vector<XString> DTD::m_dtdAttrListTypes;
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    std::vector<std::string> DTD::split(std::string stringToSplit, char delimeter)
    {
        std::stringstream ss(stringToSplit);
        std::string item;
        std::vector<std::string> splittedStrings;
        while (std::getline(ss, item, delimeter))
        {
            splittedStrings.push_back(item);
        }
        return splittedStrings;
    }
    void DTD::checkForEntityRecursion(const std::string &entityName, std::set<std::string> names)
    {
        BufferSource entitySource(entityName);
        while (entitySource.more())
        {
            if ((entitySource.current() == '&') || (entitySource.current() == '%'))
            {
                std::string mappedEntityName = entitySource.current_to_bytes();
                entitySource.next();
                while (entitySource.more() && entitySource.current() != ';')
                {
                    mappedEntityName += entitySource.current();
                    entitySource.next();
                }
                if (mappedEntityName[1] != '#')
                {
                    mappedEntityName += entitySource.current();
                    if (names.contains(mappedEntityName))
                    {
                        throw XML::SyntaxError("Entity '" + mappedEntityName + "' contains recursive definition which is not allowed.");
                    }
                    if (!m_entityMapping[mappedEntityName].internal.empty())
                    {
                        names.emplace(mappedEntityName);
                        checkForEntityRecursion(m_entityMapping[mappedEntityName].internal, names);
                        names.erase(mappedEntityName);
                    }
                }
            }
            entitySource.next();
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::parseTranslateParameterENTITIES(ISource &dtdSource)
    {
        while (dtdSource.more())
        {
            std::string translated;
            if (dtdSource.match(U"<!ENTITY"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    dtdSource.next();
                }
                dtdSource.next();
                dtdSource.ignoreWS();
            }
            else if (dtdSource.match(U"<!ELEMENT"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    translated += dtdSource.current_to_bytes();
                    dtdSource.next();
                }
                translated += dtdSource.current_to_bytes();
                dtdSource.next();
                dtdSource.ignoreWS();
                translated = parseTranslateParameterEntities(translated);
                BufferSource dtdTranslatedSource(translated);
                parseElement(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!ATTLIST"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    translated += dtdSource.current_to_bytes();
                    dtdSource.next();
                }
                translated += dtdSource.current_to_bytes();
                dtdSource.next();
                dtdSource.ignoreWS();
                translated = parseTranslateParameterEntities(translated);
                BufferSource dtdTranslatedSource(translated);
                parseAttributeList(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!NOTATION"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    translated += dtdSource.current_to_bytes();
                    dtdSource.next();
                }
                translated += dtdSource.current_to_bytes();
                dtdSource.next();
                dtdSource.ignoreWS();
                translated = parseTranslateParameterEntities(translated);
                BufferSource dtdTranslatedSource(translated);
                parseNotation(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!--"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    dtdSource.next();
                }
                dtdSource.next();
                dtdSource.ignoreWS();
            }
            else if (dtdSource.match(U"%"))
            {
                parseParameterEntity(dtdSource);
                continue;
            }
            else
            {
                throw XML::SyntaxError(dtdSource, "Invalid DTD tag.");
            }
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::parseParameterENTITIES(ISource &dtdSource)
    {
        while (dtdSource.more())
        {
            if (dtdSource.match(U"<!ENTITY"))
            {
                parseEntity(dtdSource);
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    dtdSource.next();
                }
                dtdSource.next();
                dtdSource.ignoreWS();
            }
            else if (dtdSource.match(U"<!ELEMENT"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    dtdSource.next();
                }
                dtdSource.next();
                dtdSource.ignoreWS();
            }
            else if (dtdSource.match(U"<!ATTLIST"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    dtdSource.next();
                }
                dtdSource.next();
                dtdSource.ignoreWS();
            }
            else if (dtdSource.match(U"<!NOTATION"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    dtdSource.next();
                }
                dtdSource.next();
                dtdSource.ignoreWS();
            }
            else if (dtdSource.match(U"<!--"))
            {
                dtdSource.ignoreWS();
                while (dtdSource.more() && dtdSource.current() != '>')
                {
                    dtdSource.next();
                }
                dtdSource.next();
                dtdSource.ignoreWS();
            }
            else if (dtdSource.match(U"%"))
            {
                parseParameterEntity(dtdSource);
                continue;
            }
            else
            {
                throw XML::SyntaxError(dtdSource, "Invalid DTD tag.");
            }
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    std::string DTD::parseAttributeEnumerationType(ISource &dtdSource)
    {
        std::string attributeType(1, dtdSource.current());
        dtdSource.next();
        dtdSource.ignoreWS();
        attributeType += parseName(dtdSource);
        while (dtdSource.more() && dtdSource.current() == '|')
        {
            attributeType += dtdSource.current();
            dtdSource.next();
            dtdSource.ignoreWS();
            attributeType += parseName(dtdSource);
        }
        if (dtdSource.current() != ')')
        {
            throw XML::SyntaxError(dtdSource, "Invalid attribute type specified.");
        }
        attributeType += dtdSource.current();
        dtdSource.next();
        dtdSource.ignoreWS();
        return (attributeType);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    std::string DTD::parseTranslateParameterEntities(const std::string &parameterEntities)
    {
        std::string result = parameterEntities;
        while (result.find('%') != std::string::npos)
        {
            bool noMatch = true;
            for (auto entity : m_entityMapping)
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
                throw XML::SyntaxError("No match found for entity string '" + result + "'.");
            }
        }
        return (result);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    bool DTD::parseIsChoiceOrSequence(ISource &contentSpecSource)
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
    void DTD::parseElementCP(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        contentSpecSource.next();
        contentSpecSource.ignoreWS();
        if (validNameStartChar(contentSpecSource.current()))
        {
            parseElementName(contentSpecSource, contentSpecDestination);
        }
        else if (parseIsChoiceOrSequence(contentSpecSource))
        {
            parseElementChoice(contentSpecSource, contentSpecDestination);
        }
        else
        {
            parseElementSequence(contentSpecSource, contentSpecDestination);
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
    void DTD::parseElementChoice(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        if (contentSpecSource.current() != '(')
        {
            throw XML::SyntaxError("Invalid content region specification.");
        }
        contentSpecDestination.add("(");
        parseElementCP(contentSpecSource, contentSpecDestination);
        while (contentSpecSource.more() && contentSpecSource.current() == '|')
        {
            contentSpecDestination.add("|");
            parseElementCP(contentSpecSource, contentSpecDestination);
        }
        if (contentSpecSource.current() != ')')
        {
            throw XML::SyntaxError("Invalid content region specification.");
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
    void DTD::parseElementSequence(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        if (contentSpecSource.current() != '(')
        {
            throw XML::SyntaxError("Invalid content region specification.");
        }
        contentSpecDestination.add("(");
        parseElementCP(contentSpecSource, contentSpecDestination);
        while (contentSpecSource.more() && contentSpecSource.current() == ',')
        {
            parseElementCP(contentSpecSource, contentSpecDestination);
        }
        if (contentSpecSource.current() != ')')
        {
            throw XML::SyntaxError("Invalid content region specification.");
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
    void DTD::parseElementName(ISource &contentSpecSource, IDestination &contentSpecDestination)
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
    void DTD::parseElementChildren(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        if (contentSpecSource.current() == '(')
        {
            if (parseIsChoiceOrSequence(contentSpecSource))
            {
                parseElementChoice(contentSpecSource, contentSpecDestination);
            }
            else
            {
                parseElementSequence(contentSpecSource, contentSpecDestination);
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
            throw XML::SyntaxError("Invalid content region specification.");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::parseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpecDestination)
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
                    parseElementName(contentSpecSource, contentSpecDestination);
                }
                else
                {
                    throw XML::SyntaxError("Invalid content region specification.");
                }
            }
            if (contentSpecSource.current() != ')')
            {
                throw XML::SyntaxError("Invalid content region specification.");
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
                throw XML::SyntaxError("Invalid content region specification.");
            }
        }
        else if (contentSpecSource.current() == ')')
        {
            contentSpecDestination.add(")");
        }
        else
        {
            throw XML::SyntaxError("Invalid content region specification.");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::parseElementContentSpecification(XValue &contentSpec)
    {
        BufferSource contentSpecSource(contentSpec.unparsed);
        BufferDestination contentSpecDestination;
        contentSpecSource.ignoreWS();
        if (contentSpecSource.current() == '(')
        {
            contentSpecSource.next();
            contentSpecSource.ignoreWS();
            if (contentSpecSource.match(U"#PCDATA"))
            {
                parseElementMixedContent(contentSpecSource, contentSpecDestination);
            }
            else
            {
                contentSpecSource.backup(contentSpecSource.position());
                parseElementChildren(contentSpecSource, contentSpecDestination);
            }
        }
        else
        {
            throw XML::SyntaxError("Invalid content region specification.");
        }
        contentSpec.parsed = contentSpecDestination.getBuffer();
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::parsePostProcessing()
    {
        for (auto &element : m_elements)
        {
            if (element.second.content.parsed.empty())
            {
                try
                {
                    parseElementContentSpecification(element.second.content);
                }
                catch (XML::SyntaxError &e)
                {
                    if (e.what() == std::string("XML Syntax Error: Invalid content region specification."))
                    {
                        throw XML::SyntaxError("Invalid content region specification for element <" + element.second.name + ">.");
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
                                throw XML::SyntaxError("Enumerator value '" + option + "' for attribute '" + attribute.name + "' occurs more than once in its definition.");
                            }
                        }
                        if (!options.contains(attribute.value.parsed))
                        {
                            throw XML::SyntaxError("Default value '" + attribute.value.parsed + "' for enumeration attribute '" + attribute.name + "' is invalid.");
                        }
                    }
                    else if (attribute.type == "ID")
                    {
                        if (idAttributePresent)
                        {
                            throw XML::SyntaxError("Element <" + element.second.name + "> has more than one ID attribute.");
                        }
                        idAttributePresent = true;
                    }
                }
            }
        }
        for (auto &entityName : m_entityMapping)
        {
            std::set<std::string> names{entityName.first};
            checkForEntityRecursion(entityName.first);
        }
    }
    /// <summary>
    /// Parse externally defined DTD into DTD XNode.
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::parseExternalContents()
    {
        if (m_external.type == "SYSTEM")
        {
            FileSource dtdFile(m_external.systemID);
            parseParameterENTITIES(dtdFile);
            FileSource dtdFilePass2(m_external.systemID);
            parseTranslateParameterENTITIES(dtdFilePass2);
        }
        else if (m_external.type == "PUBLIC")
        {
            // Public m_external DTD currently not supported
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    XExternalReference DTD::parseExternalReference(ISource &dtdSource)
    {
        XExternalReference result;
        if (dtdSource.match(U"SYSTEM"))
        {
            dtdSource.ignoreWS();
            result.type = "SYSTEM";
            result.systemID = parseValue(dtdSource, m_entityMapping).parsed;
        }
        else if (dtdSource.match(U"PUBLIC"))
        {
            dtdSource.ignoreWS();
            result.type = "PUBLIC";
            result.publicID = parseValue(dtdSource, m_entityMapping).parsed;
            result.systemID = parseValue(dtdSource, m_entityMapping).parsed;
        }
        else
        {
            throw XML::SyntaxError(dtdSource, "Invalid m_external DTD specifier.");
        }
        return (result);
    }
    /// <summary>
    /// Parse DTD DTD attribute type field.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns>Attribute type as string (UTF-8 encoded).</returns>
    std::string DTD::parseAttributeType(ISource &dtdSource)
    {
        std::string attributeType;
        for (auto attrType : DTD::m_dtdAttrListTypes)
        {
            if (dtdSource.match(attrType))
            {
                dtdSource.ignoreWS();
                return (dtdSource.to_bytes(attrType));
            }
        }
        if (dtdSource.current() == '(')
        {
            return (parseAttributeEnumerationType(dtdSource));
        }
        throw XML::SyntaxError(dtdSource, "Invalid attribute type specified.");
    }
    /// <summary>
    /// Parse DTD DTD attribute value.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns>Attribute value as string (UTF-8 encoded).</returns>
    /// <returns></returns>
    XValue DTD::parseAttributeValue(ISource &dtdSource)
    {
        XValue value;
        if (dtdSource.match(U"#REQUIRED"))
        {
            value.parsed = "#REQUIRED";
            value.unparsed = "#REQUIRED";
        }
        else if (dtdSource.match(U"#IMPLIED"))
        {
            value.parsed = "#IMPLIED";
            value.unparsed = "#IMPLIED";
        }
        else if (dtdSource.match(U"#FIXED"))
        {
            dtdSource.ignoreWS();
            XValue fixedValue = parseValue(dtdSource, m_entityMapping);
            value.parsed = "#FIXED " + fixedValue.parsed;
            value.unparsed = "#FIXED " + fixedValue.unparsed;
        }
        else
        {
            dtdSource.ignoreWS();
            value = parseValue(dtdSource, m_entityMapping);
        }
        return (value);
    }
    /// <summary>
    /// Parse DTD DTD element attribute list.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseAttributeList(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        std::string elementName = parseName(dtdSource);
        while (dtdSource.more() && validNameStartChar(dtdSource.current()))
        {
            XDTDAttribute xDTDAttribute;
            xDTDAttribute.name = parseName(dtdSource);
            xDTDAttribute.type = parseAttributeType(dtdSource);
            xDTDAttribute.value = parseAttributeValue(dtdSource);
            if (xDTDAttribute.type == "ID" && xDTDAttribute.value.parsed.starts_with("#FIXED "))
            {
                throw XML::SyntaxError(dtdSource, "Attribute '" + xDTDAttribute.name + "' may not be of type ID and FIXED.");
            }
            m_elements[elementName].attributes.emplace_back(xDTDAttribute);
            dtdSource.ignoreWS();
        }
    }
    /// <summary>
    /// Parse DTD DTD notation.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseNotation(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        XAttribute notation;
        std::string name = parseName(dtdSource);
        m_notations[name] = parseExternalReference(dtdSource);
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse DTD DTD entity.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseEntity(ISource &dtdSource)
    {
        std::string entityName = "&";
        dtdSource.ignoreWS();
        if (dtdSource.current() == '%')
        {
            entityName = "%";
            dtdSource.next();
            dtdSource.ignoreWS();
        }
        entityName += parseName(dtdSource) + ";";
        if (dtdSource.current() == '\'' || dtdSource.current() == '"')
        {
            XValue entityValue = parseValue(dtdSource, m_entityMapping, false);
            m_entityMapping[entityName].internal = entityValue.parsed;
        }
        else
        {
            m_entityMapping[entityName].external = parseExternalReference(dtdSource);
            if (dtdSource.match(U"NDATA"))
            {
                dtdSource.ignoreWS();
                m_entityMapping[entityName].notation = parseName(dtdSource);
            }
        }
    }
    /// <summary>
    /// Parse an DTD DTD element.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseElement(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        std::string elementName = parseName(dtdSource);
        XValue contentSpecification;
        if (dtdSource.match(U"EMPTY"))
        {
            contentSpecification.unparsed = "EMPTY";
            contentSpecification.parsed = "EMPTY";
        }
        else if (dtdSource.match(U"ANY"))
        {
            contentSpecification.unparsed = "ANY";
            contentSpecification.parsed = "ANY";
        }
        else
        {
            while (dtdSource.more() &&
                   (dtdSource.current() != '<') &&
                   (dtdSource.current() != '>'))
            {
                contentSpecification.unparsed += dtdSource.current();
                dtdSource.next();
            }
        }
        XDTDElement element(elementName, contentSpecification);
        m_elements.emplace(std::pair(element.name, element));
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse m_externally defined DTD DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseExternal(ISource &dtdSource)
    {
        m_external = parseExternalReference(dtdSource);
        parseExternalContents();
        if (dtdSource.current() != '>')
        {
            throw XML::SyntaxError(dtdSource, "Missing '>' terminator.");
        }
        dtdSource.next();
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse DTD comment in DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseComment(ISource &dtdSource)
    {
        while (dtdSource.more() && !dtdSource.match(U"--"))
        {
            dtdSource.next();
        }
    }
    /// <summary>
    /// Parse DTD parameter in DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseParameterEntity(ISource &dtdSource)
    {
        std::string parameterEntity = "%";
        while (dtdSource.more() && dtdSource.current() != ';')
        {
            parameterEntity += dtdSource.current_to_bytes();
            dtdSource.next();
        }
        parameterEntity += dtdSource.current_to_bytes();
        BufferSource entitySource(parseTranslateParameterEntities(parameterEntity));
        parseInternal(entitySource);
        dtdSource.next();
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse internally defined DTD DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseInternal(ISource &dtdSource)
    {
        while (dtdSource.more() && !dtdSource.match(U"]>"))
        {
            if (dtdSource.match(U"<!ENTITY"))
            {
                parseEntity(dtdSource);
            }
            else if (dtdSource.match(U"<!ELEMENT"))
            {
                parseElement(dtdSource);
            }
            else if (dtdSource.match(U"<!ATTLIST"))
            {
                parseAttributeList(dtdSource);
            }
            else if (dtdSource.match(U"<!NOTATION"))
            {
                parseNotation(dtdSource);
            }
            else if (dtdSource.match(U"<!--"))
            {
                parseComment(dtdSource);
            }
            else if (dtdSource.match(U"%"))
            {
                parseParameterEntity(dtdSource);
                continue;
            }
            else
            {
                throw XML::SyntaxError(dtdSource, "Invalid DTD tag.");
            }
            if (dtdSource.current() != '>')
            {
                throw XML::SyntaxError(dtdSource, "Missing '>' terminator.");
            }
            dtdSource.next();
            dtdSource.ignoreWS();
        }
    }
    /// <summary>
    /// Parse DTD DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseDTD(ISource &dtdSource, XNodeElement *xNodeElement)
    {
        // We take the easy option for allowing a DTD to be stringifyed
        // and keeping the correct order for its components by storing it
        // in its raw unparsed form.
        XNodeDTD xNodeDTD;
        xNodeDTD.dtd = this;
        long start = dtdSource.position();
        dtdSource.ignoreWS();
        xNodeDTD.name = parseName(dtdSource);
        // TODO: External needs to be parsed after any internal that is there as
        // both can be used in an DTD file.
        if (dtdSource.current() == '[')
        {
            dtdSource.next();
            dtdSource.ignoreWS();
            parseInternal(dtdSource);
        }
        else
        {
            parseExternal(dtdSource);
        }
        // Save away unparsed form
        m_unparsed = "<!DOCTYPE" + dtdSource.getRange(start, dtdSource.position());
        for (auto ch : m_unparsed)
        {
            if (ch == kLineFeed)
            {
                m_lineNumber++;
            }
        }
        parsePostProcessing();
        xNodeElement->children.emplace_back(std::make_unique<XNodeDTD>(std::move(xNodeDTD)));
    }
} // namespace H4
