#ifndef BENCODESOURCES_HPP
#define BENCODESOURCES_HPP
namespace H4
{
    //
    // Source classes for decoders.
    //
    class BufferSource : public Bencode::ISource
    {
    public:
        BufferSource(Bencoding sourceBuffer)
        {
            m_decodeBuffer = sourceBuffer;
        }
        std::byte currentByte()
        {
            return (m_decodeBuffer.bEncodedBuffer[m_bufferPosition]);
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
            return (m_bufferPosition < m_decodeBuffer.bEncodedBuffer.size());
        }
    private:
        std::size_t m_bufferPosition = 0;
        Bencoding m_decodeBuffer;
    };
    class FileSource : public Bencode::ISource
    {
    public:
        FileSource(std::string sourceFileName)
        {
            m_source.open(sourceFileName.c_str(), std::ios_base::binary);
            if (!m_source.is_open())
            {
                throw std::runtime_error("Bencode file input stream failed to open or does not exist.");
            }
        }
        std::byte currentByte()
        {
            return ((std::byte)m_source.peek());
        }
        void moveToNextByte()
        {
            char c;
            m_source.get(c);
        }
        bool bytesToDecode()
        {
            return (!m_source.eof());
        }
    private:
        std::ifstream m_source;
    };
} // namespace H4
#endif /* BENCODESOURCES_HPP */