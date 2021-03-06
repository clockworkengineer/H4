#ifndef JSON_HPP
#define JSON_HPP
//
// C++ STL
//
#include <string>
#include <map>
#include <vector>
#include <list>
#include <memory>
#include <fstream>
#include <codecvt>
#include <locale>
// #include <sstream>
#include <stdexcept>
//
// JSON JNodes
//
#include "JNode.hpp"
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class JSON
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        //
        // JSON syntax error.
        //
        struct SyntaxError : public std::exception
        {
        public:
            SyntaxError(const std::string &errorMessage="") : errorMessage(errorMessage) {}
            virtual const char *what() const throw()
            {
                return ("JSON syntax error detected.");
            }
        private:
            std::string errorMessage;
        };
        //
        // JSON translator interface
        //
        class ITranslator
        {
        public:
            virtual std::string fromEscapeSequences(const std::string &jsonString) = 0;
            virtual std::string toEscapeSequences(const std::string &utf8String) = 0;
        protected:
            bool isValidSurrogateUpper(char16_t c)
            {
                return ((c >= 0xD800) && (c <= 0xDBFF));
            }
            bool isValidSurrogateLower(char16_t c)
            {
                return ((c >= 0xDC00) && (c <= 0xDFFF));
            }
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
        JSON(ITranslator *translator = nullptr);
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        std::unique_ptr<JNode> parseBuffer(const std::string &jsonBuffer);
        std::unique_ptr<JNode> parseFile(const std::string &sourceFileName);
        std::string stringifyToBuffer(std::unique_ptr<JNode> jNodeRoot);
        void stringifyToFile(std::unique_ptr<JNode> jNodeRoot, const std::string &destinationFileName);
        std::string stripWhiteSpaceBuffer(const std::string &jsonBuffer);
        std::unique_ptr<JNode> parse(ISource &source);
        void stringify(std::unique_ptr<JNode> jNodeRoot, IDestination &destination);
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
        void ignoreWhiteSpace(ISource &source);
        std::string extractString(ISource &source);
        std::unique_ptr<JNode> parseJNodes(ISource &source);
        std::unique_ptr<JNode> parseString(ISource &source);
        std::unique_ptr<JNode> parseNumber(ISource &source);
        std::unique_ptr<JNode> parseBoolean(ISource &source);
        std::unique_ptr<JNode> parseNull(ISource &source);
        std::unique_ptr<JNode> parseObject(ISource &source);
        std::unique_ptr<JNode> parseArray(ISource &source);
        void stringifyJNodes(JNode *jNode, IDestination &destination);
        void stripWhiteSpace(ISource &source, IDestination &destination);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::string m_workBuffer;
        ITranslator *m_jsonTranslator;
    };
} // namespace H4
#endif /* JSON_HPP */
