#ifndef DTD_DPP
#define DTD_DPP
//
// DTD Core
//
#include "XML_core.hpp"
//
// DTD XNodes
//
#include "XMLNode.hpp"
//
// Source/Destination interfaces
//
#include "ISource.hpp"
#include "IDestination.hpp"
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
        //
        // XML DTD attribute types
        //
        enum DTDAttributeType
        {
            none = 0,
            cdata = 1,
            enumeration = 2,
            id = 3,
            idref = 4,
            idrefs = 5,
            nmtoken = 6,
            nmtokens = 7,
            entity = 8,
            entities = 9,
            notation = 10
        };
        //
        // XML DTD attribute definition
        //
        struct DTDAttribute
        {
            std::string name;
            DTDAttributeType  type = none;
            std::string enumeration;
            XMLValue value;
        };
        //
        // XML DTD element definition
        //
        struct DTDElement
        {
            DTDElement() {}
            DTDElement(const std::string &name, const XMLValue &content) : name(name), content(content) {}
            std::string name;
            bool idAttributePresent = false;
            XMLValue content;
            std::vector<DTDAttribute> attributes;
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
