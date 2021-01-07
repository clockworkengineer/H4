//
// Class: JSON
//
// Description: Class to perform JSON encoding encode/decode to/from
// a byte buffer or file. It is also  possible to customize this with the
// ISource and IDestination interfaces if required.
//
// Dependencies:   C17++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "JSON.hpp"
#include "JSONSources.hpp"
#include "JSONDestinations.hpp"
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
    // ==============
    // PUBLIC METHODS
    // ==============
    std::unique_ptr<JNode> JSON::decode(std::string jsonBuffer)
    {
        switch (jsonBuffer[0])
        {
        case '"':
            return (std::make_unique<JNode>(JNodeString(jsonBuffer)));
        case 't':
        case 'T':
            return (std::make_unique<JNode>(JNodeBoolean(true)));
        case 'f':
        case 'F':
            return (std::make_unique<JNode>(JNodeBoolean(false)));
        default:
            return (std::make_unique<JNode>(JNodeNumber(jsonBuffer)));
        }
    }
} // namespace H4
