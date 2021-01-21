//
// Class: JSONTranslator
//
// Description:
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
    // ==============
    // PUBLIC METHODS
    // ==============
    /// <summary>
    /// Convert any escape sequences in string to their correct sequence
    //  of characters (UTF-8).
    /// </summary>
    /// <param name="jsonString">JSON string to process.</param>
    /// <returns>Converted string.</returns>
    std::string JSONTranslator ::translateEscapesFromString(const std::string &jsonString)
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
    std::string JSONTranslator::translateEscapeToString(std::string const &utf8String)
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
    };
} // namespace H4
