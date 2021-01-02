#ifndef BENCODE_HPP
#define BENCODE_HPP
//
// C++ STL
//
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <list>
#include <memory>
#include <fstream>
#include <iostream>
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
        /// <summary>
        /// Source/destination for bencoded strings.
        /// </summary>
        using Bencoding = std::basic_string<char>;
        /// <summary>
        /// Base for BNode structure.
        /// </summary>
        struct BNode
        {
            virtual ~BNode() = default;
        };
        /// <summary>
        /// Dictionary BNode.
        /// </summary>
        struct BNodeDict : BNode
        {
            std::map<std::string, std::unique_ptr<BNode>> dict;
        };
        /// <summary>
        /// List BNode.
        /// </summary>
        struct BNodeList : BNode
        {
            std::list<std::unique_ptr<BNode>> list;
        };
        /// <summary>
        /// Number BNode.
        /// </summary>
        struct BNodeInteger : BNode
        {
            long number;
            BNodeInteger(long number)
            {
                this->number = number;
            }
        };
        /// <summary>
        /// String BNode.
        /// </summary>
        struct BNodeString : BNode
        {
        public:
            std::string string;
            BNodeString(std::string string)
            {
                this->string = string;
            }
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
        std::unique_ptr<BNode> decodeBuffer(Bencoding source);
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
        class ISource
        {
        public:
            virtual unsigned char currentByte() = 0;
            virtual void moveToNextByte() = 0;
            virtual bool bytesToDecode() = 0;
        };
        class BufferSource : public ISource
        {
        public:
            BufferSource(Bencoding sourceBuffer)
            {
                m_decodeBuffer = sourceBuffer;
            }
            unsigned char currentByte()
            {
                return (m_decodeBuffer[m_bufferPosition]);
            }
            void moveToNextByte()
            {
                if (!bytesToDecode())
                {
                    throw std::runtime_error("Decode buffer empty before decode complete.");
                }
                m_bufferPosition++;
            }
            bool bytesToDecode()
            {
                return (m_bufferPosition < m_decodeBuffer.length());
            }
        private:
            std::size_t m_bufferPosition=0;
            Bencoding m_decodeBuffer;
        };
        class FileSource : public ISource
        {
        public:
            FileSource(std::string sourceFileName)
            {
                m_source.open(sourceFileName.c_str(), std::ios_base::in | std::ios_base::binary);
                if (!m_source.is_open())
                {
                    throw std::runtime_error("Bencode file input stream failed to open or does not exist.");
                }
            }
            unsigned char currentByte()
            {
                return (m_source.peek());
            }
            void moveToNextByte()
            {
                char c;
                m_source.get(c);
            }
            bool bytesToDecode()
            {
                return (!m_source.eof());
            }
        private:
            std::fstream m_source;
        };
        class IDestination
        {
        public:
            virtual void addBytes(std::string bytes) = 0;
        };
        class BufferDestination : public IDestination
        {
        public:
            BufferDestination()
            {
            }
            void addBytes(std::string bytes)
            {
                m_encodeBuffer += bytes;
            }
            std::string getBuffer()
            {
                return (m_encodeBuffer);
            }
        private:
            std::string m_encodeBuffer;
        };
        class FileDestination : public IDestination
        {
        public:
            FileDestination(std::string desinationFileName)
            {
                m_destination.open(desinationFileName.c_str(), std::ios_base::out | std::ios_base::binary);
                if (!m_destination.is_open())
                {
                    throw std::runtime_error("Bencode file output stream failed to open or could not be created.");
                }
            }
            void addBytes(std::string bytes) {
                m_destination.write(bytes.c_str(), bytes.length());
            }
        private:
            std::ofstream m_destination;
        };
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
    // Shortcuts for node structure
    //
    using BNode = Bencode::BNode;
    using BNodeInteger = Bencode::BNodeInteger;
    using BNodeString = Bencode::BNodeString;
    using BNodeList = Bencode::BNodeList;
    using BNodeDict = Bencode::BNodeDict;
} // namespace H4
#endif /* BENCODE_HPP */
