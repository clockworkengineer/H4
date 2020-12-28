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
        while (source->bytesToDecode() && std::isdigit(source->currentByte()))
        {
            m_workBuffer += source->currentByte();
            source->moveToNextByte();
        }
        return (std::stol(m_workBuffer));
    }

    inline std::string Bencode::decodeString(ISource *source)
    {
        int stringLength = decodePositiveInteger(source);
        if (source->currentByte() != ':')
        {
            throw std::runtime_error("Missing terminating ':' on string length.");
        }
        source->moveToNextByte();
        m_workBuffer.clear();
        while (stringLength-- > 0)
        {
            m_workBuffer += source->currentByte();
            source->moveToNextByte();
        }
        return (m_workBuffer);
    }

    std::unique_ptr<BNode> Bencode::decodeToBNodes(ISource *source)
    {

        switch (source->currentByte())
        {
        case 'd':
        {
            source->moveToNextByte();
            BNodeDict bNodeDictionary;
            while (source->bytesToDecode() && source->currentByte() != 'e')
            {
                std::string key = decodeString(source);
                bNodeDictionary.dict[key] = decodeToBNodes(source);
            }
            if (source->currentByte() != 'e')
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
            while (source->bytesToDecode() && source->currentByte() != 'e')
            {
                bNodeList.list.push_back(decodeToBNodes(source));
            }
            if (source->currentByte() != 'e')
            {
                throw std::runtime_error("Missing terminating 'e' on list.");
            }
            source->moveToNextByte();
            return (std::make_unique<BNodeList>(std::move(bNodeList)));
        }
        case 'i':
        {
            long number = 1;
            source->moveToNextByte();
            if (source->currentByte() == '-')
            {
                source->moveToNextByte();
                number = -1;
            }
            number *= decodePositiveInteger(source);
            if (source->currentByte() != 'e')
            {
                throw std::runtime_error("Missing terminating 'e' on integer.");
            }
            source->moveToNextByte();
            return (std::make_unique<BNodeInteger>(BNodeInteger(number)));
        }
        default:
            return (std::make_unique<BNodeString>(BNodeString(decodeString(source))));
        }
    }

    std::string Bencode::encodeFromBNodes(BNode *bNode)
    {
        if (dynamic_cast<BNodeDict *>(bNode) != nullptr)
        {
            std::string result = "d";
            for (auto &bNodeEntry : ((BNodeDict *)bNode)->dict)
            {
                result += std::to_string(bNodeEntry.first.length()) + ":" + bNodeEntry.first;
                result += encodeFromBNodes(bNodeEntry.second.get());
            }
            result += "e";
            return (result);
        }
        if (dynamic_cast<BNodeList *>(bNode) != nullptr)
        {
            std::string result = "l";
            for (auto &bNodeEntry : ((BNodeList *)bNode)->list)
            {
                result += encodeFromBNodes(bNodeEntry.get());
            }
            result += "e";
            return (result);
        }
        if (dynamic_cast<BNodeInteger *>(bNode) != nullptr)
        {
            return ("i" + std::to_string(((BNodeInteger *)(bNode))->number) + "e");
        }
        if (dynamic_cast<BNodeString *>(bNode) != nullptr)
        {
            std::string stringToEncode = ((BNodeString *)(bNode))->string;
            return (std::to_string((int)stringToEncode.length()) + ":" + stringToEncode);
        }
        throw std::runtime_error("Unknown BNode type encountered during encode.");
    }

    // ==============
    // PUBLIC METHODS
    // ==============

    std::unique_ptr<BNode> Bencode::decode(const char *toDecode)
    {
        if ((toDecode == nullptr) || (*toDecode == '\0'))
        {
            throw std::invalid_argument("nullptr/empty string passed to be decoded.");
        }
        return decodeToBNodes(std::make_unique<BufferSource>(BufferSource(toDecode)).get());
    }

    std::string Bencode::encode(std::unique_ptr<BNode> bNode)
    {
        return (encodeFromBNodes(bNode.get()));
    }

} // namespace H4
