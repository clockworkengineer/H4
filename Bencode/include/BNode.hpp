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
        bool containsKey(const std::string &key)
        {
            return (m_value.count(key) > 0);
        }
        int size()
        {
            return ((int)m_value.size());
        }
        void addEntry(const std::string key, std::unique_ptr<BNode> entry)
        {
            m_value[key] = std::move(entry);
        }
        BNode *getEntry(const std::string &key)
        {
            return (m_value[key].get());
        }
        std::map<std::string, std::unique_ptr<BNode>> &getDict()
        {
            return (m_value);
        }

    protected:
        std::map<std::string, std::unique_ptr<BNode>> m_value;
    };
    //
    // List BNode.
    //
    struct BNodeList : BNode
    {
        BNodeList() : BNode(BNodeType::list) {}
        int size()
        {
            return ((int)m_value.size());
        }
        void addEntry(std::unique_ptr<BNode> jNode)
        {
            m_value.push_back(std::move(jNode));
        }
        std::vector<std::unique_ptr<BNode>> &getArray()
        {
            return (m_value);
        }
        BNode *getEntry(int index)
        {
            return (m_value[index].get());
        }

    protected:
        std::vector<std::unique_ptr<BNode>> m_value;
    };
    //
    // Integer BNode.
    //
    struct BNodeInteger : BNode
    {
        BNodeInteger(long value) : BNode(BNodeType::integer)
        {
            this->m_value = value;
        }
        long getInteger()
        {
            return (m_value);
        }

    protected:
        long m_value;
    };
    //
    // String BNode.
    //
    struct BNodeString : BNode
    {
    public:
        BNodeString(std::string value) : BNode(BNodeType::string)
        {
            this->m_value = value;
        }
        std::string getString()
        {
            return (m_value);
        }

    protected:
        std::string m_value;
    };
    //
    // Convert base reference
    //
    template <typename T>
    T &BNodeRef(BNode &bNode)
    {
        return (static_cast<T &>(bNode));
    }
    //
    // Index overloads
    //
    inline BNode &BNode::operator[](std::string key) //Dicionary
    {
        if (nodeType == BNodeType::dictionary)
        {
            if (BNodeRef<BNodeDict>(*this).containsKey(key))
            {
                return (*((BNodeRef<BNodeDict>(*this).getEntry(key))));
            }
        }
        throw std::runtime_error("Invalid key used in dictionary.");
    }
    inline BNode &BNode::operator[](int index) // List
    {
        if (nodeType == BNodeType::list)
        {
            if ((index >= 0) && (index < ((int)BNodeRef<BNodeList>(*this).size())))
            {
                return (*((BNodeRef<BNodeList>(*this).getEntry(index))));
            }
        }
        throw std::runtime_error("Invalid index used in list.");
    }

} // namespace H4
#endif /* BNODE_HPP */