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
        // JNode structure.
        //
        enum JNodeType
        {
            base = 0,
            dictionary = 1,
            list = 2,
            integer = 3,
            string = 4
        };
        //
        // Base JNode/
        //
        struct JNode
        {
            JNode(JNodeType nodeType = JNodeType::base)
            {
                this->nodeType = nodeType;
            }
            JNodeType nodeType;
        };
        //
        // Dictionary JNode.
        //
        struct JNodeDict : JNode
        {
            JNodeDict() : JNode(JNodeType::dictionary) {}
            std::map<std::string, std::unique_ptr<JNode>> value;
        };
        //
        // List JNode.
        //
        struct JNodeList : JNode
        {
            JNodeList() : JNode(JNodeType::list) {}
            std::list<std::unique_ptr<JNode>> value;
        };
        //
        // Integer JNode.
        //
        struct JNodeInteger : JNode
        {
            long value;
            JNodeInteger(long value) : JNode(JNodeType::integer)
            {
                this->value = value;
            }
        };
        //
        // String JNode.
        //
        struct JNodeString : JNode
        {
        public:
            std::string value;
            JNodeString(std::string value) : JNode(JNodeType::string)
            {
                this->value = value;
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

        // =================
        // PRIVATE VARIABLES
        // =================
    };
    //
    // Shortcuts for JNode structure and Bencoding type
    //
    using JNode = JSON::JNode;
    using JNodeInteger = JSON::JNodeInteger;
    using JNodeString = JSON::JNodeString;
    using JNodeList = JSON::JNodeList;
    using JNodeDict = JSON::JNodeDict;
} // namespace H4
#endif /* JSON_HPP */
