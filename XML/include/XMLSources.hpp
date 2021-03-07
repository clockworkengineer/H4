#ifndef XMLSOURCES_HPP
#define XMLSOURCES_HPP
#include <string>
#include <fstream>
namespace H4
{
    //
    // Source classes for parsers.
    //
    class BufferSource : public XML::ISource
    {
    public:
        BufferSource() {}
        BufferSource(const std::u16string &sourceBuffer)
        {
            if (sourceBuffer.empty())
            {
                throw std::invalid_argument("Empty source buffer passed to be parsed.");
            }
            std::u16string utf16xml{sourceBuffer};
            if (!utf16xml.starts_with(u"<?xml"))
            {
                for (char16_t &ch : utf16xml)
                {
                    ch = (static_cast<u_int16_t>(ch) >> 8) | (static_cast<u_int16_t>(ch) << 8);
                }
            }
            m_parseBuffer = m_UTF8.from_bytes(m_UTF16.to_bytes(utf16xml));
            convertCRLFToLF(m_parseBuffer);
        }
        BufferSource(const std::string &sourceBuffer)
        {
            if (sourceBuffer.empty())
            {
                throw std::invalid_argument("Empty source buffer passed to be parsed.");
            }
            m_parseBuffer = m_UTF8.from_bytes(sourceBuffer);
            convertCRLFToLF(m_parseBuffer);
        }
        XChar current()
        {
            if (more())
            {
                return (m_parseBuffer[m_bufferPosition]);
            }
            else
            {
                return (EOF);
            }
        }
        void next()
        {
            if (!more())
            {
                throw std::runtime_error("Parse buffer empty before parse complete.");
            }
            m_bufferPosition++;
            m_column++;
            if (current() == 0x0A)
            {
                m_lineNo++;
                m_column = 1;
            }
        }
        bool more()
        {
            return (m_bufferPosition < static_cast<long>(m_parseBuffer.size()));
        }
        void backup(long length)
        {
            m_bufferPosition -= length;
            if (m_bufferPosition < 0)
            {
                m_bufferPosition = 0;
            }
        }

    private:
        void convertCRLFToLF(XString &xmlString)
        {
            size_t pos = xmlString.find(U"\x0D\x0A");
            while (pos != std::string::npos)
            {
                xmlString.replace(pos, 2, U"\x0A");
                pos = xmlString.find(U"\x0D\x0A", pos + 1);
            }
        }
        long m_bufferPosition = 0;
        XString m_parseBuffer;
    };
    //
    // Source classes for parsers.
    //
    class FileSource : public XML::ISource
    {
    public:
        FileSource(const std::string &sourceFileName)
        {
            m_source.open(sourceFileName.c_str(), std::ios_base::binary);
            if (!m_source.is_open())
            {
                throw std::runtime_error("XML file input stream failed to open or does not exist.");
            }
            if (current() == 0x0D)
            {
                m_source.get();
                if (current() != 0x0A)
                {
                    m_source.unget();
                }
            }
        }
        XChar current()
        {
            return (m_source.peek());
        }
        void next()
        {
            if (!more())
            {
                throw std::runtime_error("Parse buffer empty before parse complete.");
            }
            m_source.get();
            if (current() == 0x0D)
            {
                m_source.get();
                if (current() != 0x0A)
                {
                    m_source.unget();
                }
            }
            m_column++;
            if (current() == 0x0A)
            {
                m_lineNo++;
                m_column = 1;
            }
        }
        bool more()
        {
            return (m_source.peek() != EOF);
        }
        void backup(long length)
        {
            if ((m_source.tellg() - length >= 0) || (current() == (XChar)EOF))
            {
                m_source.clear();
                m_source.seekg(-length, std::ios_base::cur);
            }
            else
            {
                m_source.seekg(0, std::ios_base::beg);
            }
        }

    private:
        std::ifstream m_source;
    };
} // namespace H4
#endif /* XMLSOURCES_HPP */