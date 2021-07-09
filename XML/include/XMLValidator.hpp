#ifndef XMLVALIDATOR_HPP
#define XMLVALIDATOR_HPP
//
// C++ STL
//
#include <string>
#include <unordered_map>
//
// Source/Destination interfaces
//
#include "ISource.hpp"
#include "IDestination.hpp"
//
// DTD Core
//
#include "XML_core.hpp"
//
// XML XNodes
//
#include "XMLNode.hpp"
//
//
//
#include "DTD.hpp"
//
// DTD element/attrribute
//
#include "DTDElement.hpp"
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class XMLValidator
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        //
        // DTD Type
        //
        enum DTDType : uint16_t
        {
            internal = (0x1 << 0),
            external = (0x1 << 1)
        };
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
        void validateAttributes(XMLNodeDTD *dtd, XMLNodeElement *xNodeElement);
        void validateContentSpecification(XMLNodeDTD *dtd, XMLNodeElement *xNodeElement);
        void validateElement(XMLNodeDTD *dtd, XMLNodeElement *xNodeElement);
        void validateElements(XMLNodeDTD *dtd, XMLNode *xNodeRoot);
        bool validateIsNMTOKENOK(std::string nmTokenValue);
        bool validateIsIDOK(const std::string &idValue);
        void validateAttributeValue(XMLNodeElement *xmlNodeElement, DTDAttribute &attribute);
        void validateAttributeType(XMLNodeElement *xmlNodeElement, DTDAttribute &attribute);
        bool validateIsPCDATA(XMLNodeElement *xNodeElement);
        bool validateIsEMPTY(XMLNodeElement *xNodeElement);
        void validateDTD(XMLNodeElement &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
     //   uint16_t m_type = 0;
     //   std::string m_name;
     //   XMLExternalReference m_external;
     //   std::unordered_map<std::string, DTDElement> m_elements;
     //   std::unordered_map<std::string, XMLExternalReference> m_notations;
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
  //      std::string m_unparsed;
        long m_lineNumber = 1;
//IXMLEntityMapper &m_entityMapper;
        DTD &m_dtd;
    };
} // namespace H4
#endif /* XMLVALIDATOR_HPP */
