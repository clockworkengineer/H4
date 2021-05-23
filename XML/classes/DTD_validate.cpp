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
// ====================
// CLASS IMPLEMENTATION
// ====================
//
// C++ STL
//
#include <vector>
#include <cstring>
#include <algorithm>
#include <regex>
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
    bool DTD::validateIsPCDATA(XMLNodeElement *xNodeElement)
    {
        for (auto &element : xNodeElement->children)
        {
            if ((XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::element) ||
                (XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::self))
            {
                return (false);
            }
        }
        return (!xNodeElement->getContents().empty());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    bool DTD::validateIsEMPTY(XMLNodeElement *xNodeElement)
    {
        return (xNodeElement->children.empty() || xNodeElement->getNodeType() == XMLNodeType::self);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateAttributes(XMLNodeDTD */*dtd*/, XMLNodeElement *xNodeElement)
    {
        for (auto &attribute : m_elements[xNodeElement->name].attributes)
        {
            // Validate a elements attribute type which can be one of the following.
            //
            // CDATA	    The value is character data
            // (en1|en2|..)	The value must be one from an enumerated list
            // ID	        The value is a unique id
            // IDREF        The value is the id of another element
            // IDREFS       The value is a list of other ids
            // NMTOKEN      The value is a valid DTD name
            // NMTOKENS	    The value is a list of valid DTD names
            // ENTIT        The value is an entity
            // ENTITIES	    The value is a list of entities
            // NOTATION	    The value is a name of a notation
            // xml:         The value is a predefined xml value
            //
            // AT PRESENT ONLY ENUMERATION DEALT WITH.
            if (attribute.type == "CDATA")
            {
                if (xNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed.empty()) // No character data present.
                    {
                        //throw XML::ValidationError(*this, "");
                    }
                }
            }
            else if (attribute.type == "ID")
            {
                if (xNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed[0] != '_' &&
                        !std::isalpha(elementAttribute.value.parsed[0]) &&
                        elementAttribute.value.parsed[0] != ':')
                    {
                        throw XML::ValidationError(*this, "Element <" + xNodeElement->name + "> ID attribute '" + elementAttribute.name + "' has a value that does not start with a letter, '_' or ':'.");
                    }
                    if (m_assignedIDValues.contains(elementAttribute.value.parsed))
                    {
                        throw XML::ValidationError(*this, "Element <" + xNodeElement->name + "> ID attribute '" + elementAttribute.name + "' is not unique.");
                    }
                    m_assignedIDValues.insert(elementAttribute.value.parsed);
                }
            }
            else if (attribute.type == "IDREF")
            {
                if (xNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed[0] != '_' &&
                        !std::isalpha(elementAttribute.value.parsed[0]) &&
                        elementAttribute.value.parsed[0] != ':')
                    {
                        throw XML::ValidationError(*this, "Element <" + xNodeElement->name + "> IDREF attribute '" + elementAttribute.name + "' has a value that does not start with a letter, '_' or ':'.");
                    }
                    m_assignedIDREFValues.insert(elementAttribute.value.parsed);
                }
            }
            else if (attribute.type[0] == '(')
            {
                std::set<std::string> options;
                for (auto &option : split(attribute.type.substr(1, attribute.type.size() - 2), '|'))
                {
                    options.insert(option);
                }
                if (xNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (!options.contains(elementAttribute.value.parsed))
                    {
                        throw XML::ValidationError(*this, "Element <" + xNodeElement->name + "> attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.value.parsed + "'.");
                    }
                }
            }
            // Validate attribute value which can be:
            // value	    The default value of the attribute
            // #REQUIRED    The attribute is required
            // #IMPLIED	    The attribute is optional
            // #FIXED value	The attribute value is fixed
            if (attribute.value.parsed == "#REQUIRED")
            {
                if (!xNodeElement->isAttributePresent(attribute.name))
                {
                    throw XML::ValidationError(*this, "Required attribute '" + attribute.name + "' missing for element <" + xNodeElement->name + ">.");
                }
            }
            else if (attribute.value.parsed == "#IMPLIED")
            {
                continue;
            }
            else if (attribute.value.parsed.starts_with("#FIXED "))
            {
                if (xNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (attribute.value.parsed.substr(7) != elementAttribute.value.parsed)
                    {
                        throw XML::ValidationError(*this, "Element <" + xNodeElement->name + "> attribute '" + attribute.name + "' is '" + elementAttribute.value.parsed + "' instead of '" + attribute.value.parsed.substr(7) + "'.");
                    }
                }
                else
                {
                    XMLValue value;
                    value.parsed = value.unparsed = attribute.value.parsed.substr(7);
                    xNodeElement->addAttribute(attribute.name, value);
                }
            }
            else
            {
                if (!xNodeElement->isAttributePresent(attribute.name))
                {
                    XMLValue value;
                    value.parsed = value.unparsed = attribute.value.parsed;
                    xNodeElement->addAttribute(attribute.name, value);
                }
            }
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateContentSpecification(XMLNodeDTD *dtd, XMLNodeElement *xNodeElement)
    {
        if ((dtd == nullptr) || (m_elements.empty()))
        {
            return;
        }
        if (m_elements[xNodeElement->name].content.parsed == "((<#PCDATA>))")
        {
            if (!validateIsPCDATA(xNodeElement))
            {
                throw XML::ValidationError(*this, "Element <" + xNodeElement->name + "> does not contain just any parsable data.");
            }
            return;
        }
        if (m_elements[xNodeElement->name].content.parsed == "EMPTY")
        {
            if (!validateIsEMPTY(xNodeElement))
            {
                throw XML::ValidationError(*this, "Element <" + xNodeElement->name + "> is not empty.");
            }
            return;
        }
        if (m_elements[xNodeElement->name].content.parsed == "ANY")
        {
            return;
        }
        std::regex match(m_elements[xNodeElement->name].content.parsed);
        std::string elements;
        for (auto &element : xNodeElement->children)
        {
            if ((XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::element) ||
                (XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::self))
            {
                elements += "<" + XMLNodeRef<XMLNodeElement>(*element).name + ">";
            }
            else if (XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::content)
            {
                if (!XMLNodeRef<XMLNodeContent>(*element).isWhiteSpace)
                {
                    elements += "<#PCDATA>";
                }
            }
        }
        if (!std::regex_match(elements, match))
        {
            throw XML::ValidationError(*this, "<" + xNodeElement->name + "> element does not conform to the content specification " +
                                           m_elements[xNodeElement->name].content.unparsed + ".");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateElement(XMLNodeDTD *dtd, XMLNodeElement *xNodeElement)
    {
        validateContentSpecification(dtd, xNodeElement);
        validateAttributes(dtd, xNodeElement);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateElements(XMLNodeDTD *dtd, XMLNode *xNode)
    {
        switch (xNode->getNodeType())
        {
        case XMLNodeType::prolog:
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xNode)).children)
            {
                validateElements(dtd, element.get());
            }
            break;
        case XMLNodeType::root:
            if (XMLNodeRef<XMLNodeElement>((*xNode)).name != m_name)
            {
                throw XML::ValidationError(*this, "DOCTYPE name does not match that of root element " + XMLNodeRef<XMLNodeElement>((*xNode)).name + " of DTD.");
            }
        case XMLNodeType::element:
            validateElement(dtd, static_cast<XMLNodeElement *>(xNode));
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xNode)).children)
            {
                validateElements(dtd, element.get());
            }
            break;
        case XMLNodeType::self:
            validateElement(dtd, static_cast<XMLNodeElement *>(xNode));
            break;
        case XMLNodeType::comment:
        case XMLNodeType::entity:
        case XMLNodeType::pi:
        case XMLNodeType::cdata:
        case XMLNodeType::dtd:
            break;
        case XMLNodeType::content:
            for (auto &ch : XMLNodeRef<XMLNodeContent>((*xNode)).content)
            {
                if (ch == kLineFeed)
                {
                    m_lineNumber++;
                }
            }
            break;
        default:
            throw XML::ValidationError(*this, "Invalid XNode encountered during validation.");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateDTD(XMLNodeElement &prolog)
    {
        if (prolog.getNodeType() == XMLNodeType::prolog)
        {
            XMLNodeDTD *dtd;
            for (auto &element : prolog.children)
            {
                if (element->getNodeType() == XMLNodeType::dtd)
                {
                    dtd = static_cast<XMLNodeDTD *>(element.get());
                    break;
                }
            }
            if (dtd != nullptr)
            {
                validateElements(dtd, &prolog);
                for (auto &idref : m_assignedIDREFValues)
                {
                    if (!m_assignedIDValues.contains(idref))
                    {
                        throw XML::ValidationError(*this, "IDREF attribute '"+idref+"' does not reference any element with the ID.");
                    }
                }
            }
        }
    }
} // namespace H4
