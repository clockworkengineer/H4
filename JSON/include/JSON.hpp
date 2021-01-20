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
#include <sstream>
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
        // Source interface
        //
        class ISource
        {
        public:
            virtual char currentByte() = 0;
            virtual void moveToNextByte() = 0;
            virtual bool bytesToDecode() = 0;
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
        std::unique_ptr<JNode> decodeBuffer(const std::string &jsonBuffer);
        std::unique_ptr<JNode> decodeFile(const std::string &sourceFileName);
        std::string encodeBuffer(std::unique_ptr<JNode> jNodeRoot);
        void encodeFile(std::unique_ptr<JNode> jNodeRoot, const std::string &destinationFileName);
        std::string stripWhiteSpaceBuffer(const std::string &jsonBuffer);
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
        std::string addEscapedToString(std::string const &utf8String);
        void ignoreWhiteSpace(ISource *source);
        std::string extractString(ISource *source);
        std::unique_ptr<JNode> decodeJNodes(ISource *source);
        std::unique_ptr<JNode> decodeString(ISource *source);
        std::unique_ptr<JNode> decodeNumber(ISource *source);
        std::unique_ptr<JNode> decodeBoolean(ISource *source);
        std::unique_ptr<JNode> decodeNull(ISource *source);
        std::unique_ptr<JNode> decodeObject(ISource *source);
        std::unique_ptr<JNode> decodeArray(ISource *source);
        void encodeJNodes(JNode *jNode, IDestination *destination);
        void stripWhiteSpace(ISource *source, IDestination *destination);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::string m_workBuffer;
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> m_utf8ToUnicode;
        std::ostringstream m_escapedString;
    };
} // namespace H4
#endif /* JSON_HPP */
