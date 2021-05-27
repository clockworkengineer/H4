//
// Class: DTD
//
// Description: Parse XML Document Type Definition (DTD).
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
    std::vector<XMLString> DTD::m_dtdAttrListTypes;
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
    void DTD::parseValidateAttribute(const std::string &elementName, DTDAttribute xDTDAttribute)
    {
        if (xDTDAttribute.type == "ID" && xDTDAttribute.value.parsed.starts_with("#FIXED "))
        {
            throw XML::SyntaxError("Attribute '" + xDTDAttribute.name + "' may not be of type ID and FIXED.");
        }
        else if (xDTDAttribute.type == "ID")
        {
            if (m_elements[elementName].idAttributePresent)
            {
                throw XML::SyntaxError("Element <" + elementName + "> has more than one ID attribute.");
            }
            m_elements[elementName].idAttributePresent = true;
        }
        else if (xDTDAttribute.type[0] == '(')
        {
            std::set<std::string> options;
            for (auto &option : split(xDTDAttribute.type.substr(1, xDTDAttribute.type.size() - 2), '|'))
            {
                if (!options.contains(option))
                {
                    options.insert(option);
                }
                else
                {
                    throw XML::SyntaxError("Enumerator value '" + option + "' for attribute '" + xDTDAttribute.name + "' occurs more than once in its definition.");
                }
            }
            if (!options.contains(xDTDAttribute.value.parsed))
            {
                throw XML::SyntaxError("Default value '" + xDTDAttribute.value.parsed + "' for enumeration attribute '" + xDTDAttribute.name + "' is invalid.");
            }
        }
    }
    /// <summary>
    /// Split a string into a vector of strings using the passed in delimeter.
    /// </summary>
    /// <param name="stringToSplit">String to split up.</param>
    /// <param name="delimeter">Character delimeter to split on.</param>
    /// <returns>Vector of split strings.</returns>
    std::vector<std::string> DTD::split(std::string stringToSplit, char delimeter)
    {
        std::stringstream sourceStream(stringToSplit);
        std::string splitOffItem;
        std::vector<std::string> splitStrings;
        while (std::getline(sourceStream, splitOffItem, delimeter))
        {
            splitStrings.push_back(splitOffItem);
        }
        return splitStrings;
    }
    /// <summary>
    /// Take an entity reference string, check whether it contains any infinitely recursive
    /// definition and throw an exception if so. This is done by recurively parsing any entities
    /// found in a entiity mapping and adding it to a current stack of used entities; throwing an
    /// exception if it is already being used (recursive).
    /// </summary>
    /// <param name="entityReference">Entity reference to check.</param>
    /// <param name="currentEntities">Set of currently active entities.</param>
    void DTD::checkForEntityRecursion(const std::string &entityReference, std::set<std::string> currentEntities)
    {
        BufferSource entitySource(entityReference);
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
                    if (currentEntities.contains(mappedEntityName))
                    {
                        throw XML::SyntaxError("Entity '" + mappedEntityName + "' contains recursive definition which is not allowed.");
                    }
                    if (!m_entityMapping[mappedEntityName].internal.empty())
                    {
                        currentEntities.emplace(mappedEntityName);
                        checkForEntityRecursion(m_entityMapping[mappedEntityName].internal, currentEntities);
                        currentEntities.erase(mappedEntityName);
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
            if (dtdSource.match(U"<!ENTITY"))
            {
                extractTagBody(dtdSource);
            }
            else if (dtdSource.match(U"<!ELEMENT"))
            {
                BufferSource dtdTranslatedSource(parseTranslateParameterEntities(extractTagBody(dtdSource)));
                parseElement(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!ATTLIST"))
            {
                BufferSource dtdTranslatedSource(parseTranslateParameterEntities(extractTagBody(dtdSource)));
                parseAttributeList(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!NOTATION"))
            {
                BufferSource dtdTranslatedSource(parseTranslateParameterEntities(extractTagBody(dtdSource)));
                parseNotation(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!--"))
            {
                parseComment(dtdSource);
            }
            else if (dtdSource.match(U"%"))
            {
                parseParameterEntity(dtdSource);
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
            }
            else if (dtdSource.match(U"<!ELEMENT"))
            {
                extractTagBody(dtdSource);
            }
            else if (dtdSource.match(U"<!ATTLIST"))
            {
                extractTagBody(dtdSource);
            }
            else if (dtdSource.match(U"<!NOTATION"))
            {
                extractTagBody(dtdSource);
            }
            else if (dtdSource.match(U"<!--"))
            {
                parseComment(dtdSource);
            }
            else if (dtdSource.match(U"%"))
            {
                parseParameterEntity(dtdSource);
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
            throw XML::SyntaxError(dtdSource, "Missing closing ')' on enumeration attribute type.");
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
    void DTD::parsePostProcessing()
    {
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
            FileSource dtdFilePass1(m_external.systemID);
            parseParameterENTITIES(dtdFilePass1);
            FileSource dtdFilePass2(m_external.systemID);
            parseTranslateParameterENTITIES(dtdFilePass2);
        }
        else if (m_external.type == "PUBLIC")
        {
            // Public external DTD currently not supported
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    XMLExternalReference DTD::parseExternalReference(ISource &dtdSource)
    {
        XMLExternalReference result;
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
            throw XML::SyntaxError(dtdSource, "Invalid external DTD specifier.");
        }
        return (result);
    }
    /// <summary>
    /// Parse DTD attribute type field.
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
    /// Parse DTD attribute value.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns>Attribute value as string (UTF-8 encoded).</returns>
    /// <returns></returns>
    XMLValue DTD::parseAttributeValue(ISource &dtdSource)
    {
        XMLValue value;
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
            XMLValue fixedValue = parseValue(dtdSource, m_entityMapping);
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
    /// Parse DTD attribute list.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseAttributeList(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        std::string elementName = parseName(dtdSource);
        while (dtdSource.more() && validNameStartChar(dtdSource.current()))
        {
            DTDAttribute xDTDAttribute;
            xDTDAttribute.name = parseName(dtdSource);
            xDTDAttribute.type = parseAttributeType(dtdSource);
            xDTDAttribute.value = parseAttributeValue(dtdSource);
            parseValidateAttribute(elementName, xDTDAttribute);
            m_elements[elementName].attributes.emplace_back(xDTDAttribute);
            dtdSource.ignoreWS();
        }
    }
    /// <summary>
    /// Parse DTD notation.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseNotation(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        XMLAttribute notation;
        std::string name = parseName(dtdSource);
        m_notations[name] = parseExternalReference(dtdSource);
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse DTD entity.
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
            XMLValue entityValue = parseValue(dtdSource, m_entityMapping, false);
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
    /// Parse an DTD element.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseElement(ISource &dtdSource)
    {
        dtdSource.ignoreWS();
        std::string elementName = parseName(dtdSource);
        XMLValue contentSpecification;
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
            try
            {
                parseElementContentSpecification(contentSpecification);
            }
            catch (XML::SyntaxError &e)
            {
                if (e.what() == std::string("XML Syntax Error: Invalid content region specification."))
                {
                    throw XML::SyntaxError("Invalid content region specification for element <" + elementName + ">.");
                }
                else
                {
                    throw;
                }
            }
        }
        m_elements.emplace(elementName, DTDElement(elementName, contentSpecification));
        dtdSource.ignoreWS();
    }
    /// <summary>
    /// Parse externally defined DTD.
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
    /// Parse DTD comment.
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
    /// Parse DTD parameter entity.
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
    /// Parse internally defined DTD.
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
    /// Parse XML DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseDTD(ISource &dtdSource)
    {
        // We take the easy option for allowing a DTD to be stringifyed
        // and keeping the correct order for its components by storing it
        // in its raw unparsed form.
        long start = dtdSource.position();
        dtdSource.ignoreWS();
        m_name = parseName(dtdSource);
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
    }
} // namespace H4
