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
        struct SyntaxError : public std::exception
        {
        public:
            SyntaxError(const std::string &errorMessage = "") : errorMessage(errorMessage) {}
            virtual const char *what() const throw()
            {
                return ("XML syntax error detected.");
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
            virtual bool match(const XString &targetString) = 0;
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
        std::unique_ptr<XNode> parse(const std::string &xmlToParse);
        std::unique_ptr<XNode> parse(const std::u16string &xmlToParse);
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
        long calculatecharacterReference(XString reference);
        std::string convertCRLFToLF(const std::string &xmlToParse);
        inline bool validChar(XChar ch);
        bool validNameStartChar(XChar c);
        bool validNameChar(XChar c);
        bool namePresentInAttributeList(std::vector<XAttribute> attributes, const XString &name);
        bool validateName(XString attributeName);
        bool validateXMLDeclaration(XNodeElement *xNodeElement);
        void addNameValuePairToList(std::vector<XAttribute> &attributes, const XString &attributeName, const XString &attributeValue);
        XString parseDTDValue(ISource &source);
        XString parseDTDName(ISource &source);
        XString parseEncodedCharacter(ISource &source);
        XString parseAttributeValue(ISource &source);
        XString parseAttributeName(ISource &source);
        XString parseReferenceOrEntity(ISource &source);
        void parseDefault(ISource &source, XNodeElement *xNodeElement);
        void parseTagName(ISource &source, XNodeElement *xNodeElement);
        void parseChildElement(ISource &source, XNodeElement *xNodeElement);
        void parseAttributes(ISource &source, XNodeElement *xNodeElement);
        void parseComment(ISource &source, XNodeElement *xNodeElement);
        void parseCDATA(ISource &source, XNodeElement *xNodeElement);
        void parseDTDEntity(ISource &source, XNodeDTD *xNodeDTD);
        void parseDTDElement(ISource &source, XNodeDTD *xNodeElement);
        void parseDTDExternal(ISource &source, XNodeDTD *xNodeDTD);
        void parseDTDInternal(ISource &source, XNodeDTD * xNodeDTD);
        void parseDTD(ISource &source, XNodeElement *XNodeElement);
        void parsePI(ISource &source, XNodeElement *xNodeElement);
        void parseElementContents(ISource &source, XNodeElement *XNodeElement);
        void parseElement(ISource &source, XNodeElement *XNodeElement);
        void parseProlog(ISource &source, XNodeElement *xNodeRoot);
        std::unique_ptr<XNode> parseXML(ISource &source);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::wstring_convert<std::codecvt_utf8_utf16<XString::value_type>, XString::value_type> m_toFromUTF8;
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_toFromUTF16;
        std::unordered_map<XString, XString> m_entityToCharacter;
    };
} // namespace H4
#endif /* XML_HPP */
