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
                errorMessage = "XML Validation Error [Line: " + std::to_string(dtd->dtd->m_lineNumber) + "] " + description;
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
        bool validAttributeValue(XValue &value);
        void validXMLDeclaration(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseAddElementContent(XNodeElement *xNodeElement, const std::string &content);
        void parseEntityMappingContents(XNodeElement *xNodeElement, XValue &entityReference);
        void parseDefault(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseTagName(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseChildElement(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseAttributes(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseComment(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseCDATA(ISource &xmlSource, XNodeElement *xNodeElement);
        void parsePI(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseElementContents(ISource &xmlSource, XNodeElement *XNodeElement);
        void parseElement(ISource &xmlSource, XNodeElement *XNodeElement);
        void parseProlog(ISource &xmlSource, XNodeElement *xNodeProlog);
        void parseXML();
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
