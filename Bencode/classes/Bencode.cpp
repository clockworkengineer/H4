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

    // ==============
    // PUBLIC METHODS
    // ==============

    std::unique_ptr<Bencode::BNode> Bencode::decode(const char *toDecode)
    {
        std::string workBuffer;
        switch (*toDecode)
        {
        case 'i':
            toDecode++;
            while (std::isdigit(*toDecode))
            {
                workBuffer += *toDecode++;
            }
            return (std::make_unique<Bencode::BNodeNumber>(BNodeNumber(workBuffer)));
        default:
            while (std::isdigit(*toDecode))
            {
                workBuffer += *toDecode++;
            }
            toDecode++;
            int stringLength = std::stoi(workBuffer);
            workBuffer.clear();
            while (stringLength-- > 0)
            {
                workBuffer += *toDecode++;
            }
            return (std::make_unique<Bencode::BNodeString>(BNodeString(workBuffer)));
        }
    }

} // namespace H4
