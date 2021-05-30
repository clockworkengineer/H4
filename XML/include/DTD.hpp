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
        // XML DTD attribute definition
        //
        struct DTDAttribute
        {
            std::string name;
            std::string type;
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
            bool idAttributePresent;
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
        std::vector<std::string> split(std::string strToSplit, char delimeter);
        void initialiseTables();
        void validateAttributes(XMLNodeDTD *dtd, XMLNodeElement *xNodeElement);
        void validateContentSpecification(XMLNodeDTD *dtd, XMLNodeElement *xNodeElement);
        void validateElement(XMLNodeDTD *dtd, XMLNodeElement *xNodeElement);
        void validateElements(XMLNodeDTD *dtd, XMLNode *xNodeRoot);
        bool validateIsPCDATA(XMLNodeElement *xNodeElement);
        bool validateIsEMPTY(XMLNodeElement *xNodeElement);
        void parseConditional(ISource &dtdSource, bool includeOff = true);
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
        void parsePostProcessing();
        void parseExternalRefenceContent();
        void parseAttributeList(ISource &dtdSource);
        void parseNotation(ISource &dtdSource);
        void parseEntity(ISource &dtdSource);
        void parseElement(ISource &dtdSource);
        XMLExternalReference parseExternalReference(ISource &dtdSource);
        std::string parseAttributeType(ISource &dtdSource);
        XMLValue parseAttributeValue(ISource &dtdSource);
        void parseComment(ISource &dtdSource);
        void parseParameterEntity(ISource &dtdSource);
        void parseExternal(ISource &dtdSource);
        void parseInternal(ISource &dtdSource);
        void parseDTD(ISource &dtdSource);
        void validateDTD(XMLNodeElement &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
        static std::vector<XMLString> m_dtdAttrListTypes;
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
        std::string m_unparsed;
    };
} // namespace H4
#endif /* DTD_DPP */
