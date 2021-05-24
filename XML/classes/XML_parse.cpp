//
// Class: XML
//
// Description: XML parser code. All parsing of characters takes place having
// converted the characters to UTF-32 to make the process easier.
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
    void XML::parseEntityMappingContents(XMLNodeElement *xmlNodeElement, XMLValue &entityReference)
    {
        XMLNodeEntityReference xNodeEntityReference(entityReference);
        if (entityReference.unparsed[1] != '#')
        {
            XMLNodeElement entityElement;
            BufferSource entitySource(entityReference.parsed);
            if (entityReference.parsed.find("<") == std::string::npos)
            {
                while (entitySource.more())
                {
                    parseElementContents(entitySource, &entityElement);
                }
                xNodeEntityReference.children = std::move(entityElement.children);
                if (!xmlNodeElement->children.empty())
                {
                    if (xmlNodeElement->children.back()->getNodeType() == XMLNodeType::content)
                    {
                        XMLNodeRef<XMLNodeContent>(*xmlNodeElement->children.back()).isWhiteSpace = false;
                    }
                }
            }
            else
            {
                while (entitySource.more())
                {
                    parseElementContents(entitySource, xmlNodeElement);
                }
                return;
            }
        }
        xmlNodeElement->children.emplace_back(std::make_unique<XMLNodeEntityReference>(std::move(xNodeEntityReference)));
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::parseAddElementContent(XMLNodeElement *xmlNodeElement, const std::string &content)
    {
        // Make sure there is a content node to recieve characters
        if (xmlNodeElement->children.empty() ||
            xmlNodeElement->children.back()->getNodeType() != XMLNodeType::content)
        {
            bool isWWhitespace = true;
            if (!xmlNodeElement->children.empty())
            {
                if ((xmlNodeElement->children.back()->getNodeType() == XMLNodeType::cdata) ||
                    (xmlNodeElement->children.back()->getNodeType() == XMLNodeType::entity))
                {
                    isWWhitespace = false;
                }
            }
            xmlNodeElement->children.emplace_back(std::make_unique<XMLNodeContent>());
            XMLNodeRef<XMLNodeContent>(*xmlNodeElement->children.back()).isWhiteSpace = isWWhitespace;
        }
        if (XMLNodeRef<XMLNodeContent>(*xmlNodeElement->children.back()).isWhiteSpace)
        {
            for (auto ch : content)
            {
                if (!std::iswspace(ch))
                {
                    XMLNodeRef<XMLNodeContent>(*xmlNodeElement->children.back()).isWhiteSpace = false;
                    break;
                }
            }
        }
        XMLNodeRef<XMLNodeContent>(*xmlNodeElement->children.back()).content += content;
    }
    /// <summary>
    /// Parse a element tag name and set its value in current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseTagName(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        xmlNodeElement->name = parseName(xmlSource);
    }
    /// <summary>
    /// Parse a XML comment, create a XNodeComment for it and add to list
    /// of elements for the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseComment(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        XMLNodeComment xNodeComment;
        while (xmlSource.more() && !xmlSource.match(U"--"))
        {
            xNodeComment.comment += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        if (!xmlSource.match(U">"))
        {
            throw SyntaxError(xmlSource, "Missing closing '>' for comment line.");
        }
        xmlNodeElement->children.emplace_back(std::make_unique<XMLNodeComment>(std::move(xNodeComment)));
    }
    /// <summary>
    /// Parse a XML process instruction, create an XNode for it and add it to
    /// the list of elements under the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parsePI(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        XNodePI xNodePI;
        xNodePI.name = parseName(xmlSource);
        while (xmlSource.more() && !xmlSource.match(U"?>"))
        {
            xNodePI.parameters += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        xmlNodeElement->children.emplace_back(std::make_unique<XNodePI>(std::move(xNodePI)));
    }
    /// <summary>
    /// Parse an XML CDATA section create an XNode for it and add it to
    /// the list of elements under the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseCDATA(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        XMLNodeCDATA xNodeCDATA;
        while (xmlSource.more() && !xmlSource.match(U"]]>"))
        {
            if (xmlSource.match(U"<![CDATA["))
            {
                throw SyntaxError(xmlSource, "Nesting of CDATA sections is not allowed.");
            }
            xNodeCDATA.cdata += xmlSource.current_to_bytes();
            xmlSource.next();
        }
        if (!xmlNodeElement->children.empty())
        {
            if (xmlNodeElement->children.back()->getNodeType() == XMLNodeType::content)
            {
                XMLNodeRef<XMLNodeContent>(*xmlNodeElement->children.back()).isWhiteSpace = false;
            }
        }
        xmlNodeElement->children.emplace_back(std::make_unique<XMLNodeCDATA>(std::move(xNodeCDATA)));
    }
    /// <summary>
    /// Parse list of attributes (name/value pairs) that exist in a tag and add them to
    /// the list of attributes associated with the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseAttributes(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        while (xmlSource.more() &&
               xmlSource.current() != '?' &&
               xmlSource.current() != '/' &&
               xmlSource.current() != '>')
        {
            std::string attributeName = parseName(xmlSource);
            if (!xmlSource.match(U"="))
            {
                throw SyntaxError(xmlSource, "Missing '=' between attribute name and value.");
            }
            xmlSource.ignoreWS();
            XMLValue attributeValue = parseValue(xmlSource, m_dtd.m_entityMapping);
            if (!validAttributeValue(attributeValue))
            {
                throw SyntaxError(xmlSource, "Attribute value contains invalid character '<', '\"', ''' or '&'.");
            }
            if (!xmlNodeElement->isAttributePresent(attributeName))
            {
                xmlNodeElement->addAttribute(attributeName, attributeValue);
            }
            else
            {
                throw SyntaxError(xmlSource, "Attribute defined more than once within start tag.");
            }
        }
        for (auto attribute : xmlNodeElement->getAttributeList())
        {
            if (attribute.name.starts_with("xmlns"))
            {
                attribute.name = (attribute.name.size() > 5) ? attribute.name.substr(6) : ":";
                xmlNodeElement->addNameSpace(attribute.name, attribute.value);
            }
        }
    }
    /// <summary>
    /// Recursively parse any child elements of the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseChildElement(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        XMLNodeElement xNodeChildElement;
        for (auto &ns : xmlNodeElement->getNameSpaceList())
        {
            xNodeChildElement.addNameSpace(ns.name, ns.value);
        }
        parseElement(xmlSource, &xNodeChildElement);
        if (auto pos = xNodeChildElement.name.find(':'); pos != std::string::npos)
        {
            if (!xNodeChildElement.isNameSpacePresent(xNodeChildElement.name.substr(0, pos)))
            {
                throw SyntaxError(xmlSource, "Namespace used but not defined.");
            }
        }
        xmlNodeElement->children.push_back(std::make_unique<XMLNodeElement>(std::move(xNodeChildElement)));
    }
    /// <summary>
    /// Parse any element content that is found.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseDefault(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        XMLValue entityReference = parseCharacter(xmlSource, m_dtd.m_entityMapping);
        if (entityReference.unparsed.starts_with("&") && entityReference.unparsed.ends_with(";"))
        {
            parseEntityMappingContents(xmlNodeElement, entityReference);
        }
        else
        {
            parseAddElementContent(xmlNodeElement, entityReference.parsed);
        }
    }
    /// <summary>
    /// Parse element content area generating any XNodes and adding them
    /// to the list of the current XNodeElement.
    /// </summary>
    /// <param name="xmlSource">XMl source stream.</param>
    /// <returns></returns>
    void XML::parseElementContents(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        if (xmlSource.match(U"<!--"))
        {
            parseComment(xmlSource, xmlNodeElement);
        }
        else if (xmlSource.match(U"<?"))
        {
            parsePI(xmlSource, xmlNodeElement);
        }
        else if (xmlSource.match(U"<![CDATA["))
        {
            parseCDATA(xmlSource, xmlNodeElement);
        }
        else if (xmlSource.match(U"</"))
        {
            throw SyntaxError(xmlSource, "Missing closing tag.");
        }
        else if (xmlSource.match(U"<"))
        {
            parseChildElement(xmlSource, xmlNodeElement);
        }
        else if (xmlSource.match(U"]]>"))
        {
            throw SyntaxError(xmlSource, "']]>' invalid in element content area.");
        }
        else
        {
            parseDefault(xmlSource, xmlNodeElement);
        }
    }
    /// <summary>
    /// Parse current XML element found.
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseElement(ISource &xmlSource, XMLNodeElement *xmlNodeElement)
    {
        parseTagName(xmlSource, xmlNodeElement);
        parseAttributes(xmlSource, xmlNodeElement);
        if (xmlSource.match(U">"))
        {
            while (xmlSource.more() && !xmlSource.match(U"</"))
            {
                parseElementContents(xmlSource, xmlNodeElement);
            }
            if (!xmlSource.match(xmlSource.from_bytes(xmlNodeElement->name) + U">"))
            {
                throw SyntaxError(xmlSource, "Missing closing tag.");
            }
        }
        else if (xmlSource.match(U"/>"))
        {
            // Self closing element tag
            xmlNodeElement->setNodeType(XMLNodeType::self);
        }
        else
        {
            throw SyntaxError(xmlSource, "Missing closing tag.");
        }
    }
    /// <summary>
    /// Parse XML prolog and create the necessary XNodeElements for it. Valid
    /// parts of the prolog include delaration (first line if present),
    /// processing instructions, comments, whitespace content and XML
    /// document Type Definition (DTD).
    /// </summary>
    /// <param name="xmlSource">XML source stream.</param>
    /// <returns></returns>
    void XML::parseProlog(ISource &xmlSource, XMLNodeElement *xmlNodeProlog)
    {
        xmlNodeProlog->setNodeType(XMLNodeType::prolog);
        xmlSource.ignoreWS();
        if (xmlSource.match(U"<?xml"))
        {
            xmlSource.ignoreWS();
            parseAttributes(xmlSource, xmlNodeProlog);
            if (!xmlSource.match(U"?>"))
            {
                throw SyntaxError(xmlSource, "Declaration end tag not found.");
            }
            validXMLDeclaration(xmlSource, xmlNodeProlog);
        }
        while (xmlSource.more())
        {
            if (xmlSource.match(U"<!--"))
            {
                parseComment(xmlSource, xmlNodeProlog);
            }
            else if (xmlSource.match(U"<?"))
            {
                parsePI(xmlSource, xmlNodeProlog);
            }
            else if (xmlSource.match(U"<!DOCTYPE"))
            {
                m_dtd.parse(xmlSource);
                xmlNodeProlog->children.emplace_back(std::make_unique<XMLNodeDTD>());
            }
            else if (xmlSource.current() == '<')
            {
                // Break out as root element detected
                break;
            }
            else if (std::iswspace(xmlSource.current()))
            {
                parseAddElementContent(xmlNodeProlog, std::string(1, xmlSource.current()));
                xmlSource.next();
            }
            else
            {
                throw SyntaxError(xmlSource, "Content detected before root element.");
            }
        }
    }
    /// <summary>
    /// Parse XML source stream.
    /// </summary>
    /// <returns></returns>
    void XML::parseXML()
    {
        parseProlog(m_xmlSource, &m_prolog);
        if (m_xmlSource.match(U"<"))
        {
            m_prolog.children.emplace_back(std::make_unique<XMLNodeElement>(XMLNodeElement(XMLNodeType::root)));
            parseElement(m_xmlSource, static_cast<XMLNodeElement *>(m_prolog.children.back().get()));
            while (m_xmlSource.more())
            {
                if (m_xmlSource.match(U"<!--"))
                {
                    parseComment(m_xmlSource, (&m_prolog));
                }
                else if (std::iswspace(m_xmlSource.current()))
                {
                    parseAddElementContent(&m_prolog, std::string(1, m_xmlSource.current()));
                    m_xmlSource.next();
                }
                else
                {
                    throw SyntaxError(m_xmlSource, "Extra content at the end of document.");
                }
            }
        }
        else
        {
            throw SyntaxError(m_xmlSource, "Missing root element.");
        }
    }
} // namespace H4
