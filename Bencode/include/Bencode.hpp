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
//
// Bencode types
//
#include "BNode.hpp"
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
    using Bencoding = Bencode::Bencoding;
} // namespace H4
#endif /* BENCODE_HPP */
