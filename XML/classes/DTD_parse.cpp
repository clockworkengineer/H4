//
// Class: DTD
//
// Description: Parse XML Document Type Declaration (DTD).
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
    /// Validate attribute description.
    /// </summary>
    /// <param name="elementName">Element associated with attribute.</param>
    /// <param name="dtdattribute">Attribute description to validate.</param>
    /// <returns></returns>
    void DTD::parseValidateAttribute(const std::string &elementName, DTDAttribute dtdAttribute)
    {
        // Attribute cannot be ID and fixed
        if (dtdAttribute.type == "ID" && dtdAttribute.value.parsed.find("#FIXED ")==0)
        {
            throw XML::SyntaxError("Attribute '" + dtdAttribute.name + "' may not be of type ID and FIXED.");
        }
        // Only on ID attribute allowed per element
        else if (dtdAttribute.type == "ID")
        {
            if (m_elements[elementName].idAttributePresent)
            {
                throw XML::SyntaxError("Element <" + elementName + "> has more than one ID attribute.");
            }
            m_elements[elementName].idAttributePresent = true;
        }
        // Enumeration contains unique values and default is valid value
        else if (dtdAttribute.type[0] == '(')
        {
            std::set<std::string> options;
            for (auto &option : splitString(dtdAttribute.type.substr(1, dtdAttribute.type.size() - 2), '|'))
            {
                if (options.find(option)==options.end())
                {
                    options.insert(option);
                }
                else
                {
                    throw XML::SyntaxError("Enumerator value '" + option + "' for attribute '" + dtdAttribute.name + "' occurs more than once in its definition.");
                }
            }
            if (options.find(dtdAttribute.value.parsed)==options.end())
            {
                throw XML::SyntaxError("Default value '" + dtdAttribute.value.parsed + "' for enumeration attribute '" + dtdAttribute.name + "' is invalid.");
            }
        }
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
                    if (currentEntities.find(mappedEntityName)!=currentEntities.end())
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
    /// Parse attribute of type enumeration.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns>Enumeration string.</returns>
    std::string DTD::parseAttributeEnumerationType(ISource &dtdSource)
    {
        std::string enumerationType(1, dtdSource.current());
        dtdSource.next();
        dtdSource.ignoreWS();
        enumerationType += parseName(dtdSource);
        while (dtdSource.more() && dtdSource.current() == '|')
        {
            enumerationType += dtdSource.current();
            dtdSource.next();
            dtdSource.ignoreWS();
            enumerationType += parseName(dtdSource);
        }
        if (dtdSource.current() != ')')
        {
            throw XML::SyntaxError(dtdSource, "Missing closing ')' on enumeration attribute type.");
        }
        enumerationType += dtdSource.current();
        dtdSource.next();
        dtdSource.ignoreWS();
        return (enumerationType);
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
            DTDAttribute dtdAttribute;
            dtdAttribute.name = parseName(dtdSource);
            dtdAttribute.type = parseAttributeType(dtdSource);
            dtdAttribute.value = parseAttributeValue(dtdSource);
            parseValidateAttribute(elementName, dtdAttribute);
            m_elements[elementName].attributes.emplace_back(dtdAttribute);
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
            parseElementContentSpecification(elementName, contentSpecification);
        }
        m_elements.emplace(elementName, DTDElement(elementName, contentSpecification));
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
    /// Parse DTD parameter entity reference.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseParameterEntityReference(ISource &dtdSource)
    {
        XMLValue parameterEntity = parseEntityReference(dtdSource);
        BufferSource entitySource(translateEntities(parameterEntity.unparsed, m_entityMapping));
        parseInternal(entitySource);
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
            else if (dtdSource.current() == '%')
            {
                parseParameterEntityReference(dtdSource);
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
    /// Parse XML DTD. If the DTD contains an external reference this is parsed
    /// after any internal DTD that may be specified after it in the DTD.
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
        // Parse in external DTD reference
        if (dtdSource.current() != '[')
        {
            m_external = parseExternalReference(dtdSource);
        }
        // We have internal DTD so parse that first
        if (dtdSource.current() == '[')
        {
            dtdSource.next();
            dtdSource.ignoreWS();
            parseInternal(dtdSource);
        }
        // Missing '>' after external DTD reference
        else if (dtdSource.current() != '>')
        {
            throw XML::SyntaxError(dtdSource, "Missing '>' terminator.");
        }
        // Move to the next component in XML prolog
        else
        {
            dtdSource.next();
            dtdSource.ignoreWS();
        }
        // Parse any DTD in external reference found
        if (!m_external.type.empty())
        {
            parseExternal(dtdSource);
        }
        // Save away unparsed form of DTD
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
