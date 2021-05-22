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
    bool DTD::validateIsPCDATA(XNodeElement *xNodeElement)
    {
        for (auto &element : xNodeElement->children)
        {
            if ((XNodeRef<XNode>(*element).getNodeType() == XNodeType::element) ||
                (XNodeRef<XNode>(*element).getNodeType() == XNodeType::self))
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
    bool DTD::validateIsEMPTY(XNodeElement *xNodeElement)
    {
        return (xNodeElement->children.empty() || xNodeElement->getNodeType() == XNodeType::self);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateAttributes(XNodeDTD *dtd, XNodeElement *xNodeElement)
    {
        for (auto &attribute : dtd->dtd->elements[xNodeElement->name].attributes)
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
                    XAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed.empty()) // No character data present.
                    {
                        //throw ValidationError(dtd, "");
                    }
                }
            }
            else if (attribute.type == "ID")
            {
                if (xNodeElement->isAttributePresent(attribute.name))
                {
                    XAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed[0] != '_' &&
                        !std::isalpha(elementAttribute.value.parsed[0]) &&
                        elementAttribute.value.parsed[0] != ':')
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> ID attribute '" + elementAttribute.name + "' has a value that does not start with a letter, '_' or ':'.");
                    }
                    if (dtd->dtd->assignedIDValues.contains(elementAttribute.value.parsed))
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> ID attribute '" + elementAttribute.name + "' is not unique.");
                    }
                    dtd->dtd->assignedIDValues.insert(elementAttribute.value.parsed);
                }
            }
            else if (attribute.type == "IDREF")
            {
                if (xNodeElement->isAttributePresent(attribute.name))
                {
                    XAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (elementAttribute.value.parsed[0] != '_' &&
                        !std::isalpha(elementAttribute.value.parsed[0]) &&
                        elementAttribute.value.parsed[0] != ':')
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> IDREF attribute '" + elementAttribute.name + "' has a value that does not start with a letter, '_' or ':'.");
                    }
                    dtd->dtd->assignedIDREFValues.insert(elementAttribute.value.parsed);
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
                    XAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (!options.contains(elementAttribute.value.parsed))
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> attribute '" + attribute.name + "' contains invalid enumeration value '" + elementAttribute.value.parsed + "'.");
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
                    throw ValidationError(dtd, "Required attribute '" + attribute.name + "' missing for element <" + xNodeElement->name + ">.");
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
                    XAttribute elementAttribute = xNodeElement->getAttribute(attribute.name);
                    if (attribute.value.parsed.substr(7) != elementAttribute.value.parsed)
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> attribute '" + attribute.name + "' is '" + elementAttribute.value.parsed + "' instead of '" + attribute.value.parsed.substr(7) + "'.");
                    }
                }
                else
                {
                    XValue value;
                    value.parsed = value.unparsed = attribute.value.parsed.substr(7);
                    xNodeElement->addAttribute(attribute.name, value);
                }
            }
            else
            {
                if (!xNodeElement->isAttributePresent(attribute.name))
                {
                    XValue value;
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
    void DTD::validateContentSpecification(XNodeDTD *dtd, XNodeElement *xNodeElement)
    {
        if ((dtd == nullptr) || (dtd->dtd->elements.empty()))
        {
            return;
        }
        if (dtd->dtd->elements[xNodeElement->name].content.parsed == "((<#PCDATA>))")
        {
            if (!validateIsPCDATA(xNodeElement))
            {
                throw ValidationError(dtd, "Element <" + xNodeElement->name + "> does not contain just any parsable data.");
            }
            return;
        }
        if (dtd->dtd->elements[xNodeElement->name].content.parsed == "EMPTY")
        {
            if (!validateIsEMPTY(xNodeElement))
            {
                throw ValidationError(dtd, "Element <" + xNodeElement->name + "> is not empty.");
            }
            return;
        }
        if (dtd->dtd->elements[xNodeElement->name].content.parsed == "ANY")
        {
            return;
        }
        std::regex match(dtd->dtd->elements[xNodeElement->name].content.parsed);
        std::string elements;
        for (auto &element : xNodeElement->children)
        {
            if ((XNodeRef<XNode>(*element).getNodeType() == XNodeType::element) ||
                (XNodeRef<XNode>(*element).getNodeType() == XNodeType::self))
            {
                elements += "<" + XNodeRef<XNodeElement>(*element).name + ">";
            }
            else if (XNodeRef<XNode>(*element).getNodeType() == XNodeType::content)
            {
                if (!XNodeRef<XNodeContent>(*element).isWhiteSpace)
                {
                    elements += "<#PCDATA>";
                }
            }
        }
        if (!std::regex_match(elements, match))
        {
            throw ValidationError(dtd, "<" + xNodeElement->name + "> element does not conform to the content specification " +
                                           dtd->dtd->elements[xNodeElement->name].content.unparsed + ".");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateElement(XNodeDTD *dtd, XNodeElement *xNodeElement)
    {
        validateContentSpecification(dtd, xNodeElement);
        validateAttributes(dtd, xNodeElement);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::validateElements(XNodeDTD *dtd, XNode *xNode)
    {
        switch (xNode->getNodeType())
        {
        case XNodeType::prolog:
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).children)
            {
                validateElements(dtd, element.get());
            }
            break;
        case XNodeType::root:
            if (XNodeRef<XNodeElement>((*xNode)).name != dtd->name)
            {
                throw ValidationError(dtd, "DOCTYPE name does not match that of root element " + XNodeRef<XNodeElement>((*xNode)).name + " of DTD.");
            }
        case XNodeType::element:
            validateElement(dtd, static_cast<XNodeElement *>(xNode));
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).children)
            {
                validateElements(dtd, element.get());
            }
            break;
        case XNodeType::self:
            validateElement(dtd, static_cast<XNodeElement *>(xNode));
            break;
        case XNodeType::comment:
        case XNodeType::entity:
        case XNodeType::pi:
        case XNodeType::cdata:
        case XNodeType::dtd:
            break;
        case XNodeType::content:
            for (auto &ch : XNodeRef<XNodeContent>((*xNode)).content)
            {
                if (ch == kLineFeed)
                {
                    dtd->dtd->lineNumber++;
                }
            }
            break;
        default:
            throw ValidationError(dtd, "Invalid XNode encountered during validation.");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void DTD::dtdValidateDTD(XNodeElement &prolog)
    {
        if (prolog.getNodeType() == XNodeType::prolog)
        {
            XNodeDTD *dtd;
            for (auto &element : prolog.children)
            {
                if (element->getNodeType() == XNodeType::dtd)
                {
                    dtd = static_cast<XNodeDTD *>(element.get());
                    break;
                }
            }
            if (dtd != nullptr)
            {
                validateElements(dtd, &prolog);
                for (auto &idref : dtd->dtd->assignedIDREFValues)
                {
                    if (!dtd->dtd->assignedIDValues.contains(idref))
                    {
                        throw ValidationError(dtd, "IDREF attribute '"+idref+"' does not reference any element with the ID.");
                    }
                }
            }
        }
    }
} // namespace H4
