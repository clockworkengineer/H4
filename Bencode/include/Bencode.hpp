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
            Bencoding(const char *data)
            {
                while (*data != '\0')
                {
                    buffer.push_back((std::byte)*data);
                    data++;
                }
            }
            Bencoding() {}
            std::vector<std::byte> buffer;
        };
        //
        // Base for BNode structure.
        //
        struct BNode
        {
            virtual ~BNode() = default;
        };
        //
        // Dictionary BNode.
        //
        struct BNodeDict : BNode
        {
            std::map<std::string, std::unique_ptr<BNode>> value;
        };
        //
        // List BNode.
        //
        struct BNodeList : BNode
        {
            std::list<std::unique_ptr<BNode>> value;
        };
        //
        // Integer BNode.
        //
        struct BNodeInteger : BNode
        {
            long value;
            BNodeInteger(long value)
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
            BNodeString(std::string value)
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
        std::unique_ptr<BNode> decodeBuffer(const Bencoding& source);
        std::unique_ptr<BNode> decodeFile(std::string fileName);
        Bencoding encodeToBuffer(std::unique_ptr<BNode> bNodeRoot);
        void encodeToFile(std::unique_ptr<BNode> bNodeRoot, std::string destinationFileName);
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
        std::unique_ptr<BNode> decodeToBNodes(ISource *source);
        void encodeFromBNodes(BNode *bNode, IDestination *destination);
        // =================
        // PRIVATE VARIABLES
        // =================
        std::string m_workBuffer;
    };
    //
    // Shortcuts for B node structure and Bencoding type
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
        return (std::equal(lhs.buffer.begin(), lhs.buffer.end(), rhs.buffer.begin()));
    }
} // namespace H4
#endif /* BENCODE_HPP */
