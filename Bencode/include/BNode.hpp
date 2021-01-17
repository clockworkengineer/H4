#ifndef BNODE_HPP
#define BNODE_HPP
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
    // BNode types.
    //
    enum BNodeType
    {
        base = 0,
        dictionary = 1,
        list = 2,
        integer = 3,
        string = 4
    };
    //
    // Base BNode
    //
    struct BNode
    {
        BNode(BNodeType nodeType = BNodeType::base) : nodeType(nodeType)
        {
        }
        BNode &operator[](std::string key);
        BNode &operator[](int index);
        const BNodeType nodeType;
    };
    //
    // Dictionary BNode.
    //
    struct BNodeDict : BNode
    {
        BNodeDict() : BNode(BNodeType::dictionary) {}
        std::map<std::string, std::unique_ptr<BNode>> value;
    };
    //
    // List BNode.
    //
    struct BNodeList : BNode
    {
        BNodeList() : BNode(BNodeType::list) {}
        std::vector<std::unique_ptr<BNode>> value;
    };
    //
    // Integer BNode.
    //
    struct BNodeInteger : BNode
    {
        long value;
        BNodeInteger(long value) : BNode(BNodeType::integer)
        {
            this->value = value;
        }
    };
    //
    // String BNode.
    //
    struct BNodeString : BNode
    {
    public:
        std::string value;
        BNodeString(std::string value) : BNode(BNodeType::string)
        {
            this->value = value;
        }
    };
    //
    // Index overloads
    //
    inline BNode &BNode::operator[](std::string key) //Dicionary
    {
        if (nodeType == BNodeType::dictionary)
        {
            if (static_cast<BNodeDict *>(this)->value.count(key) > 0)
            {
                return (*static_cast<BNode *>((static_cast<BNodeDict *>(this)->value[key].get())));
            }
        }
        throw std::runtime_error("Invalid key used in dictionary.");
    }
    inline BNode &BNode::operator[](int index) // List
    {
        if (nodeType == BNodeType::list)
        {
            if ((index >= 0) && (index < ((int)static_cast<BNodeList *>(this)->value.size())))
            {
                return (*static_cast<BNode *>((static_cast<BNodeList *>(this)->value[index].get())));
            }
        }
        throw std::runtime_error("Invalid index used in list.");
    }
    template <typename T>
    T &BNodeRef(BNode &bNode)
    {
        return (static_cast<T &>(bNode));
    }
} // namespace H4
#endif /* BNODE_HPP */