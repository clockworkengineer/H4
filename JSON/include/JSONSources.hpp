#ifndef JSONSOURCES_HPP
#define JSONSOURCES_HPP
namespace H4
{
    //
    // Source classes for decoders.
    //
    class BufferSource : public JSON::ISource
    {
    public:
        BufferSource(std::string sourceBuffer)
        {
            if (sourceBuffer.empty())
            {
                throw std::invalid_argument("Empty source buffer passed to be encoded.");
            }
            m_decodeBuffer = sourceBuffer;
        }
        char currentByte()
        {
            if (bytesToDecode())
            {
                return (m_decodeBuffer[m_bufferPosition]);
            }
            else
            {
                return (EOF);
            }
        }
        void moveToNextByte()
        {
            if (!bytesToDecode())
            {
                throw std::runtime_error("Decode buffer empty before decode complete.");
            }
            m_bufferPosition++;
        }
        bool bytesToDecode()
        {
            return (m_bufferPosition < m_decodeBuffer.size());
        }

    private:
        std::size_t m_bufferPosition = 0;
        std::string m_decodeBuffer;
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
        bool bytesToDecode()
        {
            return (m_source.peek() != EOF);
        }

    private:
        std::ifstream m_source;
    };
} // namespace H4
#endif /* JSONSOURCES_HPP */