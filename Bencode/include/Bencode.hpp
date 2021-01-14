#ifndef BENCODE_HPP
#define BENCODE_HPP
//
// C++ STL
//
#include <string>
#include <map>
#include <vector>
#include <list>
#include <memory>
#include <fstream>
// =========
// NAMESPACE
// =========
namespace H4
{
    // ================
    // CLASS DEFINITION
    // ================
    class Bencode
    {
    public:
        // ==========================
        // PUBLIC TYPES AND CONSTANTS
        // ==========================
        //
        // Source/destination for Bencoded data.
        //
        struct Bencoding
        {
            Bencoding() {}
            Bencoding(const char *bEncodedData)
            {
                while (*bEncodedData != '\0')
                {
                    m_Buffer.push_back((std::byte)*bEncodedData);
                    bEncodedData++;
                }
            }
            Bencoding(const std::string &bEncodedData)
            {
                for (auto byte : bEncodedData)
                {
                    m_Buffer.push_back((std::byte)byte);
                }
            }
            bool operator==(const Bencoding &rhs) const
            {
                return (std::equal(m_Buffer.begin(), m_Buffer.end(), rhs.m_Buffer.begin()));
            }
            std::byte &operator[](int index)
            {
                return m_Buffer[index];
            }
            bool isEmpty() const
            {
                return (m_Buffer.empty());
            }
            std::size_t size() const
            {
                return (m_Buffer.size());
            }
            void add(std::byte byte)
            {
                m_Buffer.push_back(byte);
            }

        private:
            std::vector<std::byte> m_Buffer;
        };
        //
        // BNode structure.
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
        struct BNodeInteger;
        struct BNodeString;
        struct BNodeList;
        struct BNodeDict;
        struct BNode
        {
            //
            // Convert BNode refence to correct type
            //
            static BNodeInteger &refBNodeInteger(BNode &bNode)
            {
                if (bNode.nodeType == Bencode::BNodeType::integer)
                {
                    return (static_cast<BNodeInteger &>(bNode));
                }
                throw std::runtime_error("Failure trying to access non BNodeInteger reference.");
            }
            static BNodeString &refBNodeString(BNode &bNode)
            {
                if (bNode.nodeType == Bencode::BNodeType::string)
                {
                    return (static_cast<BNodeString &>(bNode));
                }
                throw std::runtime_error("Failure trying to access non BNodeString reference.");
            }
            static BNodeList &refBNodeList(BNode &bNode)
            {
                if (bNode.nodeType == Bencode::BNodeType::list)
                {
                    return (static_cast<BNodeList &>(bNode));
                }
                throw std::runtime_error("Failure trying to access non BNodeList reference.");
            }
            static BNodeDict &refBNodeDict(BNode &bNode)
            {
                if (bNode.nodeType == Bencode::BNodeType::dictionary)
                {
                    return (static_cast<BNodeDict &>(bNode));
                }
                throw std::runtime_error("Failure trying to access non BNodeDict reference.");
            }
            BNode(BNodeType nodeType = BNodeType::base)
            {
                this->nodeType = nodeType;
            }
            //
            // Index for correct BNode type
            //
            BNode &operator[](std::string key) //Dicionary
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
            BNode &operator[](int index) // List
            {
                if (nodeType == BNodeType::list)
                {
                    if ((index >= 0) && (index <  ((int)static_cast<BNodeList *>(this)->value.size())))
                    {
                        return (*static_cast<BNode *>((static_cast<BNodeList *>(this)->value[index].get())));
                    }
                }
                throw std::runtime_error("Invalid index used in list.");
            }
            BNodeType nodeType;
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
        // Source interface
        //
        class ISource
        {
        public:
            virtual std::byte currentByte() = 0;
            virtual void moveToNextByte() = 0;
            virtual bool bytesToDecode() = 0;
        };
        //
        // Destination interface
        //
        class IDestination
        {
        public:
            virtual void addBytes(std::string bytes) = 0;
        };
        // ============
        // CONSTRUCTORS
        // ============
        // ==========
        // DESTRUCTOR
        // ==========
        // ==============
        // PUBLIC METHODS
        // ==============
        std::unique_ptr<BNode> decodeBuffer(const Bencoding &source);
        std::unique_ptr<BNode> decodeFile(std::string fileName);
        Bencoding encodeBuffer(std::unique_ptr<BNode> bNodeRoot);
        void encodeFile(std::unique_ptr<BNode> bNodeRoot, std::string destinationFileName);
        // ================
        // PUBLIC VARIABLES
        // ================
    private:
        // ===========================
        // PRIVATE TYPES AND CONSTANTS
        // ===========================
        // ===========================================
        // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
        // ===========================================
        // ===============
        // PRIVATE METHODS
        // ===============
        long decodePositiveInteger(ISource *source);
        std::string decodeString(ISource *source);
        std::unique_ptr<BNode> decodeBNodes(ISource *source);
        void encodeBNodes(BNode *bNode, IDestination *destination);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::string m_workBuffer;
    };
    //
    // Shortcuts for BNode structure and Bencoding type
    //
    using BNode = Bencode::BNode;
    using BNodeInteger = Bencode::BNodeInteger;
    using BNodeString = Bencode::BNodeString;
    using BNodeList = Bencode::BNodeList;
    using BNodeDict = Bencode::BNodeDict;
    using Bencoding = Bencode::Bencoding;
} // namespace H4
#endif /* BENCODE_HPP */
