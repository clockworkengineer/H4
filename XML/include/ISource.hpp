#ifndef ISOURCE_HHP
#define ISOURCE_HHP
//
// C++ STL
//
#include <string>
#include <codecvt>
#include <locale>
namespace H4
{
    //
    // Interface for reading source stream during XML/DTD parsing.
    //
    class ISource
    {
    public:
        // 32 bit characters internally
        using String = std::u32string;
        using Char = String::value_type;
        // Current character
        virtual Char current() = 0;
        // Move to next character
        virtual void next() = 0;
        // Are there still more characters to read ?
        virtual bool more() = 0;
        // Back length characters
        virtual void backup(long length) = 0;
        // Current character position within source stream
        virtual long position() = 0;
        // Return range of characters as a string from source stream
        virtual std::string getRange(long start, long end) = 0;
        // Reset to beginning of source stream
        virtual void reset() = 0;
        // Ignore shitespace on source stream
        void ignoreWS()
        {
            while (more() && std::iswspace(current()))
            {
                next();
            }
        }
        // Is current string a match at the current source stream position
        bool match(const String &targetString)
        {
            long index = 0;
            while (more() && current() == targetString[index])
            {
                next();
                if (++index == (long)targetString.length())
                {
                    return (true);
                }
            }
            backup(index);
            return (false);
        }
        // Current line number in source stream
        long getLineNo()
        {
            return (m_lineNo);
        }
        // Current column number in source stream
        long getColumnNo()
        {
            return (m_column);
        }
        // Read bytes representing the current character
        std::string current_to_bytes()
        {
            return (m_UTF8.to_bytes(current()));
        }
        // Convert to byte string (UTF-8)
        std::string to_bytes(const String &from)
        {
            return (m_UTF8.to_bytes(from));
        }
        // Convert to byte string (UTF-8)
        std::string to_bytes(const Char &from)
        {
            return (m_UTF8.to_bytes(from));
        }
        // Convert from byte string (UTF-8)
        String from_bytes(const std::string &from)
        {
            return (m_UTF8.from_bytes(from));
        }

    protected:
        long m_lineNo = 1;
        long m_column = 1;
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_UTF16;
        std::wstring_convert<std::codecvt_utf8_utf16<Char>, Char> m_UTF8;
    };
} // namespace H4
#endif /* ISOURCE_HPP */