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
    long convertHexToLong(std::string::const_iterator current)
    {
        char hexDigits[5] = {current[1], current[2], current[3], current[4], '\0'};
        char *end;
        long hex = std::strtol(hexDigits, &end, 16);
        if (*end != '\0')
        {
            throw std::runtime_error("JSON syntax error detected.");
        }
        return (hex);
    }
    void JSONTranslator::initialiseTranslationMaps()
    {
        // From Escape sequence
        m_fromMap['\\'] = "\\";
        m_fromMap['t'] = "\t";
        m_fromMap['\"'] = "\"";
        m_fromMap['/'] = "/";
        m_fromMap['b'] = "\b";
        m_fromMap['f'] = "\f";
        m_fromMap['n'] = "\n";
        m_fromMap['r'] = "\r";
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
        m_escapedString.str("");
        m_escapedString.clear();
        auto current = jsonString.begin();
        while (current != jsonString.end())
        {
            if (*current != '\\')
            {
                m_escapedString << *current++;
            }
            else
            {
                current++;
                if (current != jsonString.end())
                {
                    if (m_fromMap.count(*current) > 0)
                    {
                        m_escapedString << m_fromMap[*current++];
                    }
                    else if ((*current == 'u') && ((current + 4) < jsonString.end()))
                    {
                        std::u16string utf16String;
                        utf16String += convertHexToLong(current);
                        if ((utf16String[0] >= 0xD800) && (utf16String[0] <= 0xDBFF))
                        {
                            current += 5;
                            if (*current == '\\')
                            {
                                current++;
                                if ((*current == 'u') && ((current + 4) < jsonString.end()))
                                {
                                    utf16String += convertHexToLong(current);
                                    if ((utf16String[1] < 0xDC00) || (utf16String[1] > 0xDFFF))
                                    {
                                        throw std::runtime_error("JSON syntax error detected.");
                                    }
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
                        else if ((utf16String[0] >= 0xDC00) && (utf16String[0] <= 0xDFFF))
                        {
                            throw std::runtime_error("JSON syntax error detected.");
                        }

                        m_escapedString << m_utf8Toutf16.to_bytes(utf16String);
                        current += 5; // Move paste the \uxxxx
                    }
                    else
                    {
                        throw std::runtime_error("JSON syntax error detected.");
                    }
                }
            }
        }
        return (m_escapedString.str());
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
        std::u16string utf16String = m_utf8Toutf16.from_bytes(utf8String);
        for (char16_t utf16char : utf16String)
        { // Control characters
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
