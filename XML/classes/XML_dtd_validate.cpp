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
    bool XML::isPCDATA(XNodeElement *xNodeElement)
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
    bool XML::isEMPTY(XNodeElement *xNodeElement)
    {
        return (xNodeElement->elements.empty() || xNodeElement->getNodeType() == XNodeType::self);
    }
    /// <summary>
    ///
    /// </summary>
    /// <param name=""></param>
    /// <returns></returns>
    void XML::validateElement(XNodeDTD *dtd, XNodeElement *xNodeElement)
    {
        if ((dtd == nullptr) || (dtd->elements.empty()))
        {
            return;
        }
        if (dtd->elements[xNodeElement->name].content.parsed == "((<#PCDATA>))")
        {
            if (!isPCDATA(xNodeElement))
            {
                throw ValidationError(dtd, "Element <" + xNodeElement->name + "> does not contain just any parsable data.");
            }
            return;
        }
        if (dtd->elements[xNodeElement->name].content.parsed == "EMPTY")
        {
            if (!isEMPTY(xNodeElement))
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
            throw ValidationError(dtd, "<" + xNodeElement->name + "> element does not conform to the content specication " +
                                           dtd->elements[xNodeElement->name].content.unparsed + ".");
        }
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
            for (auto &element : XNodeRef<XNodeElement>(*xNode).elements)
            {
                if (element->getNodeType() == XNodeType::dtd)
                {
                    vadlidateElements(static_cast<XNodeDTD *>(element.get()), xNode);
                    break;
                }
            }
        }
    }
} // namespace H4
