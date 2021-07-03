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
#include <vector>
//
// Source/Destination interfaces
//
#include "ISource.hpp"
#include "IDestination.hpp"
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
        void stringifyElements(XMLNode *xmlNode, IDestination &xmlDestination);
        void validDeclaration(ISource &xmlSource, XMLNodeElement *xmlNodeElement);
        void parseAddElementContent(XMLNodeElement *xmlNodeElement, const std::string &content);
        void parseEntityMappingContents(XMLNodeElement *xmlNodeElement, XMLValue &entityReference);
        void parseDefault(ISource &xmlSource, XMLNodeElement *xmlNodeElement);
        void parseTagName(ISource &xmlSource, XMLNodeElement *xmlNodeElement);
        void parseChildElement(ISource &xmlSource, XMLNodeElement *xmlNodeElement);
        void parseAttributes(ISource &xmlSource, XMLNodeElement *xmlNodeElement);
        void parseComment(ISource &xmlSource, XMLNodeElement *xmlNodeElement);
        void parseCDATA(ISource &xmlSource, XMLNodeElement *xmlNodeElement);
        void parsePI(ISource &xmlSource, XMLNodeElement *xmlNodeElement);
        void parseElementContents(ISource &xmlSource, XMLNodeElement *XNodeElement);
        void parseElement(ISource &xmlSource, XMLNodeElement *XNodeElement);
        void parseProlog(ISource &xmlSource, XMLNodeElement *xmlNodeProlog);
        void parseXML();
        void stringifyXML(IDestination &xmlDestination);
        // =================
        // PRIVATE VARIABLES
        // =================
        static XMLAttribute m_defaultAtributes[3];
        ISource &m_xmlSource;
    };
} // namespace H4
#endif /* XML_HPP */
