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
// #include <tuple>
//
// XML Core
//
#include "XML_core.hpp"
//
// XML DTD
// 
#include "DTD.hpp"
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
                errorMessage = "XML Validation Error [Line: " + std::to_string(dtd->dtd->lineNumber) + "] " + description;
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
        DTD m_dtd;

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
        void initialiseTables();
        bool validAttributeValue(XValue &value);
        void validXMLDeclaration(ISource &xmlSource, XNodeElement *xNodeElement);
        void xmlParseAddElementContent(XNodeElement *xNodeElement, const std::string &content);
        void xmlParseEntityMappingContents(XNodeElement *xNodeElement, XValue &entityReference);
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
