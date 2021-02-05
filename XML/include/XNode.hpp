#ifndef XNODE_HPP
#define XNODE_HPP
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
    // 32 bit characters internally
    //
    using XString = std::u32string;
    using XChar = XString::value_type;
    //
    // X Attribute
    //
    struct XAttribute
    {
    public:
        XAttribute(const std::string &name, const std::string &value) : name(name), value(value)
        {
        }
        std::string name;
        std::string value;
    };
    //
    // XNode structure.
    //
    enum XNodeType
    {
        base = 0,
        root = 1,
        element = 2,
        comment = 3,
        cdata = 4,
        pi = 5
    };
    //
    // Base XNode
    //
    struct XNode
    {
    public:
        XNode(XNodeType nodeType = XNodeType::base) : nodeType(nodeType)
        {
        }
        XNodeType getNodeType()
        {
            return (nodeType);
        }
        XNode &operator[](int index);

    private:
        XNodeType nodeType;
    };
    //
    // Element XNode
    //
    struct XNodeElement : XNode
    {
    public:
        XNodeElement(XNodeType nodeType = XNodeType::element) : XNode(nodeType)
        {
        }
        XNodeElement(const std::string &name) : XNodeElement()
        {
            this->name = name;
        }
        std::string name;
        std::string contents;
        std::vector<XAttribute> attributes;
        std::vector<std::unique_ptr<XNode>> elements;
    };
    //
    // Comment XNode
    //
    struct XNodeComment : XNode
    {
    public:
        XNodeComment(XNodeType nodeType = XNodeType::comment) : XNode(nodeType)
        {
        }
        std::string comment;
    };
    //
    // CDATA XNode
    //
    struct XNodeCDATA : XNode
    {
    public:
        XNodeCDATA(XNodeType nodeType = XNodeType::cdata) : XNode(nodeType)
        {
        }
        std::string cdata;
    };
    //
    // PI XNode
    //
    struct XNodePI : XNode
    {
    public:
        XNodePI(XNodeType nodeType = XNodeType::cdata) : XNode(nodeType)
        {
        }
        std::string name;
        std::vector<XAttribute> attributes;
    };
    //
    // Convert base XNode reference
    //
    template <typename T>
    T &XNodeRef(XNode &xNode)
    {
        return (static_cast<T &>(xNode));
    }
    inline XNode &XNode::operator[](int index) // Array
    {
        if (nodeType == XNodeType::element)
        {
            if ((index >= 0) && (index < ((int)XNodeRef<XNodeElement>(*this).elements.size())))
            {
                return (*((XNodeRef<XNodeElement>(*this).elements[index].get())));
            }
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
} // namespace H4
#endif /* XNODE_HPP */
