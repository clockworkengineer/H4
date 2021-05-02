#ifndef XML_HPP
#define XML_HPP
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <memory>
#include <codecvt>
#include <locale>
#include <unordered_map>
#include <set>
#include <tuple>
//
// XML XNodes
//
#include "XNode.hpp"
//
// XML character constants
//
#define kCarriageReturn 0x0D
#define kLineFeed 0x0A
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
        class ISource;
        struct SyntaxError : public std::exception
        {
        public:
            SyntaxError(const std::string &description = "")
            {
                errorMessage = "XML Syntax Error: " + description;
            }
            SyntaxError(ISource &source, const std::string &description = "")
            {
                errorMessage = "XML Syntax Error [Line: " + std::to_string(source.getLineNo()) +
                               " Column: " + std::to_string(source.getColumnNo()) + "] " + description;
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
        //
        // Source Interface
        //
        class ISource
        {
        public:
            virtual XChar current() = 0;
            virtual void next() = 0;
            virtual bool more() = 0;
            virtual void backup(long length) = 0;
            virtual long position() = 0;
            virtual std::string getRange(long start, long end) = 0;
            void ignoreWS()
            {
                while (more() && std::iswspace(current()))
                {
                    next();
                }
            }
            bool match(const XString &targetString)
            {
                long index = 0;
                while (more() && current() == targetString[index])
                {
                    next();
                    if (++index == (long)targetString.length())
                    {
                        return (true);
                    }
                }
                backup(index);
                return (false);
            }
            long getLineNo()
            {
                return (m_lineNo);
            }
            long getColumnNo()
            {
                return (m_column);
            }
            std::string to_bytes(const XString &from)
            {
                return (m_UTF8.to_bytes(from));
            }
            std::string to_bytes(const XChar &from)
            {
                return (m_UTF8.to_bytes(from));
            }
            XString from_bytes(const std::string &from)
            {
                return (m_UTF8.from_bytes(from));
            }

        protected:
            long m_lineNo = 1;
            long m_column = 1;
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_UTF16;
            std::wstring_convert<std::codecvt_utf8_utf16<XString::value_type>, XString::value_type> m_UTF8;
        };
        //
        // Destination interface
        //
        class IDestination
        {
        public:
            virtual void add(const std::string &bytes) = 0;
            virtual void add(const XChar) = 0;

        protected:
            std::wstring_convert<std::codecvt_utf8_utf16<XString::value_type>, XString::value_type> m_UTF8;
        };
        //
        // XObject
        //
        struct XMLObject
        {
            XMLObject()
            {
                prolog.setNodeType(XNodeType::prolog);
            }
            XNodeElement prolog;
        };
        // ============
        // CONSTRUCTORS
        // ============
        XML()
        {
            initialiseTables();
        }
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        XMLObject parse(ISource &xmlSource);
        void stringify(XMLObject &xmlObject, IDestination &xmlDestination);
        void validate(XMLObject &xmlObject);
        // ================
        // PUBLIC VARIABLES
        // ================
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
        void dtdParseParameterENTITIES(XNodeDTD *xNodeDTD, ISource &dtdSource);
        void dtdValidateAttributes(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void dtdValidateContentSpecification(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void dtdValidateElement(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void dtdVadlidateElements(XNodeDTD *dtd, XNode *xNodeRoot);
        bool dtdVadlidateIsPCDATA(XNodeElement *xNodeElement);
        bool dtdValidateIsEMPTY(XNodeElement *xNodeElement);
        void initialiseTables();
        void xmlParseAddElementContent(XNodeElement *xNodeElement, const std::string &content);
        bool isAttributePresent(std::vector<XAttribute> attributes, const std::string &name);
        XAttribute getAttribute(std::vector<XAttribute> attributeList, const std::string &attributeName);
        bool validChar(XChar ch);
        bool validNameStartChar(XChar c);
        bool validNameChar(XChar c);
        bool validName(XString attributeName);
        void validXMLDeclaration(ISource &xmlSource, XNodeElement *xNodeElement);
        bool validReservedName(const XString &name);
        std::string dtdParseTranslateParameterEntities(XNodeDTD *xNodeDTD, const std::string &parameterEntities);
        std::string dtdParseAttributeEnumerationType(ISource &xmlSource);
        bool dtdParseIsChoiceOrSequence(XML::ISource &contentSpecSource);
        void dtdParseElementCP(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementChoice(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementSequence(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementChildren(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementName(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementMixedContent(ISource &contentSpecSource, IDestination &contentSpec);
        void dtdParseElementContentSpecification(XNodeDTD *xNodeDTD, XValue &contentSpec);
        void dtdParsePostProcessing(XNodeDTD *xNodeDTD);
        void dtdParseExternalContents(XNodeDTD *xNodeDTD);
        void dtdParseAttributeList(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void dtdParseNotation(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void dtdParseEntity(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void dtdParseElement(ISource &xmlSource, XNodeDTD *xNodeElement);
        XExternalReference dtdParseExternalReference(ISource &xmlSource);
        std::string dtdParseAttributeType(ISource &xmlSource);
        XValue dtdParseAttributeValue(ISource &xmlSource);
        void dtdParseComment(ISource &source, XNodeDTD *xNodeDTD);
        void dtdParseParameterEntity(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void dtdParseExternal(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void dtdParseInternal(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void dtdParse(ISource &xmlSource, XNodeElement *XNodeElement);
        std::string xmlParseName(ISource &xmlSource);
        std::string xmlParseCharacterReference(ISource &xmlSource, std::string reference);
        XValue xmlParseValue(ISource &xmlSource, bool translateEntity = true);
        XValue xmlParseCharacter(ISource &source);
        XValue xmlParseReferenceOrEntity(ISource &source);
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
        void dtdValidateXML(XNode *xNodeRoot);
        XMLObject xmlParse(ISource &xmlSource);
        void stringifyXML(XNode *xNodeRoot, IDestination &xmlDestination);
        // =================
        // PRIVATE VARIABLES
        // =================
        static XAttribute m_defaultAtributes[3];
        static std::vector<XString> m_dtdAttrListTypes;
        std::unordered_map<std::string, XEntityMapping> m_entityMapping;
    };
    //
    // Shortcuts
    //
    using XMLObject = XML::XMLObject;
} // namespace H4
#endif /* XML_HPP */
