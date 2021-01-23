//
// Class: JSONTranslator
//
// Description: Translate to/from JSON string escapes within source
// strings. This is the default translator but is possible to write 
// a custom one and pass it to the JSON class constructor to be used.
//
// Dependencies:   C20++ - Language standard features used.
//
// =================
// CLASS DEFINITIONS
// =================
#include "JSONConfig.hpp"
#include "JSON.hpp"
#include "JSONTranslator.hpp"
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
    /// <summary>
    /// Initialise tables used to convert to/from single character
    /// escape sequences within a JSON string.
    /// </summary>
    void JSONTranslator::initialiseTranslationMaps()
    {
        // From Escape sequence
        m_fromMap['\\'] = u"\\";
        m_fromMap['t'] = u"\t";
        m_fromMap['\"'] = u"\"";
        m_fromMap['/'] = u"/";
        m_fromMap['b'] = u"\b";
        m_fromMap['f'] = u"\f";
        m_fromMap['n'] = u"\n";
        m_fromMap['r'] = u"\r";
        // To Escape sequence
        m_toMap['\\'] = "\\\\";
        m_toMap['\t'] = "\\t";
        m_toMap['"'] = "\\\"";
        m_toMap['/'] = "\\/";
        m_toMap['\b'] = "\\b";
        m_toMap['\f'] = "\\f";
        m_toMap['\n'] = "\\n";
        m_toMap['\r'] = "\\r";
    }
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Convert any escape sequences in a string to their correct sequence
    //  of UTF-8 characters. If input string contains any unpaired surrogates
    //  then this is deemed as a syntax error and an error is duely thrown.
    /// </summary>
    /// <param name="jsonString">JSON string to process.</param>
    /// <returns>String with escapes translated.</returns>
    std::string JSONTranslator::fromEscapeSequences(const std::string &jsonString)
    {
        m_utf16workBuffer.clear();
        auto current = jsonString.begin();
        while (current != jsonString.end())
        {
            // Normal character
            if (*current != '\\')
            {
                m_utf16workBuffer += *current++;
            }
            else
            {
                // Escape sequence
                current++;
                if (current != jsonString.end())
                {
                    // Single character
                    if (m_fromMap.count(*current) > 0)
                    {
                        m_utf16workBuffer += m_fromMap[*current++];
                    }
                    // UTF16 "\uxxxx" 
                    else if ((*current == 'u') && ((current + 4) < jsonString.end()))
                    {
                        char hexDigits[5] = {current[1], current[2], current[3], current[4], '\0'};
                        char *end;
                        m_utf16workBuffer += std::strtol(hexDigits, &end, 16);
                        if (*end != '\0')
                        {
                            throw std::runtime_error("JSON 1 syntax error detected.");
                        }
                        current += 5; // Move paste the \uxxxx
                    }
                    else
                    {
                        throw std::runtime_error("JSON syntax error detected.");
                    }
                }
                else
                {
                    throw std::runtime_error("JSON syntax error detected.");
                }
            }
        }
        // Check that there are no single unpaired UTF-16 surrogates.From wht I see this is
        // meant to be an error but from searching the web I have not found a definitive answer.
        int index = 0;
        while (index <= ((int)m_utf16workBuffer.size() - 1))
        {
            if (isValidSurrogateUpper(m_utf16workBuffer[index]) && isValidSurrogateLower(m_utf16workBuffer[index + 1]))
            {
                index++;
            }
            else if (isValidSurrogateUpper(m_utf16workBuffer[index]) || isValidSurrogateLower(m_utf16workBuffer[index + 1]))
            {
                throw std::runtime_error("JSON syntax error detected.");
            }
            index++;
        }
        return (m_utf8Toutf16.to_bytes(m_utf16workBuffer));
    }
    /// <summary>
    /// Convert a string from raw charater values (UTF8) so that it has character
    /// escapes where applicable for its JSON form.
    /// </summary>
    /// <param name="utf8String">String to convert.</param>
    /// <returns>JSON string with escapes.</returns>
    std::string JSONTranslator::toEscapeSequences(std::string const &utf8String)
    {
        m_utf8workBuffer.clear();
        for (char16_t utf16char : m_utf8Toutf16.from_bytes(utf8String))
        {
            // Control characters
            if (m_toMap.count(utf16char) > 0)
            {
                m_utf8workBuffer += m_toMap[utf16char];
            }
            // ASCII
            else if ((utf16char > 0x1F) && (utf16char < 0x80))
            {
                m_utf8workBuffer += (char)utf16char;
            }
            // UTF8 escaped
            else
            {
                static const char *digits = "0123456789ABCDEF";
                m_utf8workBuffer += "\\u";
                m_utf8workBuffer += digits[(utf16char >> 12) & 0x0f];
                m_utf8workBuffer += digits[(utf16char >> 8) & 0x0f];
                m_utf8workBuffer += digits[(utf16char >> 4) & 0x0f];
                m_utf8workBuffer += digits[(utf16char)&0x0f];
            }
        }
        return (m_utf8workBuffer);
    };
} // namespace H4
