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

    std::unique_ptr<Bencode::BNode> Bencode::decodeToBNode()
    {

        switch (*m_decodeBuffer)
        {
        case 'd':
            return (std::make_unique<Bencode::BNodeDictionary>(BNodeDictionary()));
        case 'l':
        {
            m_decodeBuffer++;
            Bencode::BNodeList bNodeList;
            while (*m_decodeBuffer != 'e')
            {
                bNodeList.list.push_back(decodeToBNode());
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
            return (std::make_unique<Bencode::BNodeString>(BNodeString(m_workBuffer)));
        }
    }
    // ==============
    // PUBLIC METHODS
    // ==============

    std::unique_ptr<Bencode::BNode> Bencode::decode(const char *toDecode)
    {
        m_decodeBuffer = toDecode;
        return decodeToBNode();
    }

} // namespace H4
