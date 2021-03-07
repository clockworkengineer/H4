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
        // Source interface
        //
        class ISource
        {
        public:
            virtual XChar current() = 0;
            virtual void next() = 0;
            virtual bool more() = 0;
            virtual void backup(long length) = 0;
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
            std::string to_bytes(const XChar from)
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
        std::unique_ptr<XNode> parse(ISource &xmlSource);
        std::string stringify(std::unique_ptr<XNode> &xNodeRoot);
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
        void initialiseTables();
        long parseCharacterReference(ISource &xmlSource, XString reference);
        bool isAttributePresent(std::vector<XAttribute> attributes, const std::string &name);
        void addNamespacesToList(XNodeElement *XNodeElement);
        void moveToNextLineFeed(ISource &source);
        bool validChar(XChar ch);
        bool validNameStartChar(XChar c);
        bool validNameChar(XChar c);
        bool validateName(XString attributeName);
        bool validateXMLDeclaration(XNodeElement *xNodeElement);
        bool validReservedName(const XString &name);
        void addCurrentXNodeContent(XNodeElement *xNodeElement);
        std::string parseName(ISource &xmlSource);
        XValue parseValue(ISource &xmlSource);
        XValue parseCharacter(ISource &source);
        XValue parseReferenceOrEntity(ISource &source);
        std::string parseDTDValue(ISource &source);
        std::string parseDTDAttributeType(ISource &xmlSource);
        std::string parseDTDAttributeValue(ISource &xmlSource);
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
        std::unique_ptr<XNode> parseXML(ISource &xmlSource);
        std::string stringifyXML(XNode *xNodeRoot);
        // =================
        // PRIVATE VARIABLES
        // =================
        static XAttribute m_defaultAtributes[3];
        static std::set<XString> m_dtdAttrListTypes;
        std::unordered_map<XString, XString> m_entityMapping;
    };
} // namespace H4
#endif /* XML_HPP */
