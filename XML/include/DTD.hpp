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
        // DTD Type
        //
        enum DTDType : uint16_t
        {
            internal = (0x1 << 0),
            external = (0x1 << 1)
        };
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
        uint16_t getType();
        std::string &getRootName();
        XMLExternalReference &getExternalReference();
        bool isElementPresent(const std::string &elementName);
        DTDElement &getElement(const std::string &elementName);
        XMLEntityMapping &getEntity(const std::string &entityName);
        XMLExternalReference &getNotation(const std::string &notationName);
        void parse(ISource &dtdSource);
        void stringify(IDestination &xmlDestination);
        void validate(XMLNodeElement &prolog);
        // ================
        // PUBLIC VARIABLES
        // ================
        // std::string m_name;
        // XMLExternalReference m_external;
        XMLEntityMappings m_entityMapping;

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
        uint16_t m_type = 0;
        std::string m_name;
        XMLExternalReference m_external;
        std::unordered_map<std::string, DTDElement> m_elements;
        std::unordered_map<std::string, XMLExternalReference> m_notations;
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
        std::string m_unparsed;
        long m_lineNumber = 1;
    };
} // namespace H4
#endif /* DTD_DPP */
