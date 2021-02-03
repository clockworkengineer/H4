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
        element = 2
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
        std::vector<XNodeElement> elements;
    };
    //
    // Root XNode
    //
    struct XNodeElement;
    struct XNodeRoot : XNode
    {
    public:
        XNodeRoot(XNodeType nodeType = XNodeType::root) : XNode(nodeType) {}
        std::string version;
        std::string encoding;
        std::string standalone;
        XNodeElement root;
    };

} // namespace H4
#endif /* XNODE_HPP */
