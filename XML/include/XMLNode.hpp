#ifndef XNODE_HPP
#define XNODE_HPP
//
// C++ STL
//
#include <vector>
// =========
// NAMESPACE
// =========
namespace H4
{
    //
    // XML XMLNode structure
    //
    enum XMLNodeType
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
    // Base XMLNode
    //
    struct XMLNode
    {
    public:
        XMLNode(XMLNodeType nodeType = XMLNodeType::base) : nodeType(nodeType)
        {
        }
        XMLNodeType getNodeType()
        {
            return (nodeType);
        }
        void setNodeType(XMLNodeType nodeType)
        {
            this->nodeType = nodeType;
        }
        std::string getContents();
        XMLNode &operator[](int index);
        XMLNode &operator[](const std::string &name);
        std::vector<std::unique_ptr<XMLNode>> children;
    private:
        XMLNodeType nodeType;
    };
    //
    // Content XMLNode
    //
    struct XMLNodeContent : XMLNode
    {
    public:
        XMLNodeContent(XMLNodeType nodeType = XMLNodeType::content) : XMLNode(nodeType)
        {
        }
        std::string content;
        bool isWhiteSpace = true;
    };
    //
    // CDATA XMLNode
    //
    struct XMLNodeCDATA : XMLNode
    {
    public:
        XMLNodeCDATA(XMLNodeType nodeType = XMLNodeType::cdata) : XMLNode(nodeType)
        {
        }
        std::string cdata;
    };
    //
    // EntityReference XMLNode
    //
    struct XMLNodeEntityReference : XMLNode
    {
    public:
        XMLNodeEntityReference(const XMLValue &value, XMLNodeType nodeType = XMLNodeType::entity) : XMLNode(nodeType), value(value)
        {
        }
        XMLValue value;
    };
    //
    // Element XMLNode
    //
    struct XMLNodeElement : XMLNode
    {
    public:
        XMLNodeElement(XMLNodeType nodeType = XMLNodeType::element) : XMLNode(nodeType)
        {
        }
        XMLNodeElement(const std::string &name) : XMLNodeElement()
        {
            this->name = name;
        }
        bool isAttributePresent(const std::string &name)
        {
            return (std::find_if(attributes.rbegin(), attributes.rend(),
                                 [&name](const XMLAttribute &attr)
                                 { return (attr.name == name); }) != attributes.rend());
        }
        void addAttribute(const std::string &name, const XMLValue &value)
        {
            attributes.emplace_back(name, value);
        }
        XMLAttribute getAttribute(const std::string &name)
        {
            return (*std::find_if(attributes.rbegin(), attributes.rend(),
                                  [&name](const XMLAttribute &attr)
                                  { return (attr.name == name); }));
        }
        const std::vector<XMLAttribute> &getAttributeList()
        {
            return (attributes);
        }
        void clearAttributes()
        {
            attributes.clear();
        }
        bool isNameSpacePresent(const std::string &name)
        {
            return (std::find_if(namespaces.rbegin(), namespaces.rend(),
                                 [&name](const XMLAttribute &attr)
                                 { return (attr.name == name); }) != namespaces.rend());
        }
        void addNameSpace(const std::string &name, const XMLValue &value)
        {
            namespaces.emplace_back(name, value);
        }
        XMLAttribute getNameSpace(const std::string &name)
        {
            return (*std::find_if(namespaces.rbegin(), namespaces.rend(),
                                  [&name](const XMLAttribute &ns)
                                  { return (ns.name == name); }));
        }
        const std::vector<XMLAttribute> &getNameSpaceList()
        {
            return (namespaces);
        }
        void clearNameSpaces()
        {
            namespaces.clear();
        }
        XMLNodeElement &operator[](int index);
        XMLNodeElement &operator[](const std::string &name);
        std::string name;
    private:
        std::vector<XMLAttribute> namespaces;
        std::vector<XMLAttribute> attributes;
    };
    //
    // Comment XMLNode
    //
    struct XMLNodeComment : XMLNode
    {
    public:
        XMLNodeComment(XMLNodeType nodeType = XMLNodeType::comment) : XMLNode(nodeType)
        {
        }
        std::string comment;
    };
    //
    // PI XMLNode
    //
    struct XNodePI : XMLNode
    {
    public:
        XNodePI(XMLNodeType nodeType = XMLNodeType::pi) : XMLNode(nodeType)
        {
        }
        std::string name;
        std::string parameters;
    };
    //
    // DTD XMLNode
    //
    struct XMLNodeDTD : XMLNode
    {
    public:
        XMLNodeDTD(XMLNodeType nodeType = XMLNodeType::dtd) : XMLNode(nodeType)
        {
        }
    };
    //
    // Convert base XMLNode reference
    //
    template <typename T>
    T &XMLNodeRef(XMLNode &xmlNode)
    {
        return (static_cast<T &>(xmlNode));
    }
    //
    // XMLNode index access
    //
    inline XMLNode &XMLNode::operator[](int index) // Array
    {
        if ((index >= 0) && (index < ((int)XMLNodeRef<XMLNode>(*this).children.size())))
        {
            return (*((XMLNodeRef<XMLNode>(*this).children[index].get())));
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XMLNode name access
    //
    inline XMLNode &XMLNode::operator[](const std::string &name) // Array
    {
        if (nodeType <= XMLNodeType::element)
        {
            for (auto &element : XMLNodeRef<XMLNodeElement>(*this).children)
            {
                if (XMLNodeRef<XMLNodeElement>(*element).name == name)
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
    inline XMLNodeElement &XMLNodeElement::operator[](int index) // Array
    {
        int number = 0;
        if ((index >= 0) && (index < ((int)XMLNodeRef<XMLNodeElement>(*this).children.size())))
        {
            for (auto &child : XMLNodeRef<XMLNode>(*this).children)
            {
                if (XMLNodeRef<XMLNode>(*child).getNodeType() <= XMLNodeType::element)
                {
                    if (number == index)
                    {
                        return (XMLNodeRef<XMLNodeElement>(*child));
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
    inline XMLNodeElement &XMLNodeElement::operator[](const std::string &name) // Array
    {
        if (getNodeType() <= XMLNodeType::element)
        {
            for (auto &element : XMLNodeRef<XMLNodeElement>(*this).children)
            {
                if (XMLNodeRef<XMLNodeElement>(*element).name == name)
                {
                    return (XMLNodeRef<XMLNodeElement>(*element));
                }
            }
        }
        throw std::runtime_error("Invalid index used to access array.");
    }
    //
    // XMLNode get contents
    //
    inline std::string XMLNode::getContents()
    {
        std::string result;
        for (auto &node : children)
        {
            if (node->getNodeType() == XMLNodeType::content)
            {
                result += XMLNodeRef<XMLNodeContent>(*node).content;
            }
            else if (node->getNodeType() == XMLNodeType::entity)
            {
                if (!XMLNodeRef<XMLNodeEntityReference>(*node).children.empty())
                {
                    result += XMLNodeRef<XMLNodeEntityReference>(*node).getContents();
                }
                else
                {
                    result += XMLNodeRef<XMLNodeEntityReference>(*node).value.parsed;
                }
            }
            else if (node->getNodeType() == XMLNodeType::cdata)
            {
                result += XMLNodeRef<XMLNodeCDATA>(*node).cdata;
            }
        }
        return (result);
    }
} // namespace H4
#endif /* XNODE_HPP */
