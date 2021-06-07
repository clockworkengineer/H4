//
// Class: DTD
//
// Description: Parse XML DTD element content specifications.
//
// Dependencies:   C17++ - Language standard features used.
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
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    /// <summary>
    /// Is the next specification operator to be parsed choice '|' or sequence ','.
    /// </summary>
    /// <param name="contentSpecSource">Content specification source stream.</param>
    /// <returns>Returns true if it is choice.</returns>
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
    /// Parse element name, choice or sequence next in content specification.
    /// </summary>
    /// <param name="contentSpecSource">Content specification source stream.</param>
    /// <param name="contentSpecDestination">Parsed content specification stream.</param>
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
    /// Parse content specification choice next.
    /// </summary>
    /// <param name="contentSpecSource">Content specification source stream.</param>
    /// <param name="contentSpecDestination">Parsed content specification stream.</param>
    void DTD::parseElementChoice(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        if (contentSpecSource.current() != '(')
        {
            throw XML::SyntaxError("Invalid element content specification.");
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
            throw XML::SyntaxError("Invalid element content specification.");
        }
        contentSpecDestination.add(")");
        contentSpecSource.next();
        contentSpecSource.ignoreWS();
    }
    /// <summary>
    /// Parse content specification sequence next.
    /// </summary>
    /// <param name="contentSpecSource">Content specification source stream.</param>
    /// <param name="contentSpecDestination">Parsed content specification stream.</param>
    void DTD::parseElementSequence(ISource &contentSpecSource, IDestination &contentSpecDestination)
    {
        if (contentSpecSource.current() != '(')
        {
            throw XML::SyntaxError("Invalid element content specification.");
        }
        contentSpecDestination.add("(");
        parseElementCP(contentSpecSource, contentSpecDestination);
        while (contentSpecSource.more() && contentSpecSource.current() == ',')
        {
            parseElementCP(contentSpecSource, contentSpecDestination);
        }
        if (contentSpecSource.current() != ')')
        {
            throw XML::SyntaxError("Invalid element content specification.");
        }
        contentSpecDestination.add(")");
        contentSpecSource.next();
        contentSpecSource.ignoreWS();
    }
    /// <summary>
    /// Parse content specification element name next.
    /// </summary>
    /// <param name="contentSpecSource">Content specification source stream.</param>
    /// <param name="contentSpecDestination">Parsed content specification stream.</param>
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
    /// Parse content specification element children.
    /// </summary>
    /// <param name="contentSpecSource">Content specification source stream.</param>
    /// <param name="contentSpecDestination">Parsed content specification stream.</param>
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
            throw XML::SyntaxError("Invalid element content specification.");
        }
    }
    /// <summary>
    /// Parse content specification mixed content.
    /// </summary>
    /// <param name="contentSpecSource">Content specification source stream.</param>
    /// <param name="contentSpecDestination">Parsed content specification stream.</param>
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
                    throw XML::SyntaxError("Invalid element content specification.");
                }
            }
            if (contentSpecSource.current() != ')')
            {
                throw XML::SyntaxError("Invalid element content specification.");
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
                throw XML::SyntaxError("Invalid element content specification.");
            }
        }
        else if (contentSpecSource.current() == ')')
        {
            contentSpecDestination.add(")");
        }
        else
        {
            throw XML::SyntaxError("Invalid element content specification.");
        }
    }
    /// <summary>
    /// Parse elements content specification.
    /// </summary>
    /// <param name="elementName">Elements name.</param>
    /// <param name="contentSpec">Elements content specification.</param>
    void DTD::parseElementContentSpecification(const std::string &elementName, XMLValue &contentSpec)
    {
        try
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
                throw XML::SyntaxError("Invalid element content specification.");
            }
            contentSpec.parsed = contentSpecDestination.getBuffer();
        }
        catch (XML::SyntaxError &e)
        {
            if (e.what() == std::string("XML Syntax Error: Invalid element content specification."))
            {
                throw XML::SyntaxError("Invalid content specification for element <" + elementName + ">.");
            }
            else
            {
                throw;
            }
        }
    }
} // namespace H4
