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
            object = 1,
            array = 2,
            number = 3,
            string = 4,
            boolean = 5,
            null = 6
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
        struct JNodeObject : JNode
        {
            JNodeObject() : JNode(JNodeType::object) {}
            std::map<std::string, std::unique_ptr<JNode>> value;
        };
        //
        // List JNode.
        //
        struct JNodeArray : JNode
        {
            JNodeArray() : JNode(JNodeType::array) {}
            std::vector<std::unique_ptr<JNode>> value;
        };
        //
        // Integer JNode.
        //
        struct JNodeNumber : JNode
        {
            std::string value;
            JNodeNumber(std::string value) : JNode(JNodeType::number)
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
        // Boolean JNode.
        //
        struct JNodeBoolean : JNode
        {
        public:
            bool value;
            JNodeBoolean(bool value) : JNode(JNodeType::boolean)
            {
                this->value = value;
            }
        };
        //
        // Boolean JNode.
        //
        struct JNodeNull : JNode
        {
        public:
            const void *value = nullptr;
            JNodeNull() : JNode(JNodeType::null)
            {
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
        std::unique_ptr<JNode> decodeBuffer(std::string jsonBuffer);
        std::unique_ptr<JNode> decodeFile(std::string sourceFileName);
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
        void ignoreWhiteSpace(ISource *source);
        std::string extractString(ISource *source);
        std::unique_ptr<JNode> decodeJNodes(ISource *source);
        std::unique_ptr<JNode> decodeString(ISource *source);
        std::unique_ptr<JNode> decodeNumber(ISource *source);
        std::unique_ptr<JNode> decodeBoolean(ISource *source);
        std::unique_ptr<JNode> decodeNull(ISource *source);
        std::unique_ptr<JNode> decodeObject(ISource *source);
        std::unique_ptr<JNode> decodeArray(ISource *source);
        // =================
        // PRIVATE VARIABLES
        // =================
    };
    //
    // Shortcuts for JNode structure and Bencoding type
    //
    using JNode = JSON::JNode;
    using JNodeNumber = JSON::JNodeNumber;
    using JNodeString = JSON::JNodeString;
    using JNodeArray = JSON::JNodeArray;
    using JNodeObject = JSON::JNodeObject;
    using JNodeNull = JSON::JNodeNull;
    using JNodeBoolean = JSON::JNodeBoolean;
} // namespace H4
#endif /* JSON_HPP */
