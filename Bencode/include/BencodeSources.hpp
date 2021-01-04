#ifndef BENCODESOURCES_HPP
#define BENCODESOURCES_HPP
namespace H4
{
    //
    // Source/Destination classes for encoder/decoder.
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
            return (m_decodeBuffer.buffer[m_bufferPosition]);
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
            return (m_bufferPosition < m_decodeBuffer.buffer.size());
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
            m_source.open(sourceFileName.c_str(), std::ios_base::in | std::ios_base::binary);
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
        std::fstream m_source;
    };
    class BufferDestination : public Bencode::IDestination
    {
    public:
        BufferDestination()
        {
        }
        void addBytes(std::string bytes)
        {
            for (auto b : bytes)
            {
                m_encodeBuffer.buffer.push_back((std::byte)b);
            }
        }
        Bencoding getBuffer()
        {
            return (m_encodeBuffer);
        }

    private:
        Bencoding m_encodeBuffer;
    };
    class FileDestination : public Bencode::IDestination
    {
    public:
        FileDestination(std::string desinationFileName)
        {
            m_destination.open(desinationFileName.c_str(), std::ios_base::out | std::ios_base::binary);
            if (!m_destination.is_open())
            {
                throw std::runtime_error("Bencode file output stream failed to open or could not be created.");
            }
        }
        void addBytes(std::string bytes)
        {
            m_destination.write(bytes.c_str(), bytes.length());
        }

    private:
        std::ofstream m_destination;
    };
} // namespace H4
#endif /* BENCODESOURCES_HPP */