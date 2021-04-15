//
// Class: XML
//
// Description: XML Document Type Definition parsing.
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
    bool XML::vadlidateIsPCDATA(XNodeElement *xNodeElement)
    {
        for (auto &element : xNodeElement->elements)
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
    bool XML::validateIsEMPTY(XNodeElement *xNodeElement)
    {
        return (xNodeElement->elements.empty() || xNodeElement->getNodeType() == XNodeType::self);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::validateAttributes(XNodeDTD *dtd, XNodeElement *xNodeElement)
    {
        for (auto &attribute : dtd->elements[xNodeElement->name].attributes)
        {
            // Validate a elements attribute type which can be one of the following.
            //
            // CDATA	    The value is character data
            // (en1|en2|..)	The value must be one from an enumerated list
            // ID	        The value is a unique id
            // IDREF        The value is the id of another element
            // IDREFS       The value is a list of other ids
            // NMTOKEN      The value is a valid XML name
            // NMTOKENS	    The value is a list of valid XML names
            // ENTIT        The value is an entity
            // ENTITIES	    The value is a list of entities
            // NOTATION	    The value is a name of a notation
            // xml:         The value is a predefined xml value
            //
            // AT PRESENT ONLY ENUMERATION DEALT WITH.
            if (attribute.type == "CDATA")
            {
                if (isAttributePresent(xNodeElement->attributes, attribute.name))
                {
                    XAttribute elementAttribute = getAttribute(xNodeElement->attributes, attribute.name);
                    if (elementAttribute.value.parsed.empty()) // No character data present.
                    {
                        //throw ValidationError(dtd, "");
                    }
                }
            }
            else if (attribute.type == "ID")
            {
                if (isAttributePresent(xNodeElement->attributes, attribute.name))
                {
                    XAttribute elementAttribute = getAttribute(xNodeElement->attributes, attribute.name);
                    if (elementAttribute.value.parsed[0] != '_' &&
                        !std::isalpha(elementAttribute.value.parsed[0]) &&
                        elementAttribute.value.parsed[0] != ':')
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> ID attribute '" + elementAttribute.name + "' has a value that does not start with a letter, '_' or ':'.");
                    }
                    if (dtd->assignedIDValues.contains(elementAttribute.value.parsed))
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> ID attribute '" + elementAttribute.name + "' is not unique.");
                    }
                    dtd->assignedIDValues.insert(elementAttribute.value.parsed);
                }
            }
            else if (attribute.type == "IDREF")
            {
                if (isAttributePresent(xNodeElement->attributes, attribute.name))
                {
                    XAttribute elementAttribute = getAttribute(xNodeElement->attributes, attribute.name);
                    if (elementAttribute.value.parsed[0] != '_' &&
                        !std::isalpha(elementAttribute.value.parsed[0]) &&
                        elementAttribute.value.parsed[0] != ':')
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> IDREF attribute '" + elementAttribute.name + "' has a value that does not start with a letter, '_' or ':'.");
                    }
                    dtd->assignedIDREFValues.insert(elementAttribute.value.parsed);
                }
            }
            else if (attribute.type[0] == '(')
            {
                std::set<std::string> options;
                for (auto &option : split(attribute.type.substr(1, attribute.type.size() - 2), '|'))
                {
                    options.insert(option);
                }
                if (isAttributePresent(xNodeElement->attributes, attribute.name))
                {
                    XAttribute elementAttribute = getAttribute(xNodeElement->attributes, attribute.name);
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
                if (!isAttributePresent(xNodeElement->attributes, attribute.name))
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
                if (isAttributePresent(xNodeElement->attributes, attribute.name))
                {
                    XAttribute elementAttribute = getAttribute(xNodeElement->attributes, attribute.name);
                    if (attribute.value.parsed.substr(7) != elementAttribute.value.parsed)
                    {
                        throw ValidationError(dtd, "Element <" + xNodeElement->name + "> attribute '" + attribute.name + "' is '" + elementAttribute.value.parsed + "' instead of '" + attribute.value.parsed.substr(7) + "'.");
                    }
                }
                else
                {
                    XValue value;
                    value.parsed = value.unparsed = attribute.value.parsed.substr(7);
                    xNodeElement->attributes.emplace_back(attribute.name, value);
                }
            }
            else
            {
                if (!isAttributePresent(xNodeElement->attributes, attribute.name))
                {
                    XValue value;
                    value.parsed = value.unparsed = attribute.value.parsed;
                    xNodeElement->attributes.emplace_back(attribute.name, value);
                }
            }
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::validateContentSpecification(XNodeDTD *dtd, XNodeElement *xNodeElement)
    {
        if ((dtd == nullptr) || (dtd->elements.empty()))
        {
            return;
        }
        if (dtd->elements[xNodeElement->name].content.parsed == "((<#PCDATA>))")
        {
            if (!vadlidateIsPCDATA(xNodeElement))
            {
                throw ValidationError(dtd, "Element <" + xNodeElement->name + "> does not contain just any parsable data.");
            }
            return;
        }
        if (dtd->elements[xNodeElement->name].content.parsed == "EMPTY")
        {
            if (!validateIsEMPTY(xNodeElement))
            {
                throw ValidationError(dtd, "Element <" + xNodeElement->name + "> is not empty.");
            }
            return;
        }
        if (dtd->elements[xNodeElement->name].content.parsed == "ANY")
        {
            return;
        }
        std::regex match(dtd->elements[xNodeElement->name].content.parsed);
        std::string elements;
        for (auto &element : xNodeElement->elements)
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
                                           dtd->elements[xNodeElement->name].content.unparsed + ".");
        }
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::validateElement(XNodeDTD *dtd, XNodeElement *xNodeElement)
    {
        validateContentSpecification(dtd, xNodeElement);
        validateAttributes(dtd, xNodeElement);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::vadlidateElements(XNodeDTD *dtd, XNode *xNode)
    {
        switch (xNode->getNodeType())
        {
        case XNodeType::prolog:
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).elements)
            {
                vadlidateElements(dtd, element.get());
            }
            break;
        case XNodeType::root:
            if (XNodeRef<XNodeElement>((*xNode)).name != dtd->name)
            {
                throw ValidationError(dtd, "DOCTYPE name does not match that of root element " + XNodeRef<XNodeElement>((*xNode)).name + " of XML.");
            }
        case XNodeType::element:
            validateElement(dtd, static_cast<XNodeElement *>(xNode));
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).elements)
            {
                vadlidateElements(dtd, element.get());
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
                    dtd->lineNumber++;
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
    void XML::validateXML(XNode *xNode)
    {
        if (xNode->getNodeType() == XNodeType::prolog)
        {
            XNodeDTD *dtd;
            for (auto &element : XNodeRef<XNodeElement>(*xNode).elements)
            {
                if (element->getNodeType() == XNodeType::dtd)
                {
                    dtd = static_cast<XNodeDTD *>(element.get());
                    break;
                }
            }
            if (dtd != nullptr)
            {
                vadlidateElements(dtd, xNode);
                for (auto &idref : dtd->assignedIDREFValues)
                {
                    if (!dtd->assignedIDValues.contains(idref))
                    {
                        throw ValidationError(dtd, "IDREF attribute '"+idref+"' does not reference any element with the ID.");
                    }
                }
            }
        }
    }
} // namespace H4
