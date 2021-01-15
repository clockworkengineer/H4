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
    /// Extract a string from a JSON encoed source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns>Extracted string</returns>
    std::string JSON::extractString(ISource *source)
    {
        m_workBuffer.clear();
        source->moveToNextByte();
        while (source->bytesToDecode() && source->currentByte() != '"')
        {
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
        return (std::make_unique<JNodeString>(JNodeString(extractString(source))));
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
        return (std::make_unique<JNodeNumber>(JNodeNumber(m_workBuffer)));
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
            return (std::make_unique<JNodeBoolean>(JNodeBoolean(true)));
        }
        else if (m_workBuffer == "false")
        {
            return (std::make_unique<JNodeBoolean>(JNodeBoolean(false)));
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
            return (std::make_unique<JNodeNull>(JNodeNull()));
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
            std::string key = extractString(source);
            ignoreWhiteSpace(source);
            if (source->currentByte() != ':')
            {
                throw std::runtime_error("JSON syntax error detected.");
            }
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            object.value[key] = decodeJNodes(source);
            object.keys.push_back(key);
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
            array.value.push_back(decodeJNodes(source));
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
            destination->addBytes(JNode::ref<JNodeNumber>(*jNode).value);
            break;
        case JNodeType::string:
            destination->addBytes("\"" + JNode::ref<JNodeString>(*jNode).value + "\"");
            break;
        case JNodeType::boolean:
            destination->addBytes(JNode::ref<JNodeBoolean>(*jNode).value ? "true" : "false");
            break;
        case JNodeType::null:
            destination->addBytes("null");
            break;
        case JNodeType::object:
        {
            int commaCount = JNode::ref<JNodeObject>(*jNode).value.size() - 1;
            destination->addBytes("{");
            for (auto key : JNode::ref<JNodeObject>(*jNode).keys)
            {
                destination->addBytes("\"" + key + "\"" + ":");
                encodeJNodes(JNode::ref<JNodeObject>(*jNode).value[key].get(), destination);
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
            int commaCount = JNode::ref<JNodeArray>(*jNode).value.size() - 1;
            destination->addBytes("[");
            for (auto &bNodeEntry : JNode::ref<JNodeArray>(*jNode).value)
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
