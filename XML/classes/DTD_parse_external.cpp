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
    /// <returns></returns
    void DTD::parseConditional(ISource &dtdSource)
    {
        while(dtdSource.more() && !dtdSource.match(U"]]")) {
            dtdSource.next();
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::parseExternalContent(ISource &dtdSource)
    {
        while (dtdSource.more())
        {
            if (dtdSource.match(U"<!ENTITY"))
            {
                parseEntity(dtdSource);
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
                continue;
            }
            else if (dtdSource.match(U"<!["))
            {
                parseConditional(dtdSource);
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
    /// Parse externally defined DTD into DTD XNode.
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::parseExternalRefenceContent()
    {
        if (m_external.type == "SYSTEM")
        {
            FileSource dtdFile(m_external.systemID);
            parseExternalContent(dtdFile);
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
    /// Parse externally defined DTD.
    /// </summary>
    /// <param name="dtdSource">DTD source stream.</param>
    /// <returns></returns>
    void DTD::parseExternal(ISource &dtdSource)
    {
        m_external = parseExternalReference(dtdSource);
        parseExternalRefenceContent();
        if (dtdSource.current() != '>')
        {
            throw XML::SyntaxError(dtdSource, "Missing '>' terminator.");
        }
        dtdSource.next();
        dtdSource.ignoreWS();
    }
} // namespace H4
