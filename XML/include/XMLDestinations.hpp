#ifndef XMLDESTINATIONS_HPP
#define XMLDESTINATIONS_HPP
#include <fstream>
#include <string>
namespace H4
{
    //
    // Destination classes for stringification.
    //
    class BufferDestination : public XML::IDestination
    {
    public:
        BufferDestination()
        {
        }
        void add(const std::u32string &utf32string)
        {
            add(m_UTF8.to_bytes(utf32string));
        }
        void add(const std::string &bytes)
        {
            for (auto b : bytes)
            {
                m_stringifyBuffer.push_back(b);
            }
        }
        std::string getBuffer()
        {
            return (m_stringifyBuffer);
        }

    private:
        std::string m_stringifyBuffer;
    };
    class FileDestination : public XML::IDestination
    {
    public:
        FileDestination(const std::string &desinationFileName)
        {
            m_destination.open(desinationFileName.c_str(), std::ios_base::binary);
            if (!m_destination.is_open())
            {
                throw std::runtime_error("XML file output stream failed to open or could not be created.");
            }
        }
        void add(const std::u32string &utf32string)
        {
            add(m_UTF8.to_bytes(utf32string));
        }
        void add(const std::string &bytes)
        {
            m_destination.write(bytes.c_str(), bytes.length());
            m_destination.flush();
        }

    private:
        std::ofstream m_destination;
    };
} // namespace H4
#endif /* XMLDESTINATIONS_HPP */