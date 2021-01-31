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
    using XString = std::u16string;
    using XChar = XString::value_type;
    //
    // X Attribute
    //
    struct XAttribute
    {
    public:
        XAttribute(const XString &name, const XString value) : name(name), value(value)
        {
        }
        XString name;
        XString value;
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
        XString version;
        XString encoding;
        XString standalone;
        XString name;
        XString contents;
        std::vector<XAttribute> attributes;
        std::vector<XNodeElement> elements;
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
        XNodeElement(const XString &name) : XNodeElement()
        {
            this->name = name;
        }
        XString name;
        XString contents;
        std::vector<XAttribute> attributes;
        std::vector<XNodeElement> elements;
    };
} // namespace H4
#endif /* XNODE_HPP */
