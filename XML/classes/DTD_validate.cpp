//
// Class: DTD
//
// Description: Document Type Declaration (DTD) XML validation code.
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
    bool DTD::validateIsPCDATA(XMLNodeElement *xmlNodeElement)
    {
        for (auto &element : xmlNodeElement->children)
        {
            if ((XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::element) ||
                (XMLNodeRef<XMLNode>(*element).getNodeType() == XMLNodeType::self))
            {
                return (false);
            }
        }
        return (!xmlNodeElement->getContents().empty());
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    bool DTD::validateIsEMPTY(XMLNodeElement *xmlNodeElement)
    {
        return (xmlNodeElement->children.empty() || xmlNodeElement->getNodeType() == XMLNodeType::self);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateAttributes(XMLNodeDTD */*dtd*/, XMLNodeElement *xmlNodeElement)
    {
        for (auto &attribute : m_elements[xmlNodeElement->name].attributes)
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
                if (xmlNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xmlNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed.empty()) // No character data present.
                    {
                        //throw XML::ValidationError(*this, "");
                    }
                }
            }
            else if (attribute.type == "ID")
            {
                if (xmlNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xmlNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed[0] != '_' &&
                        !std::isalpha(elementAttribute.value.parsed[0]) &&
                        elementAttribute.value.parsed[0] != ':')
                    {
                        throw XML::ValidationError(*this, "Element <" + xmlNodeElement->name + "> ID attribute '" + elementAttribute.name + "' has a value that does not start with a letter, '_' or ':'.");
                    }
                    if (m_assignedIDValues.contains(elementAttribute.value.parsed))
                    {
                        throw XML::ValidationError(*this, "Element <" + xmlNodeElement->name + "> ID attribute '" + elementAttribute.name + "' is not unique.");
                    }
                    m_assignedIDValues.insert(elementAttribute.value.parsed);
                }
            }
            else if (attribute.type == "IDREF")
            {
                if (xmlNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xmlNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed[0] != '_' &&
                        !std::isalpha(elementAttribute.value.parsed[0]) &&
                        elementAttribute.value.parsed[0] != ':')
                    {
                        throw XML::ValidationError(*this, "Element <" + xmlNodeElement->name + "> IDREF attribute '" + elementAttribute.name + "' has a value that does not start with a letter, '_' or ':'.");
                    }
                    m_assignedIDREFValues.insert(elementAttribute.value.parsed);
                }
            }
            else if (attribute.type[0] == '(')
            {
                std::set<std::string> options;
                for (auto &option : splitString(attribute.type.substr(1, attribute.type.size() - 2), '|'))
                {
                    options.insert(option);
                }
                if (xmlNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xmlNodeElement->getAttribute(attribute.name);
                    if (!options.contains(elementAttribute.value.parsed))
                    {
                        throw XML::ValidationError(*this, "Element <" + xmlNodeElement->name + "> attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.value.parsed + "'.");
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
                if (!xmlNodeElement->isAttributePresent(attribute.name))
                {
                    throw XML::ValidationError(*this, "Required attribute '" + attribute.name + "' missing for element <" + xmlNodeElement->name + ">.");
                }
            }
            else if (attribute.value.parsed == "#IMPLIED")
            {
                continue;
            }
            else if (attribute.value.parsed.starts_with("#FIXED "))
            {
                if (xmlNodeElement->isAttributePresent(attribute.name))
                {
                    XMLAttribute elementAttribute = xmlNodeElement->getAttribute(attribute.name);
                    if (attribute.value.parsed.substr(7) != elementAttribute.value.parsed)
                    {
                        throw XML::ValidationError(*this, "Element <" + xmlNodeElement->name + "> attribute '" + attribute.name + "' is '" + elementAttribute.value.parsed + "' instead of '" + attribute.value.parsed.substr(7) + "'.");
                    }
                }
                else
                {
                    XMLValue value;
                    value.parsed = value.unparsed = attribute.value.parsed.substr(7);
                    xmlNodeElement->addAttribute(attribute.name, value);
                }
            }
            else
            {
                if (!xmlNodeElement->isAttributePresent(attribute.name))
                {
                    XMLValue value;
                    value.parsed = value.unparsed = attribute.value.parsed;
                    xmlNodeElement->addAttribute(attribute.name, value);
                }
            }
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateContentSpecification(XMLNodeDTD *dtd, XMLNodeElement *xmlNodeElement)
    {
        if ((dtd == nullptr) || (m_elements.empty()))
        {
            return;
        }
        if (m_elements[xmlNodeElement->name].content.parsed == "((<#PCDATA>))")
        {
            if (!validateIsPCDATA(xmlNodeElement))
            {
                throw XML::ValidationError(*this, "Element <" + xmlNodeElement->name + "> does not contain just any parsable data.");
            }
            return;
        }
        if (m_elements[xmlNodeElement->name].content.parsed == "EMPTY")
        {
            if (!validateIsEMPTY(xmlNodeElement))
            {
                throw XML::ValidationError(*this, "Element <" + xmlNodeElement->name + "> is not empty.");
            }
            return;
        }
        if (m_elements[xmlNodeElement->name].content.parsed == "ANY")
        {
            return;
        }
        std::regex match(m_elements[xmlNodeElement->name].content.parsed);
        std::string elements;
        for (auto &element : xmlNodeElement->children)
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
            throw XML::ValidationError(*this, "<" + xmlNodeElement->name + "> element does not conform to the content specification " +
                                           m_elements[xmlNodeElement->name].content.unparsed + ".");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateElement(XMLNodeDTD *dtd, XMLNodeElement *xmlNodeElement)
    {
        validateContentSpecification(dtd, xmlNodeElement);
        validateAttributes(dtd, xmlNodeElement);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateElements(XMLNodeDTD *dtd, XMLNode *xmlNode)
    {
        switch (xmlNode->getNodeType())
        {
        case XMLNodeType::prolog:
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                validateElements(dtd, element.get());
            }
            break;
        case XMLNodeType::root:
            if (XMLNodeRef<XMLNodeElement>((*xmlNode)).name != m_name)
            {
                throw XML::ValidationError(*this, "DOCTYPE name does not match that of root element " + XMLNodeRef<XMLNodeElement>((*xmlNode)).name + " of DTD.");
            }
        case XMLNodeType::element:
            validateElement(dtd, static_cast<XMLNodeElement *>(xmlNode));
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                validateElements(dtd, element.get());
            }
            break;
        case XMLNodeType::self:
            validateElement(dtd, static_cast<XMLNodeElement *>(xmlNode));
            break;
        case XMLNodeType::comment:
        case XMLNodeType::entity:
        case XMLNodeType::pi:
        case XMLNodeType::cdata:
        case XMLNodeType::dtd:
            break;
        case XMLNodeType::content:
            for (auto &ch : XMLNodeRef<XMLNodeContent>((*xmlNode)).content)
            {
                if (ch == kLineFeed)
                {
                    m_lineNumber++;
                }
            }
            break;
        default:
            throw XML::ValidationError(*this, "Invalid XMLNode encountered during validation.");
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
