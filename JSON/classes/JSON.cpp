//
// Class: JSON
//
// Description: Class to perform JSON  encode/parse to/from a byte 
// or file. It is also  possible to customize this with the ISource
// and IDestination interfaces if required. Note: At present it will
// report incorrect JSON syntax but will not be specific about what 
// error has occurred; this is reasoned to add too much overhead to 
// the process of parsing for the requirements of this library 
// (this might change in future versions). For an indepth desciption
// of the JSON specification refer to its RFC at web address 
// https://tools.ietf.org/html/rfc8259.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "JSONConfig.hpp"
#include "JSON.hpp"
#include "JSONSources.hpp"
#include "JSONDestinations.hpp"
#include "JSONTranslator.hpp"
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
    static JSONTranslator defaultTranslator;
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
        while (source->bytesToParse() && std::iswspace(source->currentByte()))
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
        while (source->bytesToParse() && source->currentByte() != '"')
        {
            if (source->currentByte() == '\\')
            {
                m_workBuffer += source->currentByte();
                source->moveToNextByte();
            }
            m_workBuffer += source->currentByte();
            source->moveToNextByte();
        }
        if (!source->bytesToParse())
        {
            throw std::runtime_error("JSON syntax error detected.");
        }
        source->moveToNextByte();
        return (m_workBuffer);
    }
    /// <summary>
    /// Parse a string from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::parseString(ISource *source)
    {
        return (std::make_unique<JNodeString>(m_jsonTranslator->fromEscapeSequences(extractString(source))));
    }
    /// <summary>
    /// Parse a number from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::parseNumber(ISource *source)
    {
        std::set<char> validCharacters{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '.', '-', '+', 'E', 'e'};
        if (validCharacters.count(source->currentByte()) == 0)
        {
            throw std::runtime_error("JSON syntax error detected.");
        }
        m_workBuffer.clear();
        m_workBuffer += source->currentByte();
        source->moveToNextByte();
        while (source->bytesToParse() && validCharacters.count(source->currentByte()) > 0)
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
    /// Parse a boolean from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::parseBoolean(ISource *source)
    {
        m_workBuffer.clear();
        m_workBuffer += source->currentByte();
        source->moveToNextByte();
        while (source->bytesToParse() && std::isalpha(source->currentByte()))
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
    /// Parse a null from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::parseNull(ISource *source)
    {
        m_workBuffer.clear();
        m_workBuffer += source->currentByte();
        source->moveToNextByte();
        while (source->bytesToParse() && std::isalpha(source->currentByte()))
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
    /// Parse an object from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::parseObject(ISource *source)
    {
        JNodeObject object;
        do
        {
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            std::string key = m_jsonTranslator->fromEscapeSequences(extractString(source));
            ignoreWhiteSpace(source);
            if (source->currentByte() != ':')
            {
                throw std::runtime_error("JSON syntax error detected.");
            }
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            object.addEntry(key, parseJNodes(source));
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
    /// Parse an array from a JSON source stream.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::parseArray(ISource *source)
    {
        JNodeArray array;
        do
        {
            source->moveToNextByte();
            ignoreWhiteSpace(source);
            array.addEntry(parseJNodes(source));
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
    /// Recursively parse JSON source stream producing a JNode structure
    /// reprentation  of it.
    /// </summary>
    /// <param name="source">Source for JSON encoded bytes.</param>
    /// <returns></returns>
    std::unique_ptr<JNode> JSON::parseJNodes(ISource *source)
    {
        ignoreWhiteSpace(source);
        switch (source->currentByte())
        {
        case '"':
            return (parseString(source));
        case 't':
        case 'f':
            return (parseBoolean(source));
        case 'n':
            return (parseNull(source));
        case '{':
            return (parseObject(source));
        case '[':
            return (parseArray(source));
        default:
            return (parseNumber(source));
        }
    }
    /// <summary>
    /// Recursively traverse JNode structure encoding it into JSON on
    /// the destination stream passed in.
    /// </summary>
    /// <param name=jNode>JNode structure to be traversed</param>
    /// <param name=desination>destination stream for encoded JSON</param>
    /// <returns></returns>
    void JSON::stringifyJNodes(JNode *jNode, IDestination *destination)
    {
        switch (jNode->nodeType)
        {
        case JNodeType::number:
            destination->addBytes(JNodeRef<JNodeNumber>(*jNode).getNumber());
            break;
        case JNodeType::string:
            destination->addBytes("\"" + m_jsonTranslator->toEscapeSequences(JNodeRef<JNodeString>(*jNode).getString()) + "\"");
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
                destination->addBytes("\"" + m_jsonTranslator->toEscapeSequences(key) + "\"" + ":");
                stringifyJNodes(JNodeRef<JNodeObject>(*jNode).getEntry(key), destination);
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
                stringifyJNodes(bNodeEntry.get(), destination);
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
        while (source->bytesToParse())
        {
            ignoreWhiteSpace(source);
            if (source->bytesToParse())
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
    /// JSON constructor.
    /// </summary>
    /// <param name=translator>Custom JSON string translator.</param>
    /// <returns></returns>
    JSON::JSON(ITranslator *translator)
    {
        if (translator == nullptr)
        {
            m_jsonTranslator = &defaultTranslator;
        }
        else
        {
            m_jsonTranslator = translator;
        }
    }
    /// <summary>
    /// Create JNode structure by recursively parsing JSON in a buffer.
    /// </summary>
    /// <param name=jsonBuffer>Buffer contains JSON to be parsed.</param>
    /// <returns>JNode structure.</returns>
    std::unique_ptr<JNode> JSON::parseBuffer(const std::string &jsonBuffer)
    {
        if (jsonBuffer.empty())
        {
            throw std::invalid_argument("Empty string passed to be parsed.");
        }
        BufferSource source(jsonBuffer);
        return (parseJNodes(&source));
    }
    /// <summary>
    /// Create JNode structure by recursively parsing JSON in a file.
    /// </summary>
    /// <param name=sourceFileName>JSON source file name</param>
    /// <returns>JNode structure.</returns>
    std::unique_ptr<JNode> JSON::parseFile(const std::string &sourceFileName)
    {
        if (sourceFileName.empty())
        {
            throw std::invalid_argument("Empty file name passed to be parsed.");
        }
        FileSource source(sourceFileName);
        return (parseJNodes(&source));
    }
    /// <summary>
    /// Recursively parse JNode structure and building its JSON before returning it.
    /// </summary>
    /// <param name="jNodeRoot">Root of JNode structure.</param>
    /// <returns>JSON string</returns>
    std::string JSON::stringifyToBuffer(std::unique_ptr<JNode> jNodeRoot)
    {
        if (jNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as JNode root to be stringified.");
        }
        BufferDestination destination;
        stringifyJNodes(jNodeRoot.get(), &destination);
        return (destination.getBuffer());
    }
    /// <summary>
    /// Recursively parse JNode structure and building its JSON and writing it to a file.
    /// </summary>
    /// <param name="jNodeRoot">Root of JNode structure.</param>
    /// <param name="destination">Destination JSON file.</param>
    /// <returns></returns>
    void JSON::stringifyToFile(std::unique_ptr<JNode> jNodeRoot, const std::string &destinationFileName)
    {
        if (jNodeRoot == nullptr)
        {
            throw std::invalid_argument("Nullptr passed as JNode root to be stringified.");
        }
        if (destinationFileName.empty())
        {
            throw std::invalid_argument("Empty file name passed to be stringified.");
        }
        FileDestination destination(std::move(destinationFileName));
        stringifyJNodes(jNodeRoot.get(), &destination);
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
