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
        // DTD validation error
        //
        struct ValidationError : public std::exception
        {
        public:
            ValidationError(XNodeDTD *dtd, const std::string &description = "")
            {
                errorMessage = "XML Validation Error [Line: " + std::to_string( dtd->dtd->lineNumber) + "] " + description;
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
        void parse(ISource &dtdSource, XNodeElement *xNodeElement);
        void stringify(IDestination &xmlDestination);
        void validate(XNodeElement &prolog);
        // ================
        // PUBLIC VARIABLES
        // ================
        XEntityMappings m_entityMapping;
        XExternalReference external;
        std::unordered_map<std::string, XExternalReference> notations;
        std::unordered_map<std::string, XDTDElement> elements;
        XEntityMappings entityMapping;
        std::set<std::string> assignedIDValues;
        std::set<std::string> assignedIDREFValues;
        std::string unparsed;
        long lineNumber = 1;

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
        void checkForEntityRecursion(XNodeDTD *xNodeDTD, const std::string &entityName, std::set<std::string> names = {});
        std::vector<std::string> split(std::string strToSplit, char delimeter);
        void initialiseTables();
        void dtdParseParameterENTITIES(XNodeDTD *xNodeDTD, ISource &dtdSource);
        void dtdParseTranslateParameterENTITIES(XNodeDTD *xNodeDTD, ISource &dtdSource);
        void dtdValidateAttributes(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void dtdValidateContentSpecification(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void dtdValidateElement(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void dtdVadlidateElements(XNodeDTD *dtd, XNode *xNodeRoot);
        bool dtdVadlidateIsPCDATA(XNodeElement *xNodeElement);
        bool dtdValidateIsEMPTY(XNodeElement *xNodeElement);
        std::string dtdParseTranslateParameterEntities(XNodeDTD *xNodeDTD, const std::string &parameterEntities);
        std::string dtdParseAttributeEnumerationType(ISource &xmlSource);
        bool dtdParseIsChoiceOrSequence(ISource &contentSpecSource);
        void dtdParseElementCP(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementChoice(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementSequence(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementChildren(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementName(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementContentSpecification(XNodeDTD *xNodeDTD, XValue &contentSpec);
        void dtdParsePostProcessing(XNodeDTD *xNodeDTD);
        void dtdParseExternalContents(XNodeDTD *xNodeDTD);
        void dtdParseAttributeList(ISource &dtdSource, XNodeDTD *xNodeDTD);
        void dtdParseNotation(ISource &dtdSource, XNodeDTD *xNodeDTD);
        void dtdParseEntity(ISource &dtdSource, XNodeDTD *xNodeDTD);
        void dtdParseElement(ISource &dtdSource, XNodeDTD *xNodeElement);
        XExternalReference dtdParseExternalReference(ISource &dtdSource);
        std::string dtdParseAttributeType(ISource &dtdSource);
        XValue dtdParseAttributeValue(ISource &dtdSource);
        void dtdParseComment(ISource &dtdSource, XNodeDTD *xNodeDTD);
        void dtdParseParameterEntity(ISource &dtdSource, XNodeDTD *xNodeDTD);
        void dtdParseExternal(ISource &dtdSource, XNodeDTD *xNodeDTD);
        void dtdParseInternal(ISource &dtdSource, XNodeDTD *xNodeDTD);
        void dtdParse(ISource &dtdSource, XNodeElement *XNodeElement);
        void dtdValidateDTD(XNodeElement &prolog);
        // =================
        // PRIVATE VARIABLES
        // =================
        // static XAttribute m_defaultAtributes[3];
        static std::vector<XString> m_dtdAttrListTypes;
    };
} // namespace H4
#endif /* DTD_DPP */
