//
// Class: XMLValidator
//
// Description: XML validation code.
//
// Dependencies:   C17++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "XMLConfig.hpp"
#include "XML.hpp"
#include "XMLValidator.hpp"
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
    bool XMLValidator::checkIsNMTOKENOK(std::string nmTokenValue)
    {
        trimmString(nmTokenValue);
        BufferSource nmTokenValueSource(nmTokenValue);
        while (nmTokenValueSource.more())
        {
            if (!validNameChar(nmTokenValueSource.current()))
            {
                return (false);
            }
            nmTokenValueSource.next();
        }
        return (true);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    bool XMLValidator::checkIsIDOK(const std::string &idValue)
    {
        try
        {
            BufferSource idSource(idValue);
            parseName(idSource);
        }
        catch (std::exception &e)
        {
            return (false);
        }
        return (true);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    bool XMLValidator::checkIsPCDATA(XMLNodeElement *xmlNodeElement)
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
    bool XMLValidator::checkIsEMPTY(XMLNodeElement *xmlNodeElement)
    {
        return (xmlNodeElement->children.empty() || xmlNodeElement->getNodeType() == XMLNodeType::self);
    }
    /// <summary>
    ///
    /// Validate attribute value which can be:
    ///
    /// value	        The default value of the attribute
    /// #REQUIRED       The attribute is required
    /// #IMPLIED	    The attribute is optional
    /// #FIXED value	The attribute value is fixed
    ////
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XMLValidator::checkAttributeValue(XMLNodeElement *xmlNodeElement, DTDAttribute &attribute)
    {
        if ((attribute.type & DTDAttributeType::required) != 0)
        {
            if (!xmlNodeElement->isAttributePresent(attribute.name))
            {
                throw XMLValidationError(m_lineNumber, "Required attribute '" + attribute.name + "' missing for element <" + xmlNodeElement->name + ">.");
            }
        }
        else if ((attribute.type & DTDAttributeType::implied) != 0)
        {
            return;
        }
        else if ((attribute.type & DTDAttributeType::fixed) != 0)
        {
            if (xmlNodeElement->isAttributePresent(attribute.name))
            {
                XMLAttribute elementAttribute = xmlNodeElement->getAttribute(attribute.name);
                if (attribute.value.parsed != elementAttribute.value.parsed)
                {
                    throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> attribute '" + attribute.name + "' is '" + elementAttribute.value.parsed + "' instead of '" + attribute.value.parsed + "'.");
                }
            }
            else
            {
                XMLValue value;
                value.parsed = value.unparsed = attribute.value.parsed;
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
    /// <summary>
    ///
    /// Validate a elements attribute type which can be one of the following.
    ///
    /// CDATA	      The value is character data
    /// (en1|en2|..)  The value must be one from an enumerated list
    /// ID	          The value is a unique id
    /// IDREF         The value is the id of another element
    /// IDREFS        The value is a list of other ids
    /// NMTOKEN       The value is a valid DTD name
    /// NMTOKENS	  The value is a list of valid DTD names
    /// ENTITY        The value is an entity
    /// ENTITIES	  The value is a list of entities
    /// NOTATION	  The value is a name of a notation
    /// xml:          The value is a predefined xml value
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XMLValidator::checkAttributeType(XMLNodeElement *xmlNodeElement, DTDAttribute &attribute)
    {
        XMLAttribute elementAttribute = xmlNodeElement->getAttribute(attribute.name);
        if ((attribute.type & DTDAttributeType::cdata) != 0)
        {
            if (elementAttribute.value.parsed.empty()) // No character data present.
            {
                XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> attribute '" + attribute.name + "' does not contain character data.");
            }
        }
        else if ((attribute.type & DTDAttributeType::id) != 0)
        {
            if (!checkIsIDOK(elementAttribute.value.parsed))
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> ID attribute '" + attribute.name + "' is invalid.");
            }
            if (m_assignedIDValues.find(elementAttribute.value.parsed) != m_assignedIDValues.end())
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> ID attribute '" + attribute.name + "' is not unique.");
            }
            m_assignedIDValues.insert(elementAttribute.value.parsed);
        }
        else if ((attribute.type & DTDAttributeType::idref) != 0)
        {
            if (!checkIsIDOK(elementAttribute.value.parsed))
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> IDREF attribute '" + attribute.name + "' is invalid.");
            }
            m_assignedIDREFValues.insert(elementAttribute.value.parsed);
        }
        else if ((attribute.type & DTDAttributeType::idrefs) != 0)
        {
            for (auto &id : splitString(elementAttribute.value.parsed, ' '))
            {
                if (!checkIsIDOK(id))
                {
                    throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> IDREFS attribute '" + attribute.name + "' contains an invalid IDREF.");
                }
                m_assignedIDREFValues.insert(id);
            }
        }
        else if ((attribute.type & DTDAttributeType::nmtoken) != 0)
        {
            if (!checkIsNMTOKENOK(elementAttribute.value.parsed))
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> NMTOKEN attribute '" + attribute.name + "' is invalid.");
            }
        }
        else if ((attribute.type & DTDAttributeType::nmtokens) != 0)
        {
            for (auto &nmtoken : splitString(elementAttribute.value.parsed, ' '))
            {
                if (!checkIsNMTOKENOK(nmtoken))
                {
                    throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> NMTOKEN attribute '" + attribute.name + "' contains an invald NMTOKEN.");
                }
            }
        }
        else if ((attribute.type & DTDAttributeType::entity) != 0)
        {
            if (!m_dtd.isEntityPresent("&" + elementAttribute.value.parsed + ";"))
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> ENTITY attribute '" + attribute.name + "' value '" + elementAttribute.value.parsed + "' is not defined.");
            }
        }
        else if ((attribute.type & DTDAttributeType::entities) != 0)
        {
            for (auto &entity : splitString(elementAttribute.value.parsed, ' '))
            {
                if (!m_dtd.isEntityPresent("&" + entity + ";"))
                {
                    throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> ENTITIES attribute '" + attribute.name + "' value '" + entity + "' is not defined.");
                }
            }
        }
        else if ((attribute.type & DTDAttributeType::notation) != 0)
        {
            std::set<std::string> notations;
            for (auto &notation : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|'))
            {
                notations.insert(notation);
            }
            if (notations.count(elementAttribute.value.parsed) == 0)
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> NOTATION attribute '" + attribute.name + "' value '" + elementAttribute.value.parsed + "' is not defined.");
            }
        }
        else if ((attribute.type & DTDAttributeType::enumeration) != 0)
        {
            std::set<std::string> enumeration;
            for (auto &option : splitString(attribute.enumeration.substr(1, attribute.enumeration.size() - 2), '|'))
            {
                enumeration.insert(option);
            }
            if (enumeration.find(elementAttribute.value.parsed) == enumeration.end())
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.value.parsed + "'.");
            }
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XMLValidator::checkAttributes(XMLNodeElement *xmlNodeElement)
    {
        for (auto &attribute : m_dtd.getElement(xmlNodeElement->name).attributes)
        {
            if (xmlNodeElement->isAttributePresent(attribute.name))
            {
                checkAttributeType(xmlNodeElement, attribute);
            }
            checkAttributeValue(xmlNodeElement, attribute);
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XMLValidator::checkContentSpecification(XMLNodeElement *xmlNodeElement)
    {
        if (m_dtd.getElementCount() == 0)
        {
            return;
        }
        if (m_dtd.getElement(xmlNodeElement->name).content.parsed == "((<#PCDATA>))")
        {
            if (!checkIsPCDATA(xmlNodeElement))
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> does not contain just any parsable data.");
            }
            return;
        }
        if (m_dtd.getElement(xmlNodeElement->name).content.parsed == "EMPTY")
        {
            if (!checkIsEMPTY(xmlNodeElement))
            {
                throw XMLValidationError(m_lineNumber, "Element <" + xmlNodeElement->name + "> is not empty.");
            }
            return;
        }
        if (m_dtd.getElement(xmlNodeElement->name).content.parsed == "ANY")
        {
            return;
        }
        std::regex match(m_dtd.getElement(xmlNodeElement->name).content.parsed);
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
            throw XMLValidationError(m_lineNumber, "<" + xmlNodeElement->name + "> element does not conform to the content specification " +
                                                       m_dtd.getElement(xmlNodeElement->name).content.unparsed + ".");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XMLValidator::checkElement(XMLNodeElement *xmlNodeElement)
    {
        checkContentSpecification(xmlNodeElement);
        checkAttributes(xmlNodeElement);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XMLValidator::checkElements(XMLNode *xmlNode)
    {
        switch (xmlNode->getNodeType())
        {
        case XMLNodeType::prolog:
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                checkElements(element.get());
            }
            break;
        case XMLNodeType::root:
        case XMLNodeType::element:
            if (xmlNode->getNodeType() == XMLNodeType::root && XMLNodeRef<XMLNodeElement>((*xmlNode)).name != m_dtd.getRootName())
            {
                throw XMLValidationError(m_lineNumber, "DOCTYPE name does not match that of root element " + XMLNodeRef<XMLNodeElement>((*xmlNode)).name + " of DTD.");
            }
            checkElement(static_cast<XMLNodeElement *>(xmlNode));
            for (auto &element : XMLNodeRef<XMLNodeElement>((*xmlNode)).children)
            {
                checkElements(element.get());
            }
            break;
        case XMLNodeType::self:
            checkElement(static_cast<XMLNodeElement *>(xmlNode));
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
            throw XMLValidationError(m_lineNumber, "Invalid XMLNode encountered during validation.");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XMLValidator::checkAgainstDTD(XMLNodeElement &prolog)
    {
        m_lineNumber = m_dtd.getLineCount();
        checkElements(&prolog);
        for (auto &idref : m_assignedIDREFValues)
        {
            if (m_assignedIDValues.find(idref) == m_assignedIDValues.end())
            {
                throw XMLValidationError(m_lineNumber, "IDREF attribute '" + idref + "' does not reference any element with the ID.");
            }
        }
    }
    /// <summary>
    /// Validate XML against its DTD. Throwing an exception if there is a
    /// issue and the XML is not well-formed.
    /// </summary>
    /// <param name="prolog">Prolog element containing root of XML to validate.</param>
    void XMLValidator::validate(XMLNodeElement &prolog)
    {
        checkAgainstDTD(prolog);
    }
} // namespace H4
