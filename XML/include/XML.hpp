#ifndef XML_HPP
#define XML_HPP
//
// C++ STL
//
#include <string>
#include <stdexcept>
#include <memory>
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
            virtual char currentByte() = 0;
            virtual void moveToNextByte() = 0;
            virtual bool bytesToParse() = 0;
            virtual bool findString(const std::string &targetString) = 0;
            void ignoreWhiteSpace()
            {
                while (bytesToParse() && std::iswspace(currentByte()))
                {
                    moveToNextByte();
                }
                if (!bytesToParse())
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
            virtual void addBytes(std::string bytes) = 0;
        };
        // ============
        // CONSTRUCTORS
        // ============
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        XNodeRoot parse(const XString &xmlToParse);
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
        bool validTagName(std::string tagName);
        bool validAtttributeName(std::string attributeName);
        std::vector<XAttribute> validateDeclaration(const std::vector<XAttribute> &attribute);
        std::string extractTagName(ISource &source);
        std::string extractAttributeValue(ISource &source);
        std::string extractAttributeName(ISource &source);
        void parseComment(ISource &source);
        std::vector<XAttribute> parseAttributes(ISource &source);
        XNodeElement parseElement(ISource &source);
        void parseRootElement(ISource &source, XNodeRoot &xNodeRoot);
        XNodeRoot parseProlog(ISource &source);
        XNodeRoot parseXML(ISource &source);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::string m_workBuffer;
    };
} // namespace H4
#endif /* XML_HPP */
