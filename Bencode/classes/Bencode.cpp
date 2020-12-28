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

    inline unsigned char Bencode::currentByte() {
        return (m_decodeBuffer[0]);
    }

    inline void Bencode::moveToNextByte() {
        if (m_decodeBuffer.empty()) {
            throw std::runtime_error("Decode buffer empty before decode complete.");
        }
        m_decodeBuffer.remove_prefix(1);
    }

    inline bool Bencode::bytesToDecode() {
        return !m_decodeBuffer.empty();
    }

    inline long Bencode::decodePositiveInteger()
    {
        m_workBuffer.clear();
        while (bytesToDecode() && std::isdigit(currentByte()))
        {
            m_workBuffer += currentByte();
            moveToNextByte();
        }
        return (std::stol(m_workBuffer));
    }

    inline std::string Bencode::decodeString()
    {
        int stringLength = decodePositiveInteger();
        if (currentByte() != ':')
        {
            throw std::runtime_error("Missing terminating ':' on string length.");
        }
        moveToNextByte();
        m_workBuffer.clear();
        while (stringLength-- > 0)
        {
            m_workBuffer += currentByte();
            moveToNextByte();
        }
        return (m_workBuffer);
    }

    std::unique_ptr<BNode> Bencode::decodeToBNodes()
    {

        switch (currentByte())
        {
        case 'd':
        {
            moveToNextByte();
            BNodeDict bNodeDictionary;
            while (bytesToDecode() && currentByte() != 'e')
            {
                std::string key = decodeString();
                bNodeDictionary.dict[key] = decodeToBNodes();
            }
            if (currentByte() != 'e')
            {
                throw std::runtime_error("Missing terminating 'e' on dictionary.");
            }
            moveToNextByte();
            return (std::make_unique<BNodeDict>(std::move(bNodeDictionary)));
        }
        case 'l':
        {
            moveToNextByte();
            BNodeList bNodeList;
            while (bytesToDecode() && currentByte() != 'e')
            {
                bNodeList.list.push_back(decodeToBNodes());
            }
            if (currentByte() != 'e')
            {
                throw std::runtime_error("Missing terminating 'e' on list.");
            }
            moveToNextByte();
            return (std::make_unique<BNodeList>(std::move(bNodeList)));
        }
        case 'i':
        {
            long number = 1;
            moveToNextByte();
            if (currentByte() == '-')
            {
                moveToNextByte();
                number = -1;
            }
            number *= decodePositiveInteger();
            if (currentByte() != 'e')
            {
                throw std::runtime_error("Missing terminating 'e' on integer.");
            }
            moveToNextByte();
            return (std::make_unique<BNodeInteger>(BNodeInteger(number)));
        }
        default:
            return (std::make_unique<BNodeString>(BNodeString(decodeString())));
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
        m_decodeBuffer = std::string_view(toDecode);
        return decodeToBNodes();
    }

    std::string Bencode::encode(std::unique_ptr<BNode> bNode)
    {
        return (encodeFromBNodes(bNode.get()));
    }

} // namespace H4
