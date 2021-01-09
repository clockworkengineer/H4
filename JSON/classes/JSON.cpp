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
    inline void JSON::ignoreWhiteSpace(ISource *source)
    {
        //   source->moveToNextByte();
        while (source->bytesToDecode() && source->currentByte() == ' ')
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
        std::string value{source->currentByte()};
        source->moveToNextByte();
        while (source->bytesToDecode() && std::isdigit(source->currentByte()))
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
    std::unique_ptr<JNode> JSON::decodeArray(ISource * /*source*/)
    {
        JNodeArray array;
        array.value.push_back(std::make_unique<JNodeNumber>(JNodeNumber("777")));
        array.value.push_back(std::make_unique<JNodeNumber>(JNodeNumber("9000")));
        array.value.push_back(std::make_unique<JNodeString>(JNodeString("apples")));
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
