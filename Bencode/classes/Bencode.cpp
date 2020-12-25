//
// Class: Bencode
//
// Description: Class to perform Bencode string encode/decode either to/from
// a byte buffer or file stream when needed.
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
        while (std::isdigit(*m_decodeBuffer))
        {
            m_workBuffer += *m_decodeBuffer++;
        }
        return (std::stol(m_workBuffer));
    }

    inline std::string Bencode::decodeString()
    {
        int stringLength = decodeNumber();
        if (*m_decodeBuffer != ':')
        {
            throw std::runtime_error("Missing terminating ':' on string length.");
        }
        m_decodeBuffer++;
        m_workBuffer.clear();
        while (stringLength-- > 0)
        {
            m_workBuffer += *m_decodeBuffer++;
        }
        return (m_workBuffer);
    }

    std::unique_ptr<Bencode::BNode> Bencode::decodeToBNodes()
    {

        switch (*m_decodeBuffer)
        {
        case 'd':
        {
            m_decodeBuffer++;
            Bencode::BNodeDict bNodeDictionary;
            while (*m_decodeBuffer != 'e')
            {
                std::string key = decodeString();
                bNodeDictionary.dict[key] = decodeToBNodes();
            }
            m_decodeBuffer++;
            return (std::make_unique<Bencode::BNodeDict>(std::move(bNodeDictionary)));
        }
        case 'l':
        {
            m_decodeBuffer++;
            Bencode::BNodeList bNodeList;
            while (*m_decodeBuffer != 'e')
            {
                bNodeList.list.push_back(decodeToBNodes());
            }
            m_decodeBuffer++;
            return (std::make_unique<Bencode::BNodeList>(std::move(bNodeList)));
        }
        case 'i':
        {
            m_decodeBuffer++;
            long number = decodeNumber();
            if (*m_decodeBuffer != 'e')
            {
                throw std::runtime_error("Missing terminating 'e' on number.");
            }
            m_decodeBuffer++;
            return (std::make_unique<Bencode::BNodeNumber>(BNodeNumber(number)));
        }
        default:
            return (std::make_unique<Bencode::BNodeString>(BNodeString(decodeString())));
        }
    }
    // ==============
    // PUBLIC METHODS
    // ==============

    std::unique_ptr<Bencode::BNode> Bencode::decode(const char *toDecode)
    {
        m_decodeBuffer = toDecode;
        return decodeToBNodes();
    }

} // namespace H4
