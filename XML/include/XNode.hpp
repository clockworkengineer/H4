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
    // XML External reference
    //
    struct XExternalReference
    {
    public:
        std::string type;
        std::string systemID;
        std::string publicID;
    };
    //
    // XML Entity mapping
    //
    struct XEntityMapping
    {
    public:
        std::string internal;
        XExternalReference external;
        std::string notation;
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
        std::string getContents();
        XNode &operator[](int index);
        XNode &operator[](const std::string &name);
        std::vector<std::unique_ptr<XNode>> children;

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
        bool isAttributePresent(const std::string &name)
        {
            return (std::find_if(attributes.rbegin(), attributes.rend(),
                                 [&name](const XAttribute &attr)
                                 { return (attr.name == name); }) != attributes.rend());
        }
        bool isNameSpacePresent(const std::string &name)
        {
            return (std::find_if(namespaces.rbegin(), namespaces.rend(),
                                 [&name](const XAttribute &attr)
                                 { return (attr.name == name); }) != namespaces.rend());
        }
        XNodeElement &operator[](int index);
        XNodeElement &operator[](const std::string &name);
        std::string name;
        std::vector<XAttribute> attributes;
        std::vector<XAttribute> namespaces;
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
        XExternalReference external;
        std::unordered_map<std::string, XExternalReference> notations;
        std::unordered_map<std::string, XDTDElement> elements;
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
        if ((index >= 0) && (index < ((int)XNodeRef<XNode>(*this).children.size())))
        {
            return (*((XNodeRef<XNode>(*this).children[index].get())));
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
            for (auto &element : XNodeRef<XNodeElement>(*this).children)
            {
                if (XNodeRef<XNodeElement>(*element).name == name)
                {
                    return (*element);
                }
            }
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XNodeElement index access
    //
    inline XNodeElement &XNodeElement::operator[](int index) // Array
    {
        int number = 0;
        if ((index >= 0) && (index < ((int)XNodeRef<XNodeElement>(*this).children.size())))
        {
            for (auto &child : XNodeRef<XNode>(*this).children)
            {
                if (XNodeRef<XNode>(*child).getNodeType() <= XNodeType::element)
                {
                    if (number == index)
                    {
                        return (XNodeRef<XNodeElement>(*child));
                    }
                    number++;
                }
            }
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XNodeElement name access
    //
    inline XNodeElement &XNodeElement::operator[](const std::string &name) // Array
    {
        if (getNodeType() <= XNodeType::element)
        {
            for (auto &element : XNodeRef<XNodeElement>(*this).children)
            {
                if (XNodeRef<XNodeElement>(*element).name == name)
                {
                    return (XNodeRef<XNodeElement>(*element));
                }
            }
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XNode get contents
    //
    inline std::string XNode::getContents()
    {
        std::string result;
        for (auto &node : children)
        {
            if (node->getNodeType() == XNodeType::content)
            {
                result += XNodeRef<XNodeContent>(*node).content;
            }
            else if (node->getNodeType() == XNodeType::entity)
            {
                if (!XNodeRef<XNodeEntityReference>(*node).children.empty())
                {
                    result += XNodeRef<XNodeEntityReference>(*node).getContents();
                }
                else
                {
                    result += XNodeRef<XNodeEntityReference>(*node).value.parsed;
                }
            }
            else if (node->getNodeType() == XNodeType::cdata)
            {
                result += XNodeRef<XNodeCDATA>(*node).cdata;
            }
        }
        return (result);
    }
} // namespace H4
#endif /* XNODE_HPP */
