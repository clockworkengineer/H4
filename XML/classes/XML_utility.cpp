//
// Class: XML
//
// Description: XML utility functions.
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
    std::vector<std::string> XML::split(std::string strToSplit, char delimeter)
    {
        std::stringstream ss(strToSplit);
        std::string item;
        std::vector<std::string> splittedStrings;
        while (std::getline(ss, item, delimeter))
        {
            splittedStrings.push_back(item);
        }
        return splittedStrings;
    }
    /// <summary>
    /// Check whether a attribute exists in list of attributes.
    /// </summary>
    /// <param name="attributeList">Vector of attribute name/value pairs.</param>
    /// <param name="attributeName">Attribute name to check for in list.</param>
    /// <returns>true if attribute name is in list.</returns>
    bool XML::isAttributePresent(std::vector<XAttribute> attributeList, const std::string &attributeName)
    {
        return (std::find_if(attributeList.rbegin(), attributeList.rend(),
                             [&attributeName](const XAttribute &attr) { return (attr.name == attributeName); }) != attributeList.rend());
    }
    /// <summary>
    /// Return attribute value if it is present in list.
    /// </summary>
    /// <param name="attributeList">Vector of attribute name/value pairs.</param>
    /// <param name="attributeName">Attribute name to get in list.</param>
    /// <returns>attribute value if attribute name is in list.</returns>
    XAttribute XML::getAttribute(std::vector<XAttribute> attributeList, const std::string &attributeName)
    {
        return (*std::find_if(attributeList.rbegin(), attributeList.rend(),
                              [&attributeName](const XAttribute &attr) { return (attr.name == attributeName); }));
    }
} // namespace H4