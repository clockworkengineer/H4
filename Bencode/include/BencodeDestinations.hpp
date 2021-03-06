#ifndef BENCODESTINATIONS_HPP
#define BENCODESTINATIONS_HPP
#include <fstream>
#include <string>
namespace H4
{
    //
    // Destination classes for encoders.
    //
    class BufferDestination : public Bencode::IDestination
    {
    public:
        BufferDestination()
        {
        }
        void addBytes(std::string bytes)
        {
            for (auto byte : bytes)
            {
                m_encodeBuffer.add((std::byte)byte);
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
            m_destination.open(desinationFileName.c_str(), std::ios_base::binary);
            if (!m_destination.is_open())
            {
                throw std::runtime_error("Bencode file output stream failed to open or could not be created.");
            }
        }
        void addBytes(std::string bytes)
        {
            m_destination.write(bytes.c_str(), bytes.length());
            m_destination.flush();
        }
    private:
        std::ofstream m_destination;
    };
} // namespace H4
#endif /* BENCODESTINATIONS_HPP */