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
            SyntaxError(ISource &source, const std::string &description = "")
            {
                errorMessage = "XML Syntax Error [Line: " + std::to_string(source.getLineNo()) +
                               " Column: " + std::to_string(source.getColumnNo()) + "]" + description;
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
        void validateElement(XNodeDTD *dtd, XNodeElement *xNodeElement);
        void vadlidateElements(XNodeDTD *dtd, XNode *xNodeRoot);
        void initialiseTables();
        void addContent(XNodeElement *xNodeElement, const std::string &content);
        bool isAttributePresent(std::vector<XAttribute> attributes, const std::string &name);
        void moveToNextLineFeed(ISource &source);
        bool validChar(XChar ch);
        bool validNameStartChar(XChar c);
        bool validNameChar(XChar c);
        bool validateName(XString attributeName);
        bool validateXMLDeclaration(XNodeElement *xNodeElement);
        bool validReservedName(const XString &name);
        std::string parseName(ISource &xmlSource);
        long parseCharacterReference(ISource &xmlSource, XString reference);
        XValue parseValue(ISource &xmlSource);
        XValue parseCharacter(ISource &source);
        XValue parseReferenceOrEntity(ISource &source);
        XAttribute parseDTDExternalReference(ISource &xmlSource);
        std::string parseDTDAttributeType(ISource &xmlSource);
        XValue parseDTDAttributeValue(ISource &xmlSource);
        void parseDTDComment(ISource &source, XNodeDTD *xNodeDTD);
        void parseDefault(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseTagName(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseChildElement(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseAttributes(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseComment(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseCDATA(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseDTDAttributeList(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void parseDTDNotation(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void parseDTDEntity(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void parseDTDElement(ISource &xmlSource, XNodeDTD *xNodeElement);
        void parseDTDExternal(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void parseDTDInternal(ISource &xmlSource, XNodeDTD *xNodeDTD);
        void parseDTD(ISource &xmlSource, XNodeElement *XNodeElement);
        void parsePI(ISource &xmlSource, XNodeElement *xNodeElement);
        void parseElementContents(ISource &xmlSource, XNodeElement *XNodeElement);
        void parseElement(ISource &xmlSource, XNodeElement *XNodeElement);
        void parseProlog(ISource &xmlSource, XNodeElement *xNodeProlog);
        void validateXML(XNode *xNodeRoot);
        XMLObject parseXML(ISource &xmlSource);
        void stringifyXML(XNode *xNodeRoot, IDestination &xmlDestination);
        // =================
        // PRIVATE VARIABLES
        // =================
        static XAttribute m_defaultAtributes[3];
        static std::set<XString> m_dtdAttrListTypes;
        std::unordered_map<XString, XString> m_entityMapping;
    };
    //
    // Shortcuts
    //
    using XMLObject = XML::XMLObject;
} // namespace H4
#endif /* XML_HPP */
