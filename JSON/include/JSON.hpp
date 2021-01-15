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
        struct JNodeNumber;
        struct JNodeString;
        struct JNodeBoolean;
        struct JNodeNull;
        struct JNodeArray;
        struct JNodeObject;
        struct JNode
        {
            template <typename T>
            static T &ref(JSON::JNode &jNode)
            {
                return (static_cast<T &>(jNode));
            }
            //
            // Convert JNode refence to correct type
            //
            static JNodeNumber &refJNodeNumber(JNode &jNode)
            {
                if (jNode.nodeType == JSON::JNodeType::number)
                {
                    return (static_cast<JNodeNumber &>(jNode));
                }
                throw std::runtime_error("Failure trying to access non JNodeNumber reference.");
            }
            static JNodeString &refJNodeString(JNode &jNode)
            {
                if (jNode.nodeType == JSON::JNodeType::string)
                {
                    return (static_cast<JNodeString &>(jNode));
                }
                throw std::runtime_error("Failure trying to access non JNodeString reference.");
            }
            static JNodeBoolean &refJNodeBoolean(JNode &jNode)
            {
                if (jNode.nodeType == JSON::JNodeType::boolean)
                {
                    return (static_cast<JNodeBoolean &>(jNode));
                }
                throw std::runtime_error("Failure trying to access non JNodeBoolean reference.");
            }
            static JNodeNull &refJNodeNull(JNode &jNode)
            {
                if (jNode.nodeType == JSON::JNodeType::null)
                {
                    return (static_cast<JNodeNull &>(jNode));
                }
                throw std::runtime_error("Failure trying to access non JNodeNull reference.");
            }
            static JNodeArray &refJNodeArray(JNode &jNode)
            {
                if (jNode.nodeType == JSON::JNodeType::array)
                {
                    return (static_cast<JNodeArray &>(jNode));
                }
                throw std::runtime_error("Failure trying to access non JNodeArray reference.");
            }
            static JNodeObject &refJNodeObject(JNode &jNode)
            {
                if (jNode.nodeType == JSON::JNodeType::object)
                {
                    return (static_cast<JNodeObject &>(jNode));
                }
                throw std::runtime_error("Failure trying to access non JNodeObject reference.");
            }
            JNode(JNodeType nodeType = JNodeType::base)
            {
                this->nodeType = nodeType;
            }
            //
            // Index for correct JNode type
            //
            JNode &operator[](std::string key) // Object
            {
                if (nodeType == JNodeType::object)
                {
                    if (static_cast<JNodeObject *>(this)->value.count(key) > 0)
                    {
                        return (*static_cast<JNode *>((static_cast<JNodeObject *>(this)->value[key].get())));
                    }
                }
                throw std::runtime_error("Invalid key used in object.");
            }
            JNode &operator[](int index) // Array
            {
                if (nodeType == JNodeType::array)
                {
                    if ((index >= 0) && (index < ((int)static_cast<JNodeArray *>(this)->value.size())))
                    {
                        return (*static_cast<JNode *>((static_cast<JNodeArray *>(this)->value[index].get())));
                    }
                }
                throw std::runtime_error("Invalid index used in array.");
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
            // Note: Store keys so when write away keep key order
            // that they have had in the source form be it file/network/buffer.
            std::vector<std::string> keys;
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
            JNodeNumber(const std::string &value) : JNode(JNodeType::number)
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
            JNodeString(const std::string &value) : JNode(JNodeType::string)
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
