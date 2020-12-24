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
        int position=0;
        std::string workBuffer;
        switch (toDecode[position])
        {
        case 'i':
            while(std::isdigit(toDecode[++position])) {
                workBuffer += toDecode[position];
            }
            return (std::make_unique<Bencode::BNodeNumber>(BNodeNumber(workBuffer)));
        default:
            return (std::make_unique<Bencode::BNodeString>(BNodeString()));
        }
    } 

} // namespace H4
