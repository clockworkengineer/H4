#ifndef IDESTINATION_HHP
#define IDESTINATION_HHP
//
// C++ STL
//
#include <string>
#include <codecvt>
#include <locale>
namespace H4
{
    class IDestination
    {
    public:
        virtual void add(const std::string &bytes) = 0;
        virtual void add(const XChar) = 0;

    protected:
        std::wstring_convert<std::codecvt_utf8_utf16<XString::value_type>, XString::value_type> m_UTF8;
    };
} // namespace H4
#endif /* IDESTINATION_HHP */