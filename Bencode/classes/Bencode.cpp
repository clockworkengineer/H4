//
// Class: Bencode
//
// Description: Class to perform Bencode encoding encode/decode to/from
// a byte buffer or file. It is also  possible to customize this with the 
// ISource and IDestination interfaces if required.
//
// Dependencies:   C17++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "Bencode.hpp"
#include "BencodeSources.hpp"
#include "BencodeDestinations.hpp"
// ====================
// CLASS IMPLEMENTATION
// ====================
//
// C++ STL
//
#include <stdexcept>
#include <utility>
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
    /// <summary>Decode a positive Integer from the input stream of characters referenced by ISource.</summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>  
    /// <returns>Positive integers value.</returns> 
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
    /// <summary>Decode a byte string from the input stream of characters referenced by ISource.</summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>  
    /// <returns>String value decoded.</returns> 
    inline std::string Bencode::decodeString(ISource *source)
    {
        long stringLength = decodePositiveInteger(source);
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
    /// <summary>Decode a BNode from the input stream of characters referenced by ISource.In order to traverse
    //  and decode complex encodings this method is called recursively to build up a BNode structure.</summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>  
    /// <returns></returns> 
    std::unique_ptr<BNode> Bencode::decodeToBNodes(ISource *source)
    {
        switch ((char) source->currentByte())
        {
        // Dictionary BNode
        case 'd':
        {
            source->moveToNextByte();
            BNodeDict bNodeDictionary;
            while (source->bytesToDecode() && source->currentByte() != (std::byte) 'e')
            {
                std::string key = decodeString(source);
                bNodeDictionary.value[key] = decodeToBNodes(source);
            }
            if (!source->bytesToDecode())
            {
                throw std::runtime_error("Missing terminating 'e' on dictionary.");
            }
            source->moveToNextByte();
            return (std::make_unique<BNodeDict>(std::move(bNodeDictionary)));
        }
        // List BNode
        case 'l':
        {
            source->moveToNextByte();
            BNodeList bNodeList;
            while (source->bytesToDecode() && source->currentByte() !=  (std::byte) 'e')
            {
                bNodeList.value.push_back(decodeToBNodes(source));
            }
            if (!source->bytesToDecode())
            {
                throw std::runtime_error("Missing terminating 'e' on list.");
            }
            source->moveToNextByte();
            return (std::make_unique<BNodeList>(std::move(bNodeList)));
        }
        // Integer BNode
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
        // String BNode
        default:
            return (std::make_unique<BNodeString>(BNodeString(decodeString(source))));
        }
    }
    /// <summary>Recursively traverse a BNode structure and produce an Bencode encoding of it on the output
    //  stream referenced through the IDestination interface.</summary>
    /// <param name="bNode">Pointer to root of current BNode structure.</param>
    /// <param name="desination ">Pointer to interface used to facilitate the output stream.</param>  
    /// <returns></returns> 
    void Bencode::encodeFromBNodes(BNode *bNode, IDestination *destination)
    {
        // Dictionary BNode
        if (dynamic_cast<BNodeDict *>(bNode) != nullptr)
        {
            destination->addBytes("d");
            for (auto &bNodeEntry : ((BNodeDict *)bNode)->value)
            {
                destination->addBytes(std::to_string(bNodeEntry.first.length()) + ":" + bNodeEntry.first);
                encodeFromBNodes(bNodeEntry.second.get(), destination);
            }
            destination->addBytes("e");
            return;
        }
        // List BNode
        if (dynamic_cast<BNodeList *>(bNode) != nullptr)
        {
            destination->addBytes("l");
            for (auto &bNodeEntry : ((BNodeList *)bNode)->value)
            {
                encodeFromBNodes(bNodeEntry.get(), destination);
            }
            destination->addBytes("e");
            return;
        }
        // Integer BNode
        if (dynamic_cast<BNodeInteger *>(bNode) != nullptr)
        {
            destination->addBytes("i" + std::to_string(((BNodeInteger *)(bNode))->value) + "e");
            return;
        }
        // Byte string BNode
        if (dynamic_cast<BNodeString *>(bNode) != nullptr)
        {
            std::string stringToEncode = ((BNodeString *)(bNode))->value;
            destination->addBytes(std::to_string((int)stringToEncode.length()) + ":" + stringToEncode);
            return;
        }
        // Unkown BNode so throw
        throw std::runtime_error("Unknown BNode type encountered during encode.");
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>Decode Bencoded source buffer into BNode(s).</summary>
    /// <param name="sourceBuffer">Source byte buffer containing Bencoding.</param>  
    /// <returns>BNode structure root.</returns> 
    std::unique_ptr<BNode> Bencode::decodeBuffer(const Bencoding& sourceBuffer)
    {
        if (sourceBuffer.buffer.empty())
        {
            throw std::invalid_argument("Empty string passed to be decoded.");
        }
        BufferSource source(sourceBuffer);
        return decodeToBNodes(&source);
    }
    /// <summary>Decode Bencoded source file into BNode(s)</summary>
    /// <param name="sourceFileName">Input source file name</param>  
    /// <returns>BNode structure root.</returns> 
    std::unique_ptr<BNode> Bencode::decodeFile(std::string sourceFileName)
    {
        FileSource source(std::move(sourceFileName));
        return decodeToBNodes(&source);
    }
    /// <summary></summary>
    /// <param name="bNodeRoot"></param>  
    /// <returns></returns> 
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
    /// <summary></summary>
    /// <param name="bNodeRoot"></param> 
    /// <param name="destinationFileName"></param>  
    /// <returns></returns> 
    void Bencode::encodeToFile(std::unique_ptr<BNode> bNodeRoot, std::string destinationFileName)
    {
        if (bNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as bNode to be encoded.");
        }
        FileDestination destination(std::move(destinationFileName));
        encodeFromBNodes(bNodeRoot.get(), &destination);
    }
} // namespace H4
