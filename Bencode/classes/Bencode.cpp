//
// Class: Bencode
//
// Description: Class to perform Bencode encode/decode.
//
// Dependencies:   C17++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "Bencode.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
//
// C++ STL
//
#include <stdexcept>
// =========
// NAMESPACE
// =========
namespace H4
{
    // ===========================
    // PRIVATE TYPES AND CONSTANTS
    // ===========================
    // ==========================
    // PUBLIC TYPES AND CONSTANTS
    // ==========================
    // ========================
    // PRIVATE STATIC VARIABLES
    // ========================
    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================
    // ===============
    // PRIVATE METHODS
    // ===============
    inline long Bencode::decodePositiveInteger(ISource *source)
    {
        m_workBuffer.clear();
        while (source->bytesToDecode() && std::isdigit((char) source->currentByte()))
        {
            m_workBuffer += (char) source->currentByte();
            source->moveToNextByte();
        }
        return (std::stol(m_workBuffer));
    }
    inline std::string Bencode::decodeString(ISource *source)
    {
        int stringLength = decodePositiveInteger(source);
        if (source->currentByte() != (std::byte) ':')
        {
            throw std::runtime_error("Missing terminating ':' on string length.");
        }
        source->moveToNextByte();
        m_workBuffer.clear();
        while (stringLength-- > 0)
        {
            m_workBuffer += (char) source->currentByte();
            source->moveToNextByte();
        }
        return (m_workBuffer);
    }
    std::unique_ptr<BNode> Bencode::decodeToBNodes(ISource *source)
    {
        switch ((char) source->currentByte())
        {
        case 'd':
        {
            source->moveToNextByte();
            BNodeDict bNodeDictionary;
            while (source->bytesToDecode() && source->currentByte() != (std::byte) 'e')
            {
                std::string key = decodeString(source);
                bNodeDictionary.dict[key] = decodeToBNodes(source);
            }
            if (!source->bytesToDecode())
            {
                throw std::runtime_error("Missing terminating 'e' on dictionary.");
            }
            source->moveToNextByte();
            return (std::make_unique<BNodeDict>(std::move(bNodeDictionary)));
        }
        case 'l':
        {
            source->moveToNextByte();
            BNodeList bNodeList;
            while (source->bytesToDecode() && source->currentByte() !=  (std::byte) 'e')
            {
                bNodeList.list.push_back(decodeToBNodes(source));
            }
            if (!source->bytesToDecode())
            {
                throw std::runtime_error("Missing terminating 'e' on list.");
            }
            source->moveToNextByte();
            return (std::make_unique<BNodeList>(std::move(bNodeList)));
        }
        case 'i':
        {
            long integer = 1;
            source->moveToNextByte();
            if (source->currentByte() ==  (std::byte)'-')
            {
                source->moveToNextByte();
                integer = -1;
            }
            integer *= decodePositiveInteger(source);
            if (source->currentByte() !=  (std::byte) 'e')
            {
                throw std::runtime_error("Missing terminating 'e' on integer.");
            }
            source->moveToNextByte();
            return (std::make_unique<BNodeInteger>(BNodeInteger(integer)));
        }
        default:
            return (std::make_unique<BNodeString>(BNodeString(decodeString(source))));
        }
    }
    void Bencode::encodeFromBNodes(BNode *bNode, IDestination *destination)
    {
        if (dynamic_cast<BNodeDict *>(bNode) != nullptr)
        {
            destination->addBytes("d");
            for (auto &bNodeEntry : ((BNodeDict *)bNode)->dict)
            {
                destination->addBytes(std::to_string(bNodeEntry.first.length()) + ":" + bNodeEntry.first);
                encodeFromBNodes(bNodeEntry.second.get(), destination);
            }
            destination->addBytes("e");
            return;
        }
        if (dynamic_cast<BNodeList *>(bNode) != nullptr)
        {
            destination->addBytes("l");
            for (auto &bNodeEntry : ((BNodeList *)bNode)->list)
            {
                encodeFromBNodes(bNodeEntry.get(), destination);
            }
            destination->addBytes("e");
            return;
        }
        if (dynamic_cast<BNodeInteger *>(bNode) != nullptr)
        {
            destination->addBytes("i" + std::to_string(((BNodeInteger *)(bNode))->number) + "e");
            return;
        }
        if (dynamic_cast<BNodeString *>(bNode) != nullptr)
        {
            std::string stringToEncode = ((BNodeString *)(bNode))->string;
            destination->addBytes(std::to_string((int)stringToEncode.length()) + ":" + stringToEncode);
            return;
        }
        throw std::runtime_error("Unknown BNode type encountered during encode.");
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    std::unique_ptr<BNode> Bencode::decodeBuffer(Bencoding sourceBuffer)
    {
        if (sourceBuffer.buffer.size()==0)
        {
            throw std::invalid_argument("Empty string passed to be decoded.");
        }
        BufferSource source(sourceBuffer);
        return decodeToBNodes(&source);
    }
    std::unique_ptr<BNode> Bencode::decodeFile(std::string sourceFileName)
    {
        FileSource source(sourceFileName);
        return decodeToBNodes(&source);
    }
    Bencode::Bencoding Bencode::encodeToBuffer(std::unique_ptr<BNode> bNodeRoot)
    {
        if (bNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as bNode to be encoded.");
        }
        BufferDestination destination;
        encodeFromBNodes(bNodeRoot.get(), &destination);
        return (destination.getBuffer());
    }
    void Bencode::encodeToFile(std::unique_ptr<BNode> bNodeRoot, std::string destinationFileName)
    {
        if (bNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as bNode to be encoded.");
        }
        FileDestination destination(destinationFileName);
        encodeFromBNodes(bNodeRoot.get(), &destination);
    }
} // namespace H4
