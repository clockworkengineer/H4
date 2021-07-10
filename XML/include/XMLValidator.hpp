#ifndef XMLVALIDATOR_HPP
#define XMLVALIDATOR_HPP
//
// C++ STL
//
#include <string>
#include <unordered_map>
#include <set>
//
// Source/Destination interfaces
//
#include "ISource.hpp"
#include "IDestination.hpp"
//
// XML Core
//
#include "XML_core.hpp"
//
// XML XNodes
//
#include "XMLNode.hpp"
//
// XML DTD
//
#include "DTD.hpp"
#include "DTDElement.hpp"
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class XMLValidator : public IXMLValidator
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        // ============
        // CONSTRUCTORS
        // ============
        XMLValidator(DTD &dtd) : m_dtd(dtd) {}
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        void validate(XMLNodeElement &prolog);
        // ================
        // PUBLIC VARIABLES
        // ================
    private:
        // ===========================
        // PRIVATE TYPES AND CONSTANTS
        // ===========================
        // ===========================================
        // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
        // ===========================================
        // ===============
        // PRIVATE METHODS
        // ===============
        void checkAttributes(XMLNodeElement *xNodeElement);
        void checkContentSpecification(XMLNodeElement *xNodeElement);
        void checkElement(XMLNodeElement *xNodeElement);
        void checkElements(XMLNode *xNodeRoot);
        bool checkIsNMTOKENOK(std::string nmTokenValue);
        bool checkIsIDOK(const std::string &idValue);
        void checkAttributeValue(XMLNodeElement *xmlNodeElement, DTDAttribute &attribute);
        void checkAttributeType(XMLNodeElement *xmlNodeElement, DTDAttribute &attribute);
        bool checkIsPCDATA(XMLNodeElement *xNodeElement);
        bool checkIsEMPTY(XMLNodeElement *xNodeElement);
        void checkAgainstDTD(XMLNodeElement &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
        long m_lineNumber = 1;
        DTD &m_dtd;
    };
} // namespace H4
#endif /* XMLVALIDATOR_HPP */
