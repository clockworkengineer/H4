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

        std::unique_ptr<BNode> decode(const char *source);
        std::unique_ptr<BNode> decodeFile(std::string fileName);
        std::string encode(std::unique_ptr<BNode> bNodeRoot);

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
            BufferSource(const char *toDecode)
            {
                m_decodeBuffer = std::string_view(toDecode);
            }

            unsigned char currentByte()
            {
                return (m_decodeBuffer[0]);
            }

            void moveToNextByte()
            {
                if (m_decodeBuffer.empty())
                {
                    throw std::runtime_error("Decode buffer empty before decode complete.");
                }
                m_decodeBuffer.remove_prefix(1);
            }

            bool bytesToDecode()
            {
                return !m_decodeBuffer.empty();
            }

        private:
            std::string_view m_decodeBuffer;
        };

        class FileSource : public ISource
        {
        public:
            FileSource(std::string fileName)
            {
                m_source.open(fileName.c_str(), std::ios_base::in | std::ios_base::binary);
                if (!m_source.is_open())
                {
                    throw std::runtime_error("File input stream source failed to open.");
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

        // ===========================================
        // DISABLED CONSTRUCTORS/DESTRUCTORS/OPERATORS
        // ===========================================

        // ===============
        // PRIVATE METHODS
        // ===============

        long decodePositiveInteger(ISource *source);
        std::string decodeString(ISource *source);
        std::unique_ptr<BNode> decodeToBNodes(ISource *source);
        std::string encodeFromBNodes(BNode *bNode);

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
