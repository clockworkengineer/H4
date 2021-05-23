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
    class ISource
    {
    public:
        virtual XMLChar current() = 0;
        virtual void next() = 0;
        virtual bool more() = 0;
        virtual void backup(long length) = 0;
        virtual long position() = 0;
        virtual std::string getRange(long start, long end) = 0;
        void ignoreWS()
        {
            while (more() && std::iswspace(current()))
            {
                next();
            }
        }
        bool match(const XMLString &targetString)
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
        long getLineNo()
        {
            return (m_lineNo);
        }
        long getColumnNo()
        {
            return (m_column);
        }
        std::string current_to_bytes()
        {
            return (m_UTF8.to_bytes(current()));
        }
        std::string to_bytes(const XMLString &from)
        {
            return (m_UTF8.to_bytes(from));
        }
        std::string to_bytes(const XMLChar &from)
        {
            return (m_UTF8.to_bytes(from));
        }
        XMLString from_bytes(const std::string &from)
        {
            return (m_UTF8.from_bytes(from));
        }

    protected:
        long m_lineNo = 1;
        long m_column = 1;
        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> m_UTF16;
        std::wstring_convert<std::codecvt_utf8_utf16<XMLString::value_type>, XMLString::value_type> m_UTF8;
    };

} // namespace H4
#endif /* ISOURCE_HPP */