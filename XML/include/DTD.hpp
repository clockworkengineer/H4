#ifndef DTD_DPP
#define DTD_DPP
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <set>
// #include <tuple>
//
// DTD Core
//
#include "XML_core.hpp"
//
// DTD XNodes
//
#include "XNode.hpp"
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
        struct XDTDAttribute
        {
            std::string name;
            std::string type;
            XValue value;
        };
        //
        // XML DTD element definition
        //
        struct XDTDElement
        {
            XDTDElement() {}
            XDTDElement(const std::string &name, const XValue &content) : name(name), content(content) {}
            std::string name;
            XValue content;
            std::vector<XDTDAttribute> attributes;
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
        void validate(XNodeElement &prolog);
        // ================
        // PUBLIC VARIABLES
        // ================
        std::string m_name;
        XEntityMappings m_entityMapping;
        XExternalReference m_external;
        std::unordered_map<std::string, XExternalReference> m_notations;
        std::unordered_map<std::string, XDTDElement> m_elements;
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
        void validateAttributes(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void validateContentSpecification(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void validateElement(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void validateElements(XNodeDTD *dtd, XNode *xNodeRoot);
        bool validateIsPCDATA(XNodeElement *xNodeElement);
        bool validateIsEMPTY(XNodeElement *xNodeElement);
        void parseParameterENTITIES(ISource &dtdSource);
        void parseTranslateParameterENTITIES(ISource &dtdSource);
        std::string parseTranslateParameterEntities(const std::string &parameterEntities);
        std::string parseAttributeEnumerationType(ISource &xmlSource);
        bool parseIsChoiceOrSequence(ISource &contentSpecSource);
        void parseElementCP(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementChoice(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementSequence(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementChildren(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementName(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpec);
        void parseElementContentSpecification(XValue &contentSpec);
        void parsePostProcessing();
        void parseExternalContents();
        void parseAttributeList(ISource &dtdSource);
        void parseNotation(ISource &dtdSource);
        void parseEntity(ISource &dtdSource);
        void parseElement(ISource &dtdSource);
        XExternalReference parseExternalReference(ISource &dtdSource);
        std::string parseAttributeType(ISource &dtdSource);
        XValue parseAttributeValue(ISource &dtdSource);
        void parseComment(ISource &dtdSource);
        void parseParameterEntity(ISource &dtdSource);
        void parseExternal(ISource &dtdSource);
        void parseInternal(ISource &dtdSource);
        void parseDTD(ISource &dtdSource);
        void validateDTD(XNodeElement &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
        static std::vector<XString> m_dtdAttrListTypes;
        std::set<std::string> m_assignedIDValues;
        std::set<std::string> m_assignedIDREFValues;
        std::string m_unparsed;
    };
} // namespace H4
#endif /* DTD_DPP */
