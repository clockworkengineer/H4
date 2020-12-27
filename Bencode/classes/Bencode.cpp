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

    inline long Bencode::decodeNumber()
    {
        m_workBuffer.clear();
        while (std::isdigit(m_decodeBuffer[0]))
        {
            m_workBuffer += m_decodeBuffer[0];
            m_decodeBuffer.remove_prefix(1);
        }
        return (std::stol(m_workBuffer));
    }

    inline std::string Bencode::decodeString()
    {
        int stringLength = decodeNumber();
        if (m_decodeBuffer[0] != ':')
        {
            throw std::runtime_error("Missing terminating ':' on string length.");
        }
        m_decodeBuffer.remove_prefix(1);
        m_workBuffer.clear();
        while (stringLength-- > 0)
        {
            m_workBuffer += m_decodeBuffer[0];
            m_decodeBuffer.remove_prefix(1);
        }
        return (m_workBuffer);
    }

    std::unique_ptr<BNode> Bencode::decodeToBNodes()
    {

        switch (m_decodeBuffer[0])
        {
        case 'd':
        {
            m_decodeBuffer.remove_prefix(1);
            BNodeDict bNodeDictionary;
            while (m_decodeBuffer[0] != 'e')
            {
                std::string key = decodeString();
                bNodeDictionary.dict[key] = decodeToBNodes();
            }
            m_decodeBuffer.remove_prefix(1);
            return (std::make_unique<BNodeDict>(std::move(bNodeDictionary)));
        }
        case 'l':
        {
            m_decodeBuffer.remove_prefix(1);
            BNodeList bNodeList;
            while (m_decodeBuffer[0] != 'e')
            {
                bNodeList.list.push_back(decodeToBNodes());
            }
            m_decodeBuffer.remove_prefix(1);
            return (std::make_unique<BNodeList>(std::move(bNodeList)));
        }
        case 'i':
        {
            m_decodeBuffer.remove_prefix(1);
            long number = decodeNumber();
            if (m_decodeBuffer[0] != 'e')
            {
                throw std::runtime_error("Missing terminating 'e' on number.");
            }
            m_decodeBuffer.remove_prefix(1);
            return (std::make_unique<BNodeNumber>(BNodeNumber(number)));
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
        if (dynamic_cast<BNodeNumber *>(bNode) != nullptr)
        {
            return ("i" + std::to_string(((BNodeNumber *)(bNode))->number) + "e");
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
        m_decodeBuffer = std::string_view(toDecode);
        return decodeToBNodes();
    }

    std::string Bencode::encode(std::unique_ptr<BNode> bNode)
    {
        return (encodeFromBNodes(bNode.get()));
    }

} // namespace H4
