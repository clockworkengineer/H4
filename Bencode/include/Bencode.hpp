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
                    bEncodedBuffer.push_back((std::byte)*bEncodedData);
                    bEncodedData++;
                }
            }
            Bencoding(const std::string bEncodedData)
            {
                for (auto byte : bEncodedData)
                {
                    bEncodedBuffer.push_back((std::byte)byte);
                }
            }
            std::vector<std::byte> bEncodedBuffer;
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
        // Base BNode/
        //
        struct BNode
        {
            BNode(BNodeType nodeType = BNodeType::base)
            {
                this->nodeType = nodeType;
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
            std::list<std::unique_ptr<BNode>> value;
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
    //
    // Bencoding comparison operator
    //
    inline bool operator==(const Bencode::Bencoding &lhs, const Bencode::Bencoding &rhs)
    {
        return (std::equal(lhs.bEncodedBuffer.begin(), lhs.bEncodedBuffer.end(), rhs.bEncodedBuffer.begin()));
    }
} // namespace H4
#endif /* BENCODE_HPP */
