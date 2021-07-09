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
#include "XMLEntityMapper.hpp"
//
//
//
#include "XMLValidator.hpp"
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
            m_dtd = std::make_unique<DTD>(m_entityMapper);
            m_validator = std::make_unique<XMLValidator>(*m_dtd);
        }
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        DTD &getDTD() { return (*m_dtd); }
        void parse();
        void stringify(IDestination &xmlDestination);
        void validate();
        // ================
        // PUBLIC VARIABLES
        // ================
        XMLNodeElement m_prolog;

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
        void parseDeclaration(ISource &xmlSource, XMLNodeElement *xmlNodeProlog);
        void parseProlog(ISource &xmlSource, XMLNodeElement *xmlNodeProlog);
        void parseXML();
        void stringifyElements(XMLNode *xmlNode, IDestination &xmlDestination);
        void stringifyXML(IDestination &xmlDestination);
        // =================
        // PRIVATE VARIABLES
        // =================
        ISource &m_xmlSource;
        std::unique_ptr<DTD> m_dtd;
        std::unique_ptr<XMLValidator> m_validator;
        XMLEntityMapper m_entityMapper;
    };
} // namespace H4
#endif /* XML_HPP */
