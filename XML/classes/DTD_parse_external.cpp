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
    void DTD::parseConditional(ISource &dtdSource, bool includeOn)
    {
        dtdSource.ignoreWS();
        std::string conditionalValue = "";
        if (includeOn)
        {
            if (dtdSource.current() == '%')
            {
                XMLValue entityRerence = parseEntityReference(dtdSource);
                dtdSource.ignoreWS();
                mapEntityReference(entityRerence, m_entityMapping);
                conditionalValue = entityRerence.parsed;
            }
            else if (dtdSource.match(U"INCLUDE"))
            {
                conditionalValue = "INCLUDE";
            }
            else if (dtdSource.match(U"IGNORE"))
            {
                conditionalValue = "IGNORE";
            }
        }
        else
        {
            conditionalValue = "IGNORE";
        }
        if (conditionalValue == "INCLUDE")
        {
            dtdSource.ignoreWS();
            if (dtdSource.current() != '[')
            {
                throw XML::SyntaxError(dtdSource, "Missing opening '[' from conditional.");
            }
            dtdSource.next();
            dtdSource.ignoreWS();
            std::string conditionalDTD;
            while (dtdSource.more() && !dtdSource.match(U"]]"))
            {
                if (dtdSource.match(U"<!["))
                {
                    parseConditional(dtdSource);
                }
                else
                {
                    conditionalDTD += dtdSource.current();
                    dtdSource.next();
                }
            }
            BufferSource conditionalDTDSource(conditionalDTD);
            parseExternalContent(conditionalDTDSource);
        }
        else if (conditionalValue == "IGNORE")
        {
            while (dtdSource.more() && !dtdSource.match(U"]]"))
            {
                if (dtdSource.match(U"<!["))
                {
                    parseConditional(dtdSource, false);
                }
                else
                {
                    dtdSource.next();
                }
            }
        }
        else
        {
            throw XML::SyntaxError(dtdSource, "Conditional value not INCLUDE or IGNORE.");
        }
        if (dtdSource.current() != '>')
        {
            throw XML::SyntaxError(dtdSource, "Missing '>' terminator.");
        }
        dtdSource.next();
        dtdSource.ignoreWS();
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
                BufferSource dtdTranslatedSource(translateEntities(extractTagBody(dtdSource), m_entityMapping));
                parseEntity(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!ELEMENT"))
            {
                BufferSource dtdTranslatedSource(translateEntities(extractTagBody(dtdSource), m_entityMapping));
                parseElement(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!ATTLIST"))
            {
                BufferSource dtdTranslatedSource(translateEntities(extractTagBody(dtdSource), m_entityMapping));
                parseAttributeList(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!NOTATION"))
            {
                BufferSource dtdTranslatedSource(translateEntities(extractTagBody(dtdSource), m_entityMapping));
                parseNotation(dtdTranslatedSource);
            }
            else if (dtdSource.match(U"<!--"))
            {
                parseComment(dtdSource);
            }
            else if (dtdSource.current()=='%')
            {
                parseParameterEntityReference(dtdSource);
                continue;
            }
            else if (dtdSource.match(U"<!["))
            {
                parseConditional(dtdSource);
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
    void DTD::parseExternal(ISource & /*dtdSource*/)
    {
        parseExternalRefenceContent();
    }
} // namespace H4
