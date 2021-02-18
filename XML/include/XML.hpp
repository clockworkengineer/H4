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
                if (!more())
                {
                    throw std::runtime_error("Parse buffer empty before parse complete.");
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

        protected:
            long m_lineNo = 1;
            long m_column = 1;
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
        long parseCharacterReference(ISource &source, XString reference);
        bool isAttributePresent(std::vector<XAttribute> attributes, const std::string &name);
        void addNamespacesToList(XNodeElement *XNodeElement);
        bool validChar(XChar ch);
        bool validNameStartChar(XChar c);
        bool validNameChar(XChar c);
        bool validateName(XString attributeName);
        bool validateXMLDeclaration(XNodeElement *xNodeElement);
        bool validReservedName(const XString &name);
        XString parseName(ISource &source);
        XString parseValue(ISource &source);
        XString parseEncodedCharacter(ISource &source);
        XString parseReferenceOrEntity(ISource &source);
        XString parseDTDAttributeType(ISource &source);
        XString parseDTDAttributeValue(ISource &source);
        void parseDefault(ISource &source, XNodeElement *xNodeElement);
        void parseTagName(ISource &source, XNodeElement *xNodeElement);
        void parseChildElement(ISource &source, XNodeElement *xNodeElement);
        void parseAttributes(ISource &source, XNodeElement *xNodeElement);
        void parseComment(ISource &source, XNodeElement *xNodeElement);
        void parseCDATA(ISource &source, XNodeElement *xNodeElement);
        void parseDTDAttributeList(ISource &source, XNodeDTD *xNodeDTD);
        void parseDTDNotation(ISource &source, XNodeDTD *xNodeDTD);
        void parseDTDEntity(ISource &source, XNodeDTD *xNodeDTD);
        void parseDTDElement(ISource &source, XNodeDTD *xNodeElement);
        void parseDTDExternal(ISource &source, XNodeDTD *xNodeDTD);
        void parseDTDInternal(ISource &source, XNodeDTD *xNodeDTD);
        void parseDTD(ISource &source, XNodeElement *XNodeElement);
        void parsePI(ISource &source, XNodeElement *xNodeElement);
        void parseElementContents(ISource &source, XNodeElement *XNodeElement);
        void parseElement(ISource &source, XNodeElement *XNodeElement);
        void parseProlog(ISource &source, XNodeElement *xNodeRoot);
        std::unique_ptr<XNode> parseXML(ISource &source);
        // =================
        // PRIVATE VARIABLES
        // =================
        static std::wstring_convert<std::codecvt_utf8_utf16<XString::value_type>, XString::value_type> m_UTF8;
        static XAttribute m_defaultAtributes[3];
        static std::set<XString> m_dtdAttrListTypes;
        std::unordered_map<XString, XString> m_entityMapping;
    };
} // namespace H4
#endif /* XML_HPP */
