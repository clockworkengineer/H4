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
#include <set>
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
    inline void JSON::ignoreWhiteSpace(ISource *source)
    {
        while (source->bytesToDecode() && std::iswspace(source->currentByte()))
        {
            source->moveToNextByte();
        }
    }
    std::string JSON::extractString(ISource *source)
    {
        std::string value;
        source->moveToNextByte();
        while (source->bytesToDecode() && source->currentByte() != '"')
        {
            value += source->currentByte();
            source->moveToNextByte();
        }
        source->moveToNextByte();
        return (value);
    }
    std::unique_ptr<JNode> JSON::decodeString(ISource *source)
    {
        return (std::make_unique<JNodeString>(JNodeString(extractString(source))));
    }
    std::unique_ptr<JNode> JSON::decodeNumber(ISource *source)
    {
        std::set<char> validCharacters { '1','2','3','4','5','6','7','8','9','0','.','-'};
        if (validCharacters.count(source->currentByte())==0) {
            throw std::runtime_error(std::string("Invalid numeric character ")+source->currentByte()+'.');
        }
        std::string value{source->currentByte()};
        source->moveToNextByte();
        while (source->bytesToDecode() && validCharacters.count(source->currentByte())>0)
        {
            value += source->currentByte();
            source->moveToNextByte();
        }
        return (std::make_unique<JNodeNumber>(JNodeNumber(value)));
    }
    std::unique_ptr<JNode> JSON::decodeBoolean(ISource *source)
    {
        std::string value{source->currentByte()};
        source->moveToNextByte();
        while (source->bytesToDecode() && std::isalpha(source->currentByte()))
        {
            value += source->currentByte();
            source->moveToNextByte();
        }
        if (value == "true")
        {
            return (std::make_unique<JNodeBoolean>(JNodeBoolean(true)));
        }
        else if (value == "false")
        {
            return (std::make_unique<JNodeBoolean>(JNodeBoolean(false)));
        }
        throw std::runtime_error("Invalid potential boolean value.");
    }
    std::unique_ptr<JNode> JSON::decodeNull(ISource *source)
    {
        std::string value{source->currentByte()};
        source->moveToNextByte();
        while (source->bytesToDecode() && std::isalpha(source->currentByte()))
        {
            value += source->currentByte();
            source->moveToNextByte();
        }
        if (value == "null")
        {
            return (std::make_unique<JNodeNull>(JNodeNull()));
        }
        throw std::runtime_error("Invalid potential null value.");
    }
    std::unique_ptr<JNode> JSON::decodeObject(ISource *source)
    {
        JNodeObject object;
        do
        {
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            std::string key = extractString(source);
            ignoreWhiteSpace(source);
            if (source->currentByte() != ':')
            {
                throw new std::runtime_error("Missing ':' after key value");
            }
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            object.value[key] = decodeJNodes(source);
            ignoreWhiteSpace(source);
        } while (source->currentByte() == ',');
        if (source->currentByte() != '}')
        {
            throw new std::runtime_error("Missing terminating '}' for object.");
        }
        source->moveToNextByte();
        return (std::make_unique<JNodeObject>(std::move(object)));
    }
    std::unique_ptr<JNode> JSON::decodeArray(ISource *source)
    {
        JNodeArray array;
        do
        {
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            array.value.push_back(decodeJNodes(source));
            ignoreWhiteSpace(source);
        } while (source->currentByte() == ',');
        if (source->currentByte() != ']')
        {
            throw new std::runtime_error("Missing terminating ']' for array.");
        }
        source->moveToNextByte();
        return (std::make_unique<JNodeArray>(std::move(array)));
    }
    std::unique_ptr<JNode> JSON::decodeJNodes(ISource *source)
    {
        ignoreWhiteSpace(source);
        switch (source->currentByte())
        {
        case '"':
            return (decodeString(source));
        case 't':
        case 'f':
            return (decodeBoolean(source));
        case 'n':
            return (decodeNull(source));
        case '{':
            return (decodeObject(source));
        case '[':
            return (decodeArray(source));
        default:
            return (decodeNumber(source));
        }
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    std::unique_ptr<JNode> JSON::decode(std::string jsonBuffer)
    {
        BufferSource source(jsonBuffer);
        return (decodeJNodes(&source));
    }
} // namespace H4
