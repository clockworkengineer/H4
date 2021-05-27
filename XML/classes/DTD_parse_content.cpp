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
    void DTD::parseElementContentSpecification(XMLValue &contentSpec)
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

} // namespace H4
