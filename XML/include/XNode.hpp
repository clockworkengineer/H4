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
    // Encoded string
    //
    using XMLString = std::string;
    //
    // X Attribute
    //
    struct XAttribute
    {
    public:
        XAttribute(const std::string &name, const std::string value) : name(name), value(value)
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
        const XNodeType nodeType;
    };
    //
    // Root XNode
    //
    struct XNodeElement;
    struct XNodeRoot : XNode
    {
    public:
        XNodeRoot(XNodeType nodeType = XNodeType::root) : XNode(nodeType) {}
        XMLString version;
        XMLString encoding;
        XMLString standalone;
        std::vector<XNodeElement> elements;
    };
    //
    // Element XNode
    //
    struct XNodeElement : XNode
    {
    public:
        XNodeElement(XNodeType nodeType = XNodeType::element) : XNode(nodeType) {}
        XMLString name;
        XMLString contents;
        std::vector<XAttribute> attributes;
        std::vector<XNodeElement> elements;
    };
} // namespace H4
#endif /* XNODE_HPP */
