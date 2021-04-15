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
    // XML value
    //
    struct XValue
    {
    public:
        std::string unparsed;
        std::string parsed;
    };
    //
    // XML Attribute
    //
    struct XAttribute
    {
    public:
        XAttribute() {}
        XAttribute(const std::string &name, const XValue &value) : name(name), value(value)
        {
        }
        std::string name;
        XValue value;
    };
    //
    // XML DTD attribute definition
    //
    struct XDTDAttribute
    {
        std::string name;
        std::string type;
        XValue value;
    };
    //
    // XML DTD element definition
    //
    struct XDTDElement
    {
        XDTDElement() {}
        XDTDElement(const std::string &name, const XValue &content) : name(name), content(content) {}
        std::string name;
        XValue content;
        std::vector<XDTDAttribute> attributes;
    };
    //
    // XML XNode structure
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
        XNode &operator[](const std::string &name);
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
        bool isWhiteSpace = true;
    };
    //
    // EntityReference XNode
    //
    struct XNodeEntityReference : XNode
    {
    public:
        XNodeEntityReference(const XValue &value, XNodeType nodeType = XNodeType::entity) : XNode(nodeType), value(value)
        {
        }
        XValue value;
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
                    result += static_cast<XNodeEntityReference *>(node.get())->value.parsed;
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
        std::set<std::string> assignedIDValues;
        std::set<std::string> assignedIDREFValues;
        std::string unparsed;
        long lineNumber = 1;
    };
    //
    // Convert base XNode reference
    //
    template <typename T>
    T &XNodeRef(XNode &xNode)
    {
        return (static_cast<T &>(xNode));
    }
    //
    // XNode index access
    //
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
    //
    // XNode name access
    //
    inline XNode &XNode::operator[](const std::string &name) // Array
    {
        if (nodeType <= XNodeType::element)
        {
            for (auto &element : XNodeRef<XNodeElement>(*this).elements)
            {
                if (XNodeRef<XNodeElement>(*element).name == name)
                {
                    return (*element);
                }
            }
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
} // namespace H4
#endif /* XNODE_HPP */
