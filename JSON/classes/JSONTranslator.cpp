//
// Class: JSONTranslator
//
// Description: Translate to/from JSON string escapes within source
// strings.
//
// Dependencies:   C17++ - Language standard features used.
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
#include <utility>
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
    //  of characters (UTF-8).
    /// </summary>
    /// <param name="jsonString">JSON string to process.</param>
    /// <returns>String with escapes translated.</returns>
    std::string JSONTranslator::fromEscapeSequences(const std::string &jsonString)
    {
        m_workBuffer.clear();
        auto current = jsonString.begin();
        while (current != jsonString.end())
        {
            if (*current != '\\')
            {
                m_workBuffer += *current++;
            }
            else
            {
                current++;
                if (current != jsonString.end())
                {
                    if (m_fromMap.count(*current) > 0)
                    {
                        m_workBuffer += m_fromMap[*current++];
                    }
                    else if ((*current == 'u') && ((current + 4) < jsonString.end()))
                    {
                        char hexDigits[5] = {current[1], current[2], current[3], current[4], '\0'};
                        char *end;
                        m_workBuffer += std::strtol(hexDigits, &end, 16);
                        if (*end != '\0')
                        {
                            throw std::runtime_error("JSON syntax error detected.");
                        }
                        current += 5; // Move paste the \uxxxx
                    }
                    else
                    {
                        throw std::runtime_error("JSON syntax error detected.");
                    }
                }
            }
        }
        int index = 0;
        while (index < ((int)m_workBuffer.size() - 1))
        {
            if ((m_workBuffer[index] >= 0xD800) && (m_workBuffer[index] <= 0xDBFF))
            {
                if ((m_workBuffer[index + 1] >= 0xDC00) && (m_workBuffer[index + 1] <= 0xDFFF))
                {
                    index += 2;
                    continue;
                }
                throw std::runtime_error("JSON syntax error detected.");
            }
            else if ((m_workBuffer[index] >= 0xDC00) && (m_workBuffer[index] <= 0xDFFF))
            {
                throw std::runtime_error("JSON syntax error detected.");
            }
            index++;
        }
        return (m_utf8Toutf16.to_bytes(m_workBuffer));
    }
    /// <summary>
    /// Convert a string from raw charater values (UTF8) so that it has character
    /// escapes where applicable for its JSON form.
    /// </summary>
    /// <param name="utf8String">String to convert.</param>
    /// <returns>JSON string with escapes.</returns>
    std::string JSONTranslator::toEscapeSequences(std::string const &utf8String)
    {
        m_escapedString.str("");
        m_escapedString.clear();
        m_workBuffer = m_utf8Toutf16.from_bytes(utf8String);
        for (char16_t utf16char : m_workBuffer)
        {   // Control characters
            if (m_toMap.count(utf16char) > 0)
            {
                m_escapedString << m_toMap[utf16char];
            }
            // ASCII
            else if ((utf16char > 0x1F) && (utf16char < 0x80))
            {
                m_escapedString << (char)utf16char;
            }
            // UTF8 escaped
            else
            {
                m_escapedString << "\\u" << std::hex << std::setfill('0') << std::setw(4) << (std::int32_t)utf16char;
            }
        }
        return (m_escapedString.str());
    };
} // namespace H4
