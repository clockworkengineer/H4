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
    void XML::validateElement(XNodeDTD *dtd, XNodeElement *xNodeElement)
    {
        if ((dtd == nullptr) || (dtd->elements.empty()))
        {
            return;
        }
        if (dtd->elements.count(xNodeElement->name) == 0)
        {
            throw std::runtime_error("Undefined element <" + xNodeElement->name + "> found.");
        }
        if (dtd->elements[xNodeElement->name].content == "(#PCDATA)")
        {
            return;
        }
        std::string regExp;
        for (auto ch : dtd->elements[xNodeElement->name].content)
        {
            if (ch != ',')
            {
                regExp += ch;
            }
        }
        std::regex match(regExp);
        std::string elements;
        for (auto &element : xNodeElement->elements)
        {
            if (XNodeRef<XNode>(*element).getNodeType() == XNodeType::element)
            {
                elements += XNodeRef<XNodeElement>(*element).name;
            }
        }
        if (!std::regex_match(elements, match))
        {
            throw std::runtime_error("Invalid <" + xNodeElement->name + "> element found.");
        }
    }
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
        case XNodeType::element:
            for (auto &element : XNodeRef<XNodeElement>((*xNode)).elements)
            {
                vadlidateElements(dtd, element.get());
            }
            validateElement(dtd, static_cast<XNodeElement *>(xNode));
            break;
        case XNodeType::self:
            validateElement(dtd, static_cast<XNodeElement *>(xNode));
            break;
        case XNodeType::comment:
        case XNodeType::content:
        case XNodeType::entity:
        case XNodeType::pi:
        case XNodeType::cdata:
        case XNodeType::dtd:
            break;
        default:
            throw std::runtime_error("Invalid XNode encountered during validation.");
        }
    }
    void XML::validateXML(XNode *xNode)
    {
        if (xNode->getNodeType() == XNodeType::prolog)
        {
            for (auto &element : XNodeRef<XNodeElement>(*xNode).elements)
            {
                if (element->getNodeType() == XNodeType::dtd)
                {
                    vadlidateElements(static_cast<XNodeDTD *>(element.get()), xNode);
                }
            }
        }
    }
} // namespace H4
