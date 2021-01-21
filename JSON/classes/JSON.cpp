//
// Class: JSON
//
// Description: Class to perform JSON encoding encode/decode to/from
// a byte buffer or file. It is also  possible to customize this with the
// ISource and IDestination interfaces if required. Note: At present it will
// report incorrect JSON syntax but will not be specific about what error has
// occurred; this is reasoned to add too much overhead to the process of parsing
// for the requirements of this library (this might change in future versions).
//
// Dependencies:   C17++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "JSONConfig.hpp"
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
#include <iomanip>
#include <iostream>

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
    /// <summary>
    /// Convert any escape sequences in string to their correct sequence
    //  of characters (UTF-8).
    /// </summary>
    /// <param name="jsonString">JSON string to process.</param>
    /// <returns>Converted string.</returns>
    std::string JSON::translateEscapesFromString(const std::string &jsonString)
    {
        m_escapedString.str("");
        m_escapedString.clear();
        auto it = jsonString.begin();
        while (it != jsonString.end())
        {
            if (*it != '\\')
            {
                m_escapedString << *it;
            }
            else
            {
                it++;
                if (it != jsonString.end())
                {
                    if (*it == 't')
                    {
                        m_escapedString << '\t';
                    }
                    else if (*it == '\"')
                    {
                        m_escapedString << '\"';
                    }
                    else if (*it == '\\')
                    {
                        m_escapedString << '\\';
                    }
                    else if (*it == '/')
                    {
                        m_escapedString << '/';
                    }
                    else if (*it == 'b')
                    {
                        m_escapedString << '\b';
                    }
                    else if (*it == 'f')
                    {
                        m_escapedString << '\f';
                    }
                    else if (*it == 'n')
                    {
                        m_escapedString << '\n';
                    }
                    else if (*it == 'r')
                    {
                        m_escapedString << '\r';
                    }
                    else if (*it == 't')
                    {
                        m_escapedString << '\t';
                    }
                    else if (*it == 'u')
                    {
                        it++;
                        char hexDigits[5];
                        for (auto number = 0; number < 4; number++)
                        {
                            hexDigits[number] = *it++;
                            if (it == jsonString.end() || (std::isxdigit(hexDigits[number]) == 0))
                            {
                                throw std::runtime_error("JSON syntax error detected.");
                            }
                        }
                        m_escapedString << m_utf8ToUnicode.to_bytes((int)std::stoi(hexDigits, 0, 16));
                        continue;
                    }
                    else
                    {
                        throw std::runtime_error("JSON syntax error detected.");
                    }
                }
            }
            it++;
        }
        return (m_escapedString.str());
    }
    /// <summary>
    /// Convert any escapable character sequence to their escape sequence representation.
    /// </summary>
    /// <param name="utf8String">String to convert.</param>
    /// <returns>Converted string.</returns>
    std::string JSON::translateEscapeToString(std::string const &utf8String)
    {
        m_escapedString.str("");
        m_escapedString.clear();
        std::u32string utf32String = m_utf8ToUnicode.from_bytes(utf8String);
        for (char32_t unicodeCharacter : utf32String)
        { // ASCII and control characters
            if (unicodeCharacter == '\\')
            { 
                m_escapedString << "\\\\";
            }
            else if (unicodeCharacter == '/')
            {
                m_escapedString << "\\/";
            }
            else if (unicodeCharacter == '"')
            {
                m_escapedString << "\\\"";
            }
            else if (unicodeCharacter == '\b')
            {
                m_escapedString << "\\b";
            }
            else if (unicodeCharacter == '\f')
            {
                m_escapedString << "\\f";
            }
            else if (unicodeCharacter == '\n')
            {
                m_escapedString << "\\n";
            }
            else if (unicodeCharacter == '\r')
            {
                m_escapedString << "\\r";
            }
            else if (unicodeCharacter == '\t')
            {
                m_escapedString << "\\t";
            }
            else if ((unicodeCharacter > 0x1F) && (unicodeCharacter < 0x80))
            {
                m_escapedString << (char)unicodeCharacter;
            }
            // UTF8 escaped
            else
            {
                m_escapedString << "\\u" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << (std::int32_t)unicodeCharacter;
            }
        }
        return (m_escapedString.str());
    }
    /// <summary>
    /// Move to next non-whitespace character in JSON encoded source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    inline void JSON::ignoreWhiteSpace(ISource *source)
    {
        while (source->bytesToDecode() && std::iswspace(source->currentByte()))
        {
            source->moveToNextByte();
        }
    }
    /// <summary>
    /// Extract a string from a JSON encoded source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns>Extracted string</returns>
    std::string JSON::extractString(ISource *source)
    {
        m_workBuffer.clear();
        source->moveToNextByte();
        while (source->bytesToDecode() && source->currentByte() != '"')
        {
            if (source->currentByte() == '\\')
            {
                m_workBuffer += source->currentByte();
                source->moveToNextByte();
            }
            m_workBuffer += source->currentByte();
            source->moveToNextByte();
        }
        if (!source->bytesToDecode())
        {
            throw std::runtime_error("JSON syntax error detected.");
        }
        source->moveToNextByte();
        return (m_workBuffer);
    }
    /// <summary>
    /// Decode a string from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::decodeString(ISource *source)
    {
        return (std::make_unique<JNodeString>(translateEscapesFromString(extractString(source))));
    }
    /// <summary>
    /// Decode a number from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::decodeNumber(ISource *source)
    {
        std::set<char> validCharacters{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', '-', '+', 'E', 'e'};
        if (validCharacters.count(source->currentByte()) == 0)
        {
            throw std::runtime_error("JSON syntax error detected.");
        }
        m_workBuffer.clear();
        m_workBuffer += source->currentByte();
        source->moveToNextByte();
        while (source->bytesToDecode() && validCharacters.count(source->currentByte()) > 0)
        {
            m_workBuffer += source->currentByte();
            source->moveToNextByte();
        }
        // Throw error if not valid integer or floating point
        char *end;
        std::strtoll(m_workBuffer.c_str(), &end, 10);
        if (*end != '\0')
        {
            std::strtod(m_workBuffer.c_str(), &end);
            if (*end != '\0')
            {
                throw std::runtime_error("JSON syntax error detected.");
            }
        }
        return (std::make_unique<JNodeNumber>(m_workBuffer));
    }
    /// <summary>
    /// Decode a boolean from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::decodeBoolean(ISource *source)
    {
        m_workBuffer.clear();
        m_workBuffer += source->currentByte();
        source->moveToNextByte();
        while (source->bytesToDecode() && std::isalpha(source->currentByte()))
        {
            m_workBuffer += source->currentByte();
            source->moveToNextByte();
        }
        if (m_workBuffer == "true")
        {
            return (std::make_unique<JNodeBoolean>(true));
        }
        else if (m_workBuffer == "false")
        {
            return (std::make_unique<JNodeBoolean>(false));
        }
        throw std::runtime_error("JSON syntax error detected.");
    }
    /// <summary>
    /// Decode a null from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::decodeNull(ISource *source)
    {
        m_workBuffer.clear();
        m_workBuffer += source->currentByte();
        source->moveToNextByte();
        while (source->bytesToDecode() && std::isalpha(source->currentByte()))
        {
            m_workBuffer += source->currentByte();
            source->moveToNextByte();
        }
        if (m_workBuffer == "null")
        {
            return (std::make_unique<JNodeNull>());
        }
        throw std::runtime_error("JSON syntax error detected.");
    }
    /// <summary>
    /// Decode an object from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::decodeObject(ISource *source)
    {
        JNodeObject object;
        do
        {
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            std::string key = translateEscapesFromString(extractString(source));
            ignoreWhiteSpace(source);
            if (source->currentByte() != ':')
            {
                throw std::runtime_error("JSON syntax error detected.");
            }
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            object.addEntry(key, decodeJNodes(source));
            ignoreWhiteSpace(source);
        } while (source->currentByte() == ',');
        if (source->currentByte() != '}')
        {
            throw std::runtime_error("JSON syntax error detected.");
        }
        source->moveToNextByte();
        return (std::make_unique<JNodeObject>(std::move(object)));
    }
    /// <summary>
    /// Decode an array from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::decodeArray(ISource *source)
    {
        JNodeArray array;
        do
        {
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            array.addEntry(decodeJNodes(source));
            ignoreWhiteSpace(source);
        } while (source->currentByte() == ',');
        if (source->currentByte() != ']')
        {
            throw std::runtime_error("JSON syntax error detected.");
        }
        source->moveToNextByte();
        return (std::make_unique<JNodeArray>(std::move(array)));
    }
    /// <summary>
    /// Recursively decode JSON source stream producing a JNode structure
    /// reprentation  of it.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
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
    /// <summary>
    /// Recursively traverse JNode structure encoding it into JSON on
    /// the destination stream passed in.
    /// </summary>
    /// <param name=jNode>JNode structure to be traversed</param>
    /// <param name=desination>destination stream for encoded JSON</param>
    /// <returns></returns>
    void JSON::encodeJNodes(JNode *jNode, IDestination *destination)
    {
        switch (jNode->nodeType)
        {
        case JNodeType::number:
            destination->addBytes(JNodeRef<JNodeNumber>(*jNode).getNumber());
            break;
        case JNodeType::string:
            destination->addBytes("\"" + translateEscapeToString(JNodeRef<JNodeString>(*jNode).getString()) + "\"");
            break;
        case JNodeType::boolean:
            destination->addBytes(JNodeRef<JNodeBoolean>(*jNode).getBoolean() ? "true" : "false");
            break;
        case JNodeType::null:
            destination->addBytes("null");
            break;
        case JNodeType::object:
        {
            int commaCount = JNodeRef<JNodeObject>(*jNode).size() - 1;
            destination->addBytes("{");
            for (auto key : JNodeRef<JNodeObject>(*jNode).getKeys())
            {
                destination->addBytes("\"" + translateEscapeToString(key) + "\"" + ":");
                encodeJNodes(JNodeRef<JNodeObject>(*jNode).getEntry(key), destination);
                if (commaCount-- > 0)
                {
                    destination->addBytes(",");
                }
            }
            destination->addBytes("}");
            break;
        }
        case JNodeType::array:
        {
            int commaCount = JNodeRef<JNodeArray>(*jNode).size() - 1;
            destination->addBytes("[");
            for (auto &bNodeEntry : JNodeRef<JNodeArray>(*jNode).getArray())
            {
                encodeJNodes(bNodeEntry.get(), destination);
                if (commaCount-- > 0)
                {
                    destination->addBytes(",");
                }
            }
            destination->addBytes("]");
            break;
        }
        default:
            throw std::runtime_error("Unknown JNode type encountered during encode.");
        }
    }
    /// <summary>
    /// Remove all whitespace characters from a JSON source stream and
    /// write them out to a JSON destination stream.
    /// </summary>
    /// <param name=source>JSON source stream</param>
    /// <param name=jNode>JSON destination stream</param>
    /// <returns></returns>
    void JSON::stripWhiteSpace(ISource *source, IDestination *destination)
    {
        while (source->bytesToDecode())
        {
            ignoreWhiteSpace(source);
            if (source->bytesToDecode())
            {
                if (source->currentByte() == '"')
                {
                    destination->addBytes("\"" + extractString(source) + "\"");
                }
                else
                {
                    destination->addBytes(std::string(1, source->currentByte()));
                    source->moveToNextByte();
                }
            }
        }
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Create JNode structure by recursively parsing JSON in a buffer.
    /// </summary>
    /// <param name=jsonBuffer>Buffer contains JSON to be parsed.</param>
    /// <returns>JNode structure.</returns>
    std::unique_ptr<JNode> JSON::decodeBuffer(const std::string &jsonBuffer)
    {
        if (jsonBuffer.empty())
        {
            throw std::invalid_argument("Empty string passed to be decoded.");
        }
        BufferSource source(jsonBuffer);
        return (decodeJNodes(&source));
    }
    /// <summary>
    /// Create JNode structure by recursively parsing JSON in a file.
    /// </summary>
    /// <param name=sourceFileName>JSON source file name</param>
    /// <returns>JNode structure.</returns>
    std::unique_ptr<JNode> JSON::decodeFile(const std::string &sourceFileName)
    {
        if (sourceFileName.empty())
        {
            throw std::invalid_argument("Empty file name passed to be decoded.");
        }
        FileSource source(sourceFileName);
        return (decodeJNodes(&source));
    }
    /// <summary>
    /// Recursively parse JNode structure and building its JSON before returning it.
    /// </summary>
    /// <param name="jNodeRoot">Root of JNode structure.</param>
    /// <returns>JSON string</returns>
    std::string JSON::encodeBuffer(std::unique_ptr<JNode> jNodeRoot)
    {
        if (jNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as JNode root to be encoded.");
        }
        BufferDestination destination;
        encodeJNodes(jNodeRoot.get(), &destination);
        return (destination.getBuffer());
    }
    /// <summary>
    /// Recursively parse JNode structure and building its JSON and writing it to a file.
    /// </summary>
    /// <param name="jNodeRoot">Root of JNode structure.</param>
    /// <param name="destination">Destination JSON file.</param>
    /// <returns></returns>
    void JSON::encodeFile(std::unique_ptr<JNode> jNodeRoot, const std::string &destinationFileName)
    {
        if (jNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as JNode root to be encoded.");
        }
        if (destinationFileName.empty())
        {
            throw std::invalid_argument("Empty file name passed to be encoded.");
        }
        FileDestination destination(std::move(destinationFileName));
        encodeJNodes(jNodeRoot.get(), &destination);
    }
    /// <summary>
    /// Remove all whitespace from a JSOn encoded buffer.
    /// </summary>
    /// <param name="jsonBuffer">Buffer of encoded JSON</param>
    /// <returns>Whitespace free JSON.</returns>
    std::string JSON::stripWhiteSpaceBuffer(const std::string &jsonBuffer)
    {
        BufferSource source(jsonBuffer);
        BufferDestination destination;
        stripWhiteSpace(&source, &destination);
        return (destination.getBuffer());
    }
} // namespace H4
