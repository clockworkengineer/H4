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

    std::string Bencode::decodeString()
    {
        m_workBuffer.clear();
        while (std::isdigit(*m_decodeBuffer))
        {
            m_workBuffer += *m_decodeBuffer++;
        }
        m_decodeBuffer++;
        int stringLength = std::stoi(m_workBuffer);
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
            Bencode::BNodeDictionary bNodeDictionary;
            while (*m_decodeBuffer != 'e')
            {
                std::string key = decodeString();
                bNodeDictionary.dict[key] = decodeToBNodes();
            }
            return (std::make_unique<Bencode::BNodeDictionary>(std::move(bNodeDictionary)));
        }
        case 'l':
        {
            m_decodeBuffer++;
            Bencode::BNodeList bNodeList;
            while (*m_decodeBuffer != 'e')
            {
                bNodeList.list.push_back(decodeToBNodes());
            }
            return (std::make_unique<Bencode::BNodeList>(std::move(bNodeList)));
        }
        case 'i':
            m_workBuffer.clear();
            m_decodeBuffer++;
            while (std::isdigit(*m_decodeBuffer))
            {
                m_workBuffer += *m_decodeBuffer++;
            }
            m_decodeBuffer++;
            return (std::make_unique<Bencode::BNodeNumber>(BNodeNumber(m_workBuffer)));
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
