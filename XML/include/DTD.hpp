#ifndef DTD_DPP
#define DTD_DPP
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
    class DTD
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        //
        // XML syntax error.
        //
        struct SyntaxError : public std::exception
        {
        public:
            SyntaxError(const std::string &description = "")
            {
                errorMessage = "DTD Syntax Error: " + description;
            }
            SyntaxError(ISource &xmlSource, const std::string &description = "")
            {
                errorMessage = "DTD Syntax Error [Line: " + std::to_string(xmlSource.getLineNo()) +
                               " Column: " + std::to_string(xmlSource.getColumnNo()) + "] " + description;
            }
            virtual const char *what() const throw()
            {
                return (errorMessage.c_str());
            }

        private:
            std::string errorMessage;
        };
        //
        // XML validation error
        //
        struct ValidationError : public std::exception
        {
        public:
            ValidationError(DTD &dtd, const std::string &description = "")
            {
                errorMessage = "XML Validation Error [Line: " + std::to_string(dtd.m_lineNumber) + "] " + description;
            }
            virtual const char *what() const throw()
            {
                return (errorMessage.c_str());
            }

        private:
            std::string errorMessage;
        };
        // //
        // // XML DTD attribute types. Note only one type bit and
        // // value bit should be set for each attribute and that
        // // some combinations like id and fixed are illegal and
        // // are reported as so by the parser.
        // //
        // enum DTDAttributeType : uint16_t
        // {
        //     // Types bits 0 - 9
        //     cdata = (0x1 << 0),
        //     enumeration = (0x1 << 1),
        //     id = (0x1 << 2),
        //     idref = (0x1 << 3),
        //     idrefs = (0x1 << 4),
        //     nmtoken = (0x1 << 5),
        //     nmtokens = (0x1 << 6),
        //     entity = (0x1 << 7),
        //     entities = (0x1 << 8),
        //     notation = (0x1 << 9),
        //     // Value bits 10 - 13
        //     normal = (0x1 << 10),
        //     required = (0x1 << 11),
        //     implied = (0x1 << 12),
        //     fixed = (0x1 << 13)
        // };
        // //
        // // XML DTD attribute definition
        // //
        // struct DTDAttribute
        // {
        //     std::string name;
        //     uint16_t type;
        //     std::string enumeration;
        //     XMLValue value;
        // };
        // //
        // // XML DTD element definition
        // //
        // struct DTDElement
        // {
        //     DTDElement() {}
        //     DTDElement(const std::string &name, const XMLValue &content) : name(name), content(content) {}
        //     std::string name;
        //     bool idAttributePresent = false;
        //     XMLValue content;
        //     std::vector<DTDAttribute> attributes;
        // };
        // ============
        // CONSTRUCTORS
        // ============
        DTD()
        {
            initialiseTables();
        }
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        void parse(ISource &dtdSource);
        void stringify(IDestination &xmlDestination);
        void validate(XMLNodeElement &prolog);
        // ================
        // PUBLIC VARIABLES
        // ================
        std::string m_name;
        XMLEntityMappings m_entityMapping;
        XMLExternalReference m_external;
        std::unordered_map<std::string, XMLExternalReference> m_notations;
        std::unordered_map<std::string, DTDElement> m_elements;
        long m_lineNumber = 1;

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
        void checkForEntityRecursion(const std::string &entityName, std::set<std::string> names = {});
        void initialiseTables();
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
        void parseConditional(ISource &dtdSource, bool includeOff = true);
        void parseValidNotations(const std::string &notations);
        void parseValidateAttribute(const std::string &elementName, DTDAttribute xDTDAttribute);
        void parseParameterEntities(ISource &dtdSource);
        void parseExternalContent(ISource &dtdSource);
        std::string parseAttributeEnumerationType(ISource &xmlSource);
        bool parseIsChoiceOrSequence(ISource &contentSpecSource);
        void parseElementCP(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementChoice(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementSequence(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementChildren(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementName(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementContentSpecification(const std::string &elementName, XMLValue &contentSpec);
        void parseExternalRefenceContent();
        void parseAttributeList(ISource &dtdSource);
        void parseNotation(ISource &dtdSource);
        void parseEntity(ISource &dtdSource);
        void parseElement(ISource &dtdSource);
        XMLExternalReference parseExternalReference(ISource &dtdSource);
        void parseAttributeType(ISource &dtdSource, DTDAttribute &attribute);
        void parseAttributeValue(ISource &dtdSource, DTDAttribute &attribute);
        void parseComment(ISource &dtdSource);
        void parseParameterEntityReference(ISource &dtdSource);
        void parseExternal(ISource &dtdSource);
        void parseInternal(ISource &dtdSource);
        void parseDTD(ISource &dtdSource);
        void validateDTD(XMLNodeElement &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
        std::string m_unparsed;
    };
} // namespace H4
#endif /* DTD_DPP */
