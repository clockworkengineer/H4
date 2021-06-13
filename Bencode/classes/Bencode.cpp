//
// Class: Bencode
//
// Description: Class to perform Bencode encoding encode/decode to/from
// a byte buffer or file. It is also  possible to customize this with the
// ISource and IDestination interfaces if required. Although bencoded
// data is treated as std::byte externally this library uses char and
// std::string internally.Note: At present it will report incorrect Bencode
// syntax but will not be specific about what error has occurred; this
// is reasoned to add too much overhead to the process of parsing for the
// requirements of this library (this might change in future versions).
// For an indepth desciption of the Bencoding specification refer to its
// wiki page at https://en.wikipedia.org/wiki/Bencode.
//
// Dependencies:   C17++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "BencodeConfig.hpp"
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
    /// <summary>
    /// Extract a positive Integer from the input stream of characters referenced by ISource.
    /// </summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>
    /// <returns>Positive integers value.</returns>
    inline long Bencode::extractPositiveInteger(ISource &source)
    {
        m_workBuffer.clear();
        while (source.bytesToDecode() && std::isdigit((char)source.currentByte()))
        {
            m_workBuffer += (char)source.currentByte();
            source.moveToNextByte();
        }
        return (std::stol(m_workBuffer));
    }
    /// <summary>
    /// Extract a byte string from the input stream of characters referenced by ISource.
    /// </summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>
    /// <returns>String value decoded.</returns>
    inline std::string Bencode::extractString(ISource &source)
    {
        long stringLength = extractPositiveInteger(source);
        if (source.currentByte() != (std::byte)':')
        {
            throw Bencode::SyntaxError();
        }
        source.moveToNextByte();
        m_workBuffer.clear();
        while (stringLength-- > 0)
        {
            m_workBuffer += (char)source.currentByte();
            source.moveToNextByte();
        }
        return (m_workBuffer);
    }
    /// <summary>
    /// Decode a byte string from the input stream of characters referenced by ISource.
    /// </summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>
    /// <returns>String BNode.</returns>
    std::unique_ptr<BNode> Bencode::decodeString(ISource &source)
    {
        return (std::make_unique<BNodeString>(BNodeString(extractString(source))));
    }
    /// <summary>
    /// Decode an integer from the input stream of characters referenced by ISource.
    /// </summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>
    /// <returns>Integer BNode.</returns>
    std::unique_ptr<BNode> Bencode::decodeInteger(ISource &source)
    {
        long integer = 1;
        source.moveToNextByte();
        if (source.currentByte() == (std::byte)'-')
        {
            source.moveToNextByte();
            integer = -1;
        }
        integer *= extractPositiveInteger(source);
        if (source.currentByte() != (std::byte)'e')
        {
            throw Bencode::SyntaxError();
        }
        source.moveToNextByte();
        return (std::make_unique<BNodeInteger>(BNodeInteger(integer)));
    }
    /// <summary>
    /// Decode a dictionary from the input stream of characters referenced by ISource.
    /// </summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>
    /// <returns>Dictionary BNode.</returns>
    std::unique_ptr<BNode> Bencode::decodeDictionary(ISource &source)
    {
        std::unique_ptr<BNode> bNode = std::make_unique<BNodeDict>();
        source.moveToNextByte();
        while (source.bytesToDecode() && source.currentByte() != (std::byte)'e')
        {
            std::string key = extractString(source);
            BNodeRef<BNodeDict>(*bNode).addEntry(key, decodeBNodes(source));
        }
        if (source.currentByte() != (std::byte)'e')
        {
            throw Bencode::SyntaxError();
        }
        source.moveToNextByte();
        return (bNode);
    }
    /// <summary>
    /// Decode a list from the input stream of characters referenced by ISource.
    /// </summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>
    /// <returns>List BNode.</returns>
    std::unique_ptr<BNode> Bencode::decodeList(ISource &source)
    {
        std::unique_ptr<BNode> bNode = std::make_unique<BNodeList>();
        source.moveToNextByte();
        while (source.bytesToDecode() && source.currentByte() != (std::byte)'e')
        {
            BNodeRef<BNodeList>(*bNode).addEntry(decodeBNodes(source));
        }
        if (source.currentByte() != (std::byte)'e')
        {
            throw Bencode::SyntaxError();
        }
        source.moveToNextByte();
        return (bNode);
    }
    /// <summary>
    /// Decode a BNode from the input stream of characters referenced by ISource.In order to traverse
    //  and decode complex encodings this method is called recursively to build up a BNode structure.
    /// </summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>
    /// <returns>Root BNode.</returns>
    std::unique_ptr<BNode> Bencode::decodeBNodes(ISource &source)
    {
        switch ((char)source.currentByte())
        {
        // Dictionary BNode
        case 'd':
            return (decodeDictionary(source));
        // List BNode
        case 'l':
            return (decodeList(source));
        // Integer BNode
        case 'i':
            return (decodeInteger(source));
        // String BNode
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return (decodeString(source));
        }
        throw Bencode::SyntaxError();
    }
    /// <summary>
    /// Recursively traverse a BNode structure and produce an Bencode encoding of it on the output
    //  stream referenced through the IDestination interface.
    /// </summary>
    /// <param name="bNode">Pointer to root of current BNode structure.</param>
    /// <param name="desination ">Pointer to interface used to facilitate the output stream.</param>
    /// <returns></returns>
    void Bencode::encodeBNodes(BNode *bNode, IDestination &destination)
    {
        switch (bNode->nodeType)
        {
        case BNodeType::dictionary:
            destination.addBytes("d");
            for (auto &bNodeEntry : ((BNodeDict *)bNode)->getDict())
            {
                destination.addBytes(std::to_string(bNodeEntry.first.length()) + ":" + bNodeEntry.first);
                encodeBNodes(bNodeEntry.second.get(), destination);
            }
            destination.addBytes("e");
            break;
        case BNodeType::list:
            destination.addBytes("l");
            for (auto &bNodeEntry : BNodeRef<BNodeList>(*bNode).getArray())
            {
                encodeBNodes(bNodeEntry.get(), destination);
            }
            destination.addBytes("e");
            break;
        case BNodeType::integer:
            destination.addBytes("i" + std::to_string(BNodeRef<BNodeInteger>(*bNode).getInteger()) + "e");
            break;
        case BNodeType::string:
        {
            std::string stringToEncode = BNodeRef<BNodeString>(*bNode).getString();
            destination.addBytes(std::to_string((int)stringToEncode.length()) + ":" + stringToEncode);
            break;
        }
        default:
            throw std::runtime_error("Unknown BNode type encountered during encode.");
        }
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Decode Bencoded source buffer into BNode(s).
    /// </summary>
    /// <param name="sourceBuffer">Source byte buffer containing Bencoding.</param>
    /// <returns>BNode structure root.</returns>
    std::unique_ptr<BNode> Bencode::decodeBuffer(const Bencoding &sourceBuffer)
    {
        if (sourceBuffer.isEmpty())
        {
            throw std::invalid_argument("Empty string passed to be decoded.");
        }
        BufferSource source(sourceBuffer);
        return decodeBNodes(source);
    }
    /// <summary>
    /// Decode Bencoded source file into BNode(s)
    /// </summary>
    /// <param name="sourceFileName">Input source file name</param>
    /// <returns>BNode structure root.</returns>
    std::unique_ptr<BNode> Bencode::decodeFile(const std::string &sourceFileName)
    {
        if (sourceFileName.empty())
        {
            throw std::invalid_argument("Empty string passed for file name.");
        }
        FileSource source(std::move(sourceFileName));
        return decodeBNodes(source);
    }
    /// <summary>
    /// Take BNode structure and create an Bencode encoding for it in a destination buffer.
    /// </summary>
    /// <param name="bNodeRoot">Bnode structure root.</param>
    /// <returns>Encoded data buffer.</returns>
    Bencoding Bencode::encodeBuffer(std::unique_ptr<BNode> bNodeRoot)
    {
        if (bNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as bNode to be encoded.");
        }
        BufferDestination destination;
        encodeBNodes(bNodeRoot.get(), destination);
        return (destination.getBuffer());
    }
    /// <summary>
    /// Take BNode structure and create an Bencode encoding for it in a destination file.
    /// </summary>
    /// <param name="bNodeRoot">Bnode structure root.</param>
    /// <param name="destinationFileName">Destination file name.</param>
    void Bencode::encodeFile(std::unique_ptr<BNode> bNodeRoot, const std::string &destinationFileName)
    {
        if (bNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as bNode to be encoded.");
        }
        if (destinationFileName.empty())
        {
            throw std::invalid_argument("Empty string passed for file name.");
        }
        FileDestination destination(std::move(destinationFileName));
        encodeBNodes(bNodeRoot.get(), destination);
    }
    /// <summary>
    /// Decode Bencoded byte string pointed to by source stream into BNode(s).
    /// </summary>
    /// <param name="source">Pointer to input interface used to decode Bencoded stream.</param>
    /// <returns>BNode structure root.</returns>
    std::unique_ptr<BNode> Bencode::decode(ISource &source)
    {
        return decodeBNodes(source);
    }
    /// <summary>
    /// Take BNode structure and create an Bencode encoding for it in the destination stream.
    /// </summary>
    /// <param name="bNodeRoot">Bnode structure root.</param>
    /// <param name="desination ">Pointer to interface used to facilitate the output stream.</param>
    void Bencode::encode(std::unique_ptr<BNode> bNodeRoot, IDestination &destination)
    {
        encodeBNodes(bNodeRoot.get(), destination);
    }
} // namespace H4
