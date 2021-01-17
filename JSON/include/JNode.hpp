#ifndef JNODE_HPP
#define JNODE_HPP
//
// C++ STL
//
#include <string>
#include <map>
#include <vector>
#include <list>
#include <memory>
// =========
// NAMESPACE
// =========
namespace H4
{
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
    public:
        JNode(JNodeType nodeType = JNodeType::base)
        {
            this->nodeType = nodeType;
        }
        //
        // Index for correct JNode type
        //
        JNode &operator[](std::string key);
        JNode &operator[](int index);
        JNodeType nodeType;
    };
    //
    // Dictionary JNode.
    //
    struct JNodeObject : JNode
    {
    public:
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
    public:
        JNodeArray() : JNode(JNodeType::array) {}
        std::vector<std::unique_ptr<JNode>> value;
    };
    //
    // Number JNode.
    //
    struct JNodeNumber : JNode
    {
    public:
        std::string value;
        JNodeNumber(const std::string &value) : JNode(JNodeType::number)
        {
            this->value = value;
        }
        // Convert to long returning true on success
        // Note: Can still return a long value for floating point
        // but false as the number is not in integer format
        bool getInteger(long &longValue)
        {
            char *end;
            longValue = std::strtoll(value.c_str(), &end, 10);
            return (*end == '\0'); // If not all characters used then not success
        }
        // Convert to double returning true on success
        bool getFloatingPoint(double &doubleValue)
        {
            char *end;
            doubleValue = std::strtod(value.c_str(), &end);
            return (*end == '\0'); // If not all characters used then not success
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
    // Index overloads
    //
    inline JNode &JNode::operator[](std::string key) // Object
    {
        if (nodeType == JNodeType::object)
        {
            if (static_cast<JNodeObject *>(this)->value.count(key) > 0)
            {
                return (*static_cast<JNode *>((static_cast<JNodeObject *>(this)->value[key].get())));
            }
        }
        throw std::runtime_error("Invalid key used to access object.");
    }
    inline JNode &JNode::operator[](int index) // Array
    {
        if (nodeType == JNodeType::array)
        {
            if ((index >= 0) && (index < ((int)static_cast<JNodeArray *>(this)->value.size())))
            {
                return (*static_cast<JNode *>((static_cast<JNodeArray *>(this)->value[index].get())));
            }
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
    template <typename T>
    T &JNodeRef(JNode &jNode)
    {
        return (static_cast<T &>(jNode));
    }
} // namespace H4
#endif /* JNODE_HPP */