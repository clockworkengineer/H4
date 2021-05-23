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
        XML(ISource &xmlSource) : m_xmlSource(xmlSource)
        {
        }
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        XMLEntityMapping &getEntity(const std::string &entityName);
        void parse();
        void stringify(IDestination &xmlDestination);
        void validate();
        // ================
        // PUBLIC VARIABLES
        // ================
        XMLNodeElement m_prolog;
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
        void stringifyElements(XMLNode *xNode, IDestination &xmlDestination);
        bool validAttributeValue(XMLValue &value);
        void validXMLDeclaration(ISource &xmlSource, XMLNodeElement *xNodeElement);
        void parseAddElementContent(XMLNodeElement *xNodeElement, const std::string &content);
        void parseEntityMappingContents(XMLNodeElement *xNodeElement, XMLValue &entityReference);
        void parseDefault(ISource &xmlSource, XMLNodeElement *xNodeElement);
        void parseTagName(ISource &xmlSource, XMLNodeElement *xNodeElement);
        void parseChildElement(ISource &xmlSource, XMLNodeElement *xNodeElement);
        void parseAttributes(ISource &xmlSource, XMLNodeElement *xNodeElement);
        void parseComment(ISource &xmlSource, XMLNodeElement *xNodeElement);
        void parseCDATA(ISource &xmlSource, XMLNodeElement *xNodeElement);
        void parsePI(ISource &xmlSource, XMLNodeElement *xNodeElement);
        void parseElementContents(ISource &xmlSource, XMLNodeElement *XNodeElement);
        void parseElement(ISource &xmlSource, XMLNodeElement *XNodeElement);
        void parseProlog(ISource &xmlSource, XMLNodeElement *xNodeProlog);
        void parseXML();
        void stringifyXML(IDestination &xmlDestination);
        // =================
        // PRIVATE VARIABLES
        // =================
        static XMLAttribute m_defaultAtributes[3];
        static std::vector<XMLString> m_dtdAttrListTypes;
        ISource &m_xmlSource;
    };
} // namespace H4
#endif /* XML_HPP */
