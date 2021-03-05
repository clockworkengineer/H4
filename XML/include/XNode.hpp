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
    // DTD element definition
    //
    struct XDTDAttribute
    {
        std::string name;
        std::string type;
        std::string value;
    };
    struct XDTDElement
    {
        XDTDElement() {}
        XDTDElement(const std::string &name, const std::string &content) : name(name), content(content) {}
        std::string name;
        std::string content;
        std::vector<XDTDAttribute> attributes;
    };
    //
    // X Attribute
    //
    struct XValue
    {
    public:
        XValue() {}
        XValue(const std::string &unparsed, const std::string &parsed) : unparsed(unparsed), parsed(parsed)
        {
        }
        std::string unparsed;
        std::string parsed;
    };
    //
    // X Attribute
    //
    struct XAttribute
    {
    public:
        XAttribute() {}
        XAttribute(const std::string &name, const std::string &value, const std::string &unparsed = "") : name(name), value(value), unparsed(unparsed)
        {
        }
        std::string name;
        std::string value;
        std::string unparsed;
    };
    //
    // XNode structure.
    //
    enum XNodeType
    {
        base = 0,
        prolog = 1,
        root = 2,
        self = 3,
        element = 4,
        content = 5,
        entity = 6,
        comment = 7,
        cdata = 8,
        pi = 9,
        dtd = 10
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
        void setNodeType(XNodeType nodeType)
        {
            this->nodeType = nodeType;
        }
        XNode &operator[](int index);

    private:
        XNodeType nodeType;
    };
    //
    // Content XNode
    //
    struct XNodeContent : XNode
    {
    public:
        XNodeContent(XNodeType nodeType = XNodeType::content) : XNode(nodeType)
        {
        }
        std::string content;
    };
    //
    // EntityReference XNode
    //
    struct XNodeEntityReference : XNode
    {
    public:
        XNodeEntityReference(XNodeType nodeType = XNodeType::entity) : XNode(nodeType)
        {
        }
        std::string unparsed;
        std::string parsed;
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
        std::string content;
        std::vector<XAttribute> attributes;
        std::vector<XAttribute> namespaces;
        std::vector<std::unique_ptr<XNode>> elements;
        std::string getContents()
        {
            std::string result;
            for (auto &node : elements)
            {
                if (node.get()->getNodeType() == XNodeType::content)
                {
                    result += static_cast<XNodeContent *>(node.get())->content;
                }
                else if (node.get()->getNodeType() == XNodeType::entity)
                {
                    result += static_cast<XNodeEntityReference *>(node.get())->parsed;
                }
                else if (node.get()->getNodeType() == XNodeType::cdata)
                {
                    result += static_cast<XNodeCDATA *>(node.get())->cdata;
                }
            }
            return (result);
        }
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
    // PI XNode
    //
    struct XNodePI : XNode
    {
    public:
        XNodePI(XNodeType nodeType = XNodeType::pi) : XNode(nodeType)
        {
        }
        std::string name;
        std::string parameters;
    };
    //
    // DTD XNode
    //
    struct XNodeDTD : XNode
    {
    public:
        XNodeDTD(XNodeType nodeType = XNodeType::dtd) : XNode(nodeType)
        {
        }
        std::string name;
        XAttribute external;
        std::unordered_map<std::string, XAttribute> notations;
        std::unordered_map<std::string, XDTDElement> elements;
        std::unordered_map<std::string, std::string> entityMapping;
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
        if (nodeType <= XNodeType::element)
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
