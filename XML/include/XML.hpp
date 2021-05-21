#ifndef XML_HPP
#define XML_HPP
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <memory>
#include <unordered_map>
#include <set>
#include <tuple>
//
// XML Core
//
#include "XML_core.hpp"
//
// XML XNodes
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
    class XML
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        //
        // XML syntax error.
        //
        // class ISource;
        struct SyntaxError : public std::exception
        {
        public:
            SyntaxError(const std::string &description = "")
            {
                errorMessage = "XML Syntax Error: " + description;
            }
            SyntaxError(ISource &xmlSource, const std::string &description = "")
            {
                errorMessage = "XML Syntax Error [Line: " + std::to_string(xmlSource.getLineNo()) +
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
            ValidationError(XNodeDTD *dtd, const std::string &description = "")
            {
                errorMessage = "XML Validation Error [Line: " + std::to_string(dtd->lineNumber) + "] " + description;
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
        XML(ISource &xmlSource) : m_xmlSource(xmlSource)
        {
            initialiseTables();
        }
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        void parse();
        void stringify(IDestination &xmlDestination);
        void validate();
        // ================
        // PUBLIC VARIABLES
        // ================
        XNodeElement m_prolog;
        std::unordered_map<std::string, XEntityMapping> m_entityMapping;

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
        void stringifyElements(XNode *xNode, IDestination &xmlDestination);
        void checkForEntityRecursion(XNodeDTD *xNodeDTD, const std::string &entityName, std::set<std::string> names = {});
        std::vector<std::string> split(std::string strToSplit, char delimeter);
        void initialiseTables();
        bool validAttributeValue(XValue &value);
        void validXMLDeclaration(ISource &xmlSource, XNodeElement *xNodeElement);
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
        void xmlParseAddElementContent(XNodeElement *xNodeElement, const std::string &content);
        void xmlParseEntityMappingContents(XNodeElement *xNodeElement, XValue &entityReference);
        void mapEntityReference(XValue &entityReference);
        XValue xmlParseValue(ISource &xmlSource, bool translateEntity = true);
        XValue xmlParseCharacter(ISource &xmlSource,  bool translateEntity = true);
        void xmlParseDefault(ISource &xmlSource, XNodeElement *xNodeElement);
        void xmlParseTagName(ISource &xmlSource, XNodeElement *xNodeElement);
        void xmlParseChildElement(ISource &xmlSource, XNodeElement *xNodeElement);
        void xmlParseAttributes(ISource &xmlSource, XNodeElement *xNodeElement);
        void xmlParseComment(ISource &xmlSource, XNodeElement *xNodeElement);
        void xmlParseCDATA(ISource &xmlSource, XNodeElement *xNodeElement);
        void xmlParsePI(ISource &xmlSource, XNodeElement *xNodeElement);
        void xmlParseElementContents(ISource &xmlSource, XNodeElement *XNodeElement);
        void xmlParseElement(ISource &xmlSource, XNodeElement *XNodeElement);
        void xmlParseProlog(ISource &xmlSource, XNodeElement *xNodeProlog);
        void dtdValidateXML();
        void xmlParse();
        void stringifyXML(IDestination &xmlDestination);
        // =================
        // PRIVATE VARIABLES
        // =================
        static XAttribute m_defaultAtributes[3];
        static std::vector<XString> m_dtdAttrListTypes;
        ISource &m_xmlSource;
    };
} // namespace H4
#endif /* XML_HPP */
