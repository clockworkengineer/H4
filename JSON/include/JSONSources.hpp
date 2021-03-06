#ifndef JSONSOURCES_HPP
#define JSONSOURCES_HPP
#include <fstream>
#include <string>
namespace H4
{
    //
    // Source classes for parsers.
    //
    class BufferSource : public JSON::ISource
    {
    public:
        BufferSource(std::string sourceBuffer)
        {
            if (sourceBuffer.empty())
            {
                throw std::invalid_argument("Empty source buffer passed to be parsed.");
            }
            m_parseBuffer = sourceBuffer;
        }
        char currentByte()
        {
            if (bytesToParse())
            {
                return (m_parseBuffer[m_bufferPosition]);
            }
            else
            {
                return (EOF);
            }
        }
        void moveToNextByte()
        {
            if (!bytesToParse())
            {
                throw std::runtime_error("Parse buffer empty before parse complete.");
            }
            m_bufferPosition++;
        }
        bool bytesToParse()
        {
            return (m_bufferPosition < m_parseBuffer.size());
        }

    private:
        std::size_t m_bufferPosition = 0;
        std::string m_parseBuffer;
    };
    class FileSource : public JSON::ISource
    {
    public:
        FileSource(std::string sourceFileName)
        {
            m_source.open(sourceFileName.c_str(), std::ios_base::binary);
            if (!m_source.is_open())
            {
                throw std::runtime_error("JSON file input stream failed to open or does not exist.");
            }
        }
        char currentByte()
        {
            return (m_source.peek());
        }
        void moveToNextByte()
        {
            char c;
            m_source.get(c);
        }
        bool bytesToParse()
        {
            return (m_source.peek() != EOF);
        }

    private:
        std::ifstream m_source;
    };
} // namespace H4
#endif /* JSONSOURCES_HPP */